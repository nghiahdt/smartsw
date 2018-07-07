#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include <functional>
#include <vector>

extern "C" {
#include "user_interface.h"
}

class AutoWifi
{
	AutoWifi();
	static AutoWifi* _instance;
	os_timer_t _timer;
	int64_t _lastTry;
	int _wifiMode;
	const int SaveWifi;
	const int PairMode;
	const int DefaultWifi;
	std::string _apName, _apPass, _dfName, _dfPass;
	int _apChannel;
	std::function<std::string()> _getNameFunc, _getPassFunc;
	std::function<void(const std::string&, const std::string&, const std::string&)> _pairServerCallback;
	std::string _deviceId;
	std::function<void(int)> _waitFunc;
	std::function<bool()> _isConnectedFunc;
	std::vector<std::function<void()>> _callbackConnecteds;
	std::vector<std::function<void()>> _callbackLoops;
	void setAsAP();
	void setAsClient(const char* name, const char* pass);
	bool pairServer();
	bool loop();

public:
	static AutoWifi* getInstance();
	void setValue(const std::string& apName, const std::string& apPass, int apChannel, const std::string& dfName, const std::string& dfPass
		, const std::function<std::string()>&  getNameFunc, const std::function<std::string()>&  getPassFunc);
	void setPairValue(const std::function<void(const std::string&, const std::string&, const std::string&)>& pairServerCallback
		, const std::string deviceId);
	void setWaitFunc(const std::function<void(int)>& waitFunc) { _waitFunc = waitFunc; }
	void setIsConnectedFunc(const std::function<bool()>& isConnectedFunc) { _isConnectedFunc = isConnectedFunc; }
	void addCallbackConnected(const std::function<void()>& func);
	void addCallbackLoop(const std::function<void()>& func);
	bool isConnected();
	void httpRequest(const char* domain, const String& url, String& result);
};

#endif