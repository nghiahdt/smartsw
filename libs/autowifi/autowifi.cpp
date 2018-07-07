#include "autowifi.h"
#include <ESP8266WiFi.h>
#include <AdamRuddBase64.h>
#include <time.h>

String wifiBase64Decode(const String& input)
{
	int inputLength = input.length() + 1;
	char c_input[inputLength];
	input.toCharArray(c_input, inputLength);
	int decodedLength = base64_dec_len(c_input, inputLength);
	char decodedOutput[decodedLength];
	base64_decode(decodedOutput, c_input, inputLength);
	return String(decodedOutput);
}

std::string wifiToStdString(const String& input)
{
  int inputLength = input.length() + 1;
  char c_input[inputLength];
  input.toCharArray(c_input, inputLength);
  return c_input;
}

WiFiServer server(80);
WiFiClient client;

AutoWifi* AutoWifi::_instance = nullptr;

AutoWifi* AutoWifi::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new AutoWifi();
	}
	return _instance;
}

AutoWifi::AutoWifi()
	: _lastTry(0)
	, _wifiMode(-1)
	, SaveWifi(0)
	, PairMode(1)
	, DefaultWifi(2)
	, _apName()
	, _apPass()
	, _apChannel(0)
	, _dfName()
	, _dfPass()
	, _deviceId()
	, _callbackConnecteds()
{
	_getNameFunc = [] () { return ""; };
	_getPassFunc = [] () { return ""; };
	_pairServerCallback = [] (const std::string& name, const std::string& pass, const std::string& account) {};
	_waitFunc = [] (int dt) { delay(dt); };
	_isConnectedFunc = [] () { return true; };
}

void AutoWifi::setValue(const std::string& apName, const std::string& apPass, int apChannel, const std::string& dfName, const std::string& dfPass
		, const std::function<std::string()>&  getNameFunc, const std::function<std::string()>&  getPassFunc)
{
	_apName = apName;
	_apPass = apPass;
	_apChannel = apChannel;
	_dfName = dfName;
	_dfPass = dfPass;
	_getNameFunc = getNameFunc;
	_getPassFunc = getPassFunc;
}

void AutoWifi::setPairValue(const std::function<void(const std::string&, const std::string&, const std::string&)>& pairServerCallback
		, const std::string deviceId)
{
	_pairServerCallback = pairServerCallback;
	_deviceId = deviceId;
}

void AutoWifi::addCallbackConnected(const std::function<void()>& func)
{
	_callbackConnecteds.push_back(func);
}

void AutoWifi::setAsAP()
{
	WiFi.mode(WIFI_AP);
	WiFi.softAP(_apName.c_str(), _apPass.c_str(), _apChannel);
	server.begin();
}

void AutoWifi::setAsClient(const char* name, const char* pass)
{
	WiFi.mode(WIFI_STA);
	WiFi.begin(name, pass);
	int i = 0;
	while (WiFi.status() != WL_CONNECTED && i++ <= 60)
	{
		_waitFunc(1000);
	}
	Serial.println("Wifi is connected");
	for (const auto& f : _callbackConnecteds)
	{
		f();
	}
}

bool AutoWifi::pairServer()
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
			_waitFunc(1);
		}
		if (i == maxI)
		{
			break;
		}
		String url = client.readStringUntil('\r');
		client.flush();
		if(url.indexOf("WIFI_NAME") != -1 && url.indexOf("WIFI_PASS") != -1 && url.indexOf("ACCOUNT") != -1)
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
			String account = url.substring(url.indexOf("ACCOUNT=") + 6);
			if (account.indexOf("&") != -1)
			{
				account = account.substring(0, account.indexOf("&"));
			}
			wifiName = wifiBase64Decode(wifiName);
			wifiPass = wifiBase64Decode(wifiPass);
			_pairServerCallback(wifiToStdString(wifiName), wifiToStdString(wifiPass), wifiToStdString(account));
			String json = String("{'wifisent':'ok','id':'") + _deviceId.c_str() + "'}";
			json.replace('\'', '\"');
			client.flush();
			client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
			client.print(json);
			client.flush();
			_waitFunc(3000);
			ESP.restart();
		}
		return true;
	} while(false);
	return false;
}

bool AutoWifi::isConnected()
{
	if (pairServer()) return false;
	if (WiFi.status() != WL_CONNECTED) return false;
	if (!_isConnectedFunc()) return false;
	return true;
}

bool AutoWifi::loop()
{
	if (isConnected())
	{
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
				setAsClient(_getNameFunc().c_str(), _getPassFunc().c_str());
			}
			else if (_wifiMode == PairMode)
			{
				setAsAP();
			}
			else if (_wifiMode == DefaultWifi)
			{
				setAsClient(_dfName.c_str(), _dfPass.c_str());
			}
		}
	}
}

void AutoWifi::httpRequest(const char* domain, const String& url, String& result)
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
}
