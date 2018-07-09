#include "device.h"
#include "nrf.h"
#include "func.h"
#include <ArduinoJson.h>
#include <save.h>

void Hub::updateStatus(const std::string& text)
{
	_timeout = 10000;
	int num = _relays.size();
	if (text.length() == num + 1)
	{
		bool updated = false;
		for (int id=0; id < num; id++)
		{
			auto oldStatus = _relays[id].getStatus();
			auto newStatus = Relay::stringToStatus(text[id+1]);
			if (oldStatus != newStatus)
			{
				_relays[id].setStatus(newStatus);
				updated = true;
			}
		}
		if (updated)
		{
			Serial.println(String("Update hub ") + _id.c_str() + " = " + text.c_str());
		}
	}
}

void Hub::loop(int dt)
{
	if (_timeout > 0)
	{
		if (dt > _timeout)
		{
			dt = _timeout;
		}
		_timeout -= dt;
		if (_timeout == 0)
		{
			for (auto& relay : _relays)
			{
				relay.setStatus(Relay::Status::Offline);
			}
			Serial.println(String("Hub ") + _id.c_str() + " is offline");
		}
	}
	else
	{
		_timeout = 0;
	}
	sendWant();
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
		static String lastJson = "";
		if (json != lastJson)
		{
			Serial.println("Send: " + json);
			lastJson = json;
		}
	}
}

int Hub::getRelayCount()
{
	return _relays.size();
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
		_instance->_addNewHub = 0;
		Nrf.begin([](const std::string& text) {
			_instance->updateStatus(text, _instance->_addNewHub > 0);
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
	auto x = toStdString(root["x"].as<String>());
	auto m = root["m"].as<String>();
	auto s = toStdString(root["s"].as<String>());
	auto slaveListCount = "slaveListCount";
	auto slaveIdKey = "slave";
	if (m == "0")
	{
		bool inSlaveList = false;
		{
			int listCount = Save::getInstance()->getInt(slaveListCount, 0);
			for (int i=0; i<listCount; i++)
			{
				if (x == Save::getInstance()->getString(toStdString(slaveIdKey + String(i)).c_str()))
				{
					inSlaveList = true;
					Serial.println(String("Found slave in list: ") + x.c_str());
					break;
				}
			}
		}
		if (!inSlaveList && addNew)
		{
			int listCount = Save::getInstance()->getInt(slaveListCount, 0);
			Save::getInstance()->setString(toStdString(slaveIdKey + String(listCount)).c_str(), x.c_str(), false);
			Save::getInstance()->setInt(slaveListCount, listCount + 1, true);
			Serial.println(String("Add slave into list: ") + x.c_str());
			inSlaveList = true;
		}
		if (inSlaveList)
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
	}
	else if (m == ID::get())
	{
		bool found = false;
		for (auto& hub : _hubs)
		{
			if (hub.getId() == x)
			{
				hub.updateStatus(s);
				found = true;
				break;
			}
		}
		if (!found)
		{	
			int num = s[0] - '0';
			if (0 < num && num < 10)
			{
				Serial.println(String("Hub ") + x.c_str() + " have " + s[0] + " relays");
				Hub hub(x);
				for (int i=0; i<num; i++)
				{
					hub.add(Relay());
				}
				_hubs.push_back(hub);
			}
		}
	}
	else // remove x (change master)
	{
		int listCount = Save::getInstance()->getInt(slaveListCount, 0);
		int found = -1;
		for (int i=0; i<listCount; i++)
		{
			if (x == Save::getInstance()->getString(toStdString(slaveIdKey + String(i)).c_str()))
			{
				found = i;
				break;
			}
		}
		if (found >= 0)
		{
			for (int i=found; i<listCount-1; i++)
			{
				auto slave = Save::getInstance()->getString(toStdString(slaveIdKey + String(i+1)).c_str());
				Save::getInstance()->setString(toStdString(slaveIdKey + String(i)).c_str(), slave, false);
			}
			Save::getInstance()->setInt(slaveListCount, listCount - 1, true);
			Serial.println(String("Remove slave in list: ") + x.c_str());
		}
	}
}

void HubManager::loop(int dt)
{
	Nrf.loop();
	for (auto& hub : _hubs)
	{
		hub.loop(dt);
	}
	if (_addNewHub > 0)
	{
		if (dt > _addNewHub)
		{
			dt = _addNewHub;
		}
		_addNewHub -= dt;
		if (_addNewHub == 0)
		{
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

void HubManager::setAllRelayWant(Relay::Status status)
{
	for (auto& hub : _hubs)
	{
		for (int i = 0; i < hub.getRelayCount(); i++)
		{
			hub.getRelay(i)->setWant(status);
		}
	}
}

//---------------------------