#ifndef ALEXA_IOT_H
#define ALEXA_IOT_H
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUDP.h>
#include <functional>
#include <vector>
 
class AlexaSwitch
{
private:
	typedef std::function<void()> CallbackFunction;
	ESP8266WebServer _server;
	String _uuid;
	String _name;
	unsigned int _port;
	CallbackFunction _onCallback;
	CallbackFunction _offCallback;

public:
	AlexaSwitch(unsigned int port);
	void begin(String alexaInvokeName, CallbackFunction onCallback, CallbackFunction offCallback);
	void loop();
	void respondToSearch(IPAddress& senderIP, unsigned int senderPort);
};

class AlexaSwitchManager
{
	WiFiUDP _udp;
	std::vector<AlexaSwitch*> _switches;
	static AlexaSwitchManager* _instance;
	AlexaSwitchManager() {}
public:
	static AlexaSwitchManager* getInstance();
	void begin();
	void loop();
	void clearDevice();
	void addDevice(AlexaSwitch* device);
};
 
#endif
