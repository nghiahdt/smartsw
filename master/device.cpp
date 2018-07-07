#include "device.h"
#include "nrf.h"
#include "func.h"
#include <ArduinoJson.h>

void Hub::updateStatus(const std::string& text)
{
	int num = _relays.size();
	if (text.length() == num + 1)
	{
		for (int id=0; id < num; id++)
		{
			auto oldStatus = _relays[id].getStatus();
			auto newStatus = Relay::stringToStatus(text[id+1]);
			if (oldStatus != newStatus)
			{
				_relays[id].setStatus(newStatus);
				Serial.println(String("Update hub ") + _id.c_str() + " = " + text.c_str());
			}
		}
	}
}

bool Hub::needSend() const
{
	for (const auto& relay : _relays)
	{
		if (relay.needSend())
		{
			return true;
		}
	}
	return false;
}

void Hub::sendWant() const
{
	if (needSend())
	{
		std::string c;
		for (const auto& relay : _relays)
		{
			c += Relay::statusToString(relay.getWant());
			if (c[0] == '0') c = ""; // json number
		}
		if (c.empty()) c = "0";
		auto json = String() +
			"{"
				"'x':" + _id.c_str() + ","
				"'m':" + ID::get() + ","
				"'c':" + c.c_str() +
			"}";
		json.replace('\'', '\"');
		Nrf.send(json);
		Serial.println("Send: " + json);
	}
}

Relay* Hub::getRelay(int id)
{
	if (id < _relays.size())
	{
		return &_relays[id];
	}
	return nullptr;
}

HubManager* HubManager::_instance = nullptr;

HubManager* HubManager::getInstance()
{
	if (!_instance)
	{
		_instance = new HubManager();
		_instance->_addNewHub = false;
		Nrf.begin([](const std::string& text) {
			_instance->updateStatus(text, _instance->_addNewHub);
		});
	}
	return _instance;
}

void HubManager::updateStatus(const std::string& text, bool addNew)
{
	std::string json = text.substr(0, 15) + text.substr(16); ///////////////////
	DynamicJsonBuffer jsonBuffer(64);
	JsonObject& root = jsonBuffer.parseObject(json.c_str());
	if (!root.success())
	{
		return;
	}
	std::string x = toStdString(root["x"].as<String>());
	std::string m = toStdString(root["m"].as<String>());
	std::string s = toStdString(root["s"].as<String>());
	if (/*x is in slave list OR*/  addNew)
	{
		bool found = false;
		for (const auto& hub : _hubs)
		{
			if (hub.getId() == x)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{	
			int num = s[0] - '0';
			if (0 < num && num < 10)
			{
				Serial.println(String("Add hub ") + x.c_str() + " have " + s[0] + " relays");
				Hub hub(x);
				for (int i=0; i<num; i++)
				{
					hub.add(Relay());
				}
				_hubs.push_back(hub);
			}
		}
	}
	if (m == "0")
	{
		auto json = String() +
			"{"
				"'x':" + x.c_str() + ","
				"'m':" + ID::get() + ","
				"'c':22222"
			"}";
		json.replace('\'', '\"');
		Nrf.send(json);
	}
	for (auto& hub : _hubs)
	{
		if (hub.getId() == x)
		{
			hub.updateStatus(s);
		}
	}
}

void HubManager::sendWant()
{
	for (const auto& hub : _hubs)
	{
		if (hub.needSend())
		{
			hub.sendWant();
		}
	}
}

void HubManager::loop(int dt)
{
	Nrf.loop();
	sendWant();
	if (_addNewHub)
	{
		static uint16_t c = 0;
		c += dt;
		if (c >= 60000)
		{
			c = 0;			
			_addNewHub = false;
			Serial.println("Auto stop add new hub");
		}
	}
}

Hub* HubManager::getHub(const std::string& id)
{
	for (auto& hub : _hubs)
	{
		if (hub.getId() == id)
		{
			return &hub;
		}
	}
	return nullptr;
}

void HubManager::setRelayWant(const std::string& hubId, int relayId, Relay::Status status)
{
	auto hub = getHub(hubId);
	if (hub)
	{
		auto relay = hub->getRelay(relayId);
		if (relay)
		{
			relay->setWant(status);
		}
	}
}

//---------------------------