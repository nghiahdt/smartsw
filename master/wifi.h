#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include <functional>
#include <vector>

extern void wait(int dt);

class Wifi
{
	Wifi();
	static Wifi* _instance;
	int64_t _lastTry;
	int _wifiMode;
	std::vector<std::function<void()>> _callbackConnecteds;
	std::vector<std::function<void()>> _callbackLoops;
	const int SaveWifi;
	const int PairMode;
	const int DefaultWifi;
	void setAsAP();
	void setAsClient(const char* name, const char* pass);
	bool pairServer();
public:
	static Wifi* getInstance();
	void addCallbackConnected(const std::function<void()>& func);
	void addCallbackLoop(const std::function<void()>& func);
	bool isConnected();
	bool loop();
	void httpRequest(const char* domain, const String& url, String& result);
};

#endif