#include "wifi.h"
#include <time.h>
#include <ESP8266WiFi.h>
#include "conf.h"
#include "func.h"

WiFiServer server(80);
WiFiClient client;

Wifi* Wifi::_instance = nullptr;

Wifi* Wifi::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new Wifi();
	}
	return _instance;
}

Wifi::Wifi()
	: _lastTry(0)
	, _wifiMode(-1)
	, SaveWifi(0)
	, PairMode(1)
	, DefaultWifi(2)
{
}

void Wifi::addCallbackConnected(const std::function<void()>& func)
{
	_callbackConnecteds.push_back(func);
}

void Wifi::addCallbackLoop(const std::function<void()>& func)
{
	_callbackLoops.push_back(func);
}

void Wifi::setAsAP()
{
	WiFi.mode(WIFI_AP);
	WiFi.softAP(toStdString(WIFIAP_NAME).c_str(), WIFIAP_PASS, WIFIAP_CHANNEL);
	server.begin();
}

void Wifi::setAsClient(const char* name, const char* pass)
{
	WiFi.mode(WIFI_STA);
	WiFi.begin(name, pass);
	int i = 0;
	while (WiFi.status() != WL_CONNECTED && i++ <= 60)
	{
		wait(1000);
	}
	for (const auto& f : _callbackConnecteds)
	{
		f();
	}
}

bool Wifi::pairServer()
{
	do
	{
		if (_wifiMode != PairMode)
		{
			break;
		}
		static bool firstTime = true;
		if (firstTime)
		{
			firstTime = false;
			setAsAP();
		}
		client = server.available();
		if (!client) break;
		int i = 0;
		const int maxI = 100;
		for (i = 0; i < maxI; i++)
		{
			if (client.available()) break;
			wait(1);
		}
		if (i == maxI)
		{
			break;
		}
		String url = client.readStringUntil('\r');
		client.flush();
		/*if(url.indexOf("WIFI_NAME") != -1 && url.indexOf("WIFI_PASS") != -1 && url.indexOf("EMAIL") != -1)
		{
			String wifiName = url.substring(url.indexOf("WIFI_NAME=") + 10);
			if (wifiName.indexOf("&") != -1)
			{
				wifiName = wifiName.substring(0, wifiName.indexOf("&"));
			}
			String wifiPass = url.substring(url.indexOf("WIFI_PASS=") + 10);
			if (wifiPass.indexOf("&") != -1)
			{
				wifiPass = wifiPass.substring(0, wifiPass.indexOf("&"));
			}
			String email = url.substring(url.indexOf("EMAIL=") + 6);
			if (email.indexOf("&") != -1)
			{
				email = email.substring(0, email.indexOf("&"));
			}
			wifiName = base64Decode(wifiName);
			wifiPass = base64Decode(wifiPass);
			SavingData::getInstance()->setString("wifiname", toStdString(wifiName).c_str(), false);
			SavingData::getInstance()->setString("wifipass", toStdString(wifiPass).c_str(), false);
			SavingData::getInstance()->setString("email", toStdString(email).c_str(), true); //buzz 1s
			String json = "{'wifisent':'ok','plantid':'" + ID::get() + "'}";
			json.replace('\'', '\"');
			client.flush();
			client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
			client.print(json);
			client.flush();
			Realtime::getInstance()->wait(10000, Shield::getInstance()->getDateTime());
			ESP.restart();
		}*/
		return true;
	} while(false);
	return false;
}

bool Wifi::isConnected()
{
	if (pairServer()) return false;
	if (WiFi.status() != WL_CONNECTED) return false;
	/*if (!mqttClient.connected())
	{
		if (mqttClient.connect(getTopicName()))
		{
			if (mqttClient.subscribe(getTopicName()))
			{
				#ifdef SERIAL_DEBUG
				Serial.println("MQTT connect OK");
				Serial.print("MQTT subscribe topic: ");
				Serial.println(getTopicName());
				#endif
				return true;
			}
		}
		return false;
	}*/
	return true;
}

bool Wifi::loop()
{
	if (isConnected())
	{
		//mqttClient.loop();
	}
	else
	{
		int64_t _millis = millis();
		int wait = 0;
		if (_wifiMode == SaveWifi)
		{
			wait = 2;
		}
		else if (_wifiMode == PairMode)
		{
			wait = 5;
		}
		else if (_wifiMode == DefaultWifi)
		{
			wait = 2;
		}
		if (_millis - _lastTry > wait*60*1000)
		{
			_lastTry = _millis;
			_wifiMode++;
			if (_wifiMode > DefaultWifi)
			{
				_wifiMode = SaveWifi;
			}
			if (_wifiMode == SaveWifi)
			{
				setAsClient(
					//SavingData::getInstance()->getString("wifiname"), SavingData::getInstance()->getString("wifipass")
					WIFI_DEFAULT_NAME, WIFI_DEFAULT_PASS
					);
			}
			else if (_wifiMode == PairMode)
			{
				setAsAP();
			}
			else if (_wifiMode == DefaultWifi)
			{
				setAsClient(WIFI_DEFAULT_NAME, WIFI_DEFAULT_PASS);
			}
		}
	}
	for (const auto& f : _callbackLoops)
	{
		f();
	}
}


void Wifi::httpRequest(const char* domain, const String& url, String& result)
{
	if (pairServer()) return;
	String error;
	do
	{
		WiFiClient browser;
		if (!browser.connect(domain, 80))
		{
			error = "connection failed";
			break;
		}
		browser.print(String("GET ") + url + " HTTP/1.1\r\n" +
							 "Host: " + domain + "\r\n" + 
							 "Connection: close\r\n\r\n");
		int64_t timeout = millis();
		bool isTimeout = false;
		while (browser.available() == 0)
		{
			auto diff = millis() - timeout;
			if (diff < 0 || diff > 1000)
			{
				error = "timeout";
				browser.stop();
				isTimeout = true;
				break;
			}
		}
		if (isTimeout) break;
		while(browser.available())
		{
			result.concat(browser.readStringUntil('\r'));
		}
		return;
	} while (false);
	//debug("Web Browser ERROR: ");
	//debug(toStdString(error).c_str());
}
