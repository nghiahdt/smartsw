#include "alexa.h"

AlexaSwitch::AlexaSwitch(unsigned int port)
  : _port(port)
  , _server(port)
{
}

AlexaSwitch* AlexaSwitch::begin(String alexaInvokeName, CallbackFunction oncb, CallbackFunction offcb)
{
	uint32_t chipId = ESP.getChipId();
	char uuid[64];
	sprintf_P(uuid, PSTR("38323636-4558-4dda-9188-cda0e6%02x%02x%02x"),
				(uint16_t) ((chipId >> 16) & 0xff),
				(uint16_t) ((chipId >>  8) & 0xff),
				(uint16_t)   chipId        & 0xff);
	_uuid = "Socket-1_0-" + String(uuid) + "-" + String(_port);		
	_name = alexaInvokeName;
	_onCallback = oncb;
	_offCallback = offcb;	
	_server.on("/", [&]() {
		_server.send(200, "text/plain", "You should tell Alexa to discover devices");
	});
	_server.on("/setup.xml", [&]() {
		IPAddress localIP = WiFi.localIP();
		char s[16];
		sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);
		String setup_xml = "<?xml version=\"1.0\"?>"
					"<root>"
					 "<device>"
							"<deviceType>urn:Belkin:device:controllee:1</deviceType>"
							"<friendlyName>"+ _name +"</friendlyName>"
							"<manufacturer>Belkin International Inc.</manufacturer>"
							"<modelName>Emulated Socket</modelName>"
							"<modelNumber>3.1415</modelNumber>"
							"<UDN>uuid:"+ _uuid +"</UDN>"
							"<serialNumber>221517K0101769</serialNumber>"
							"<binaryState>0</binaryState>"
							"<serviceList>"
								"<service>"
										"<serviceType>urn:Belkin:service:basicevent:1</serviceType>"
										"<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>"
										"<controlURL>/upnp/control/basicevent1</controlURL>"
										"<eventSubURL>/upnp/event/basicevent1</eventSubURL>"
										"<SCPDURL>/eventservice.xml</SCPDURL>"
								"</service>"
						"</serviceList>" 
						"</device>"
					"</root>\r\n"
					"\r\n";
		_server.send(200, "text/xml", setup_xml.c_str());
	});
	_server.on("/eventservice.xml", [&]() {
		String eventservice_xml = "<?scpd xmlns=\"urn:Belkin:service-1-0\"?>"
				"<actionList>"
					"<action>"
						"<name>SetBinaryState</name>"
						"<argumentList>"
							"<argument>"
								"<retval/>"
								"<name>BinaryState</name>"
								"<relatedStateVariable>BinaryState</relatedStateVariable>"
								"<direction>in</direction>"
							"</argument>"
						"</argumentList>"
						 "<serviceStateTable>"
							"<stateVariable sendEvents=\"yes\">"
								"<name>BinaryState</name>"
								"<dataType>Boolean</dataType>"
								"<defaultValue>0</defaultValue>"
							"</stateVariable>"
							"<stateVariable sendEvents=\"yes\">"
								"<name>level</name>"
								"<dataType>string</dataType>"
								"<defaultValue>0</defaultValue>"
							"</stateVariable>"
						"</serviceStateTable>"
					"</action>"
				"</scpd>\r\n"
				"\r\n";
		_server.send(200, "text/plain", eventservice_xml.c_str());
	});
	_server.on("/upnp/control/basicevent1", [&]() {
		String request = _server.arg(0);      
		if(request.indexOf("<BinaryState>1</BinaryState>") > 0) {
			_onCallback();
		}
		if(request.indexOf("<BinaryState>0</BinaryState>") > 0) {
			_offCallback();
		}	
		_server.send(200, "text/plain", "");
	});
	_server.begin();
	return this;
}

void AlexaSwitch::loop()
{
	_server.handleClient();
}

void AlexaSwitch::respondToSearch(IPAddress& senderIP, unsigned int senderPort)
{
	IPAddress localIP = WiFi.localIP();
	char s[16];
	sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);
	String response = 
			 "HTTP/1.1 200 OK\r\n"
			 "CACHE-CONTROL: max-age=86400\r\n"
			 "DATE: Sat, 26 Nov 2016 04:56:29 GMT\r\n"
			 "EXT:\r\n"
			 "LOCATION: http://" + String(s) + ":" + String(_port) + "/setup.xml\r\n"
			 "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
			 "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
			 "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
			 "ST: urn:Belkin:device:**\r\n"
			 "USN: uuid:" + _uuid + "::urn:Belkin:device:**\r\n"
			 "X-User-Agent: redsonic\r\n\r\n";
	WiFiUDP udp;
	udp.beginPacket(senderIP, senderPort);
	udp.write(response.c_str());
	udp.endPacket();
	Serial.println("Alexa device: " + _name + " (" + String(s) + ":" + String(_port) + ")");
}

AlexaSwitchManager* AlexaSwitchManager::_instance = nullptr;
AlexaSwitchManager* AlexaSwitchManager::getInstance()
{
	if (!_instance)
	{
		_instance = new AlexaSwitchManager();
	}
	return _instance;
}

void AlexaSwitchManager::begin()
{
	_udp.beginMulticast(WiFi.localIP(), IPAddress(239, 255, 255, 250), 1900);
}

void AlexaSwitchManager::loop()
{
	int packetSize = _udp.parsePacket();
	if (packetSize > 0)
	{
		auto buffer = new char[packetSize];
		_udp.read(buffer, packetSize);
		if(strstr(buffer, "M-SEARCH"))
		{
				if(strstr(buffer,"urn:Belkin:device:**"))
				{
					auto senderIP = _udp.remoteIP();
					unsigned int senderPort = _udp.remotePort();
					for (auto sw : _switches)
					{
						sw->respondToSearch(senderIP, senderPort);
						delay(100); //wait for send
					}
				}
		}
		delete[] buffer;
	}
	for (auto sw : _switches)
	{
		sw->loop();
	}
}

void AlexaSwitchManager::clearDevice()
{
	_switches.clear();
}

void AlexaSwitchManager::addDevice(AlexaSwitch* device)
{
	_switches.push_back(device);
}