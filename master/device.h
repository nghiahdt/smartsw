#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <Arduino.h>
#include <functional>
#include <vector>

class Hub;
class AlexaSwitch;

class Relay
{
public:
	enum class Status
	{
		Off = 0,
		On = 1,
		DontCare = 2
	};
	inline static char statusToString(Status status) { if (status == Status::Off) return '0'; else if (status == Status::On) return '1'; else return '2'; }
	inline static Status stringToStatus(char c) { if (c == '0') return Status::Off; else if (c == '1') return Status::On; else return Status::DontCare; }
private:
	Status _status;
	Status _want;
public:
	Relay() : _status(Status::Off), _want(Status::DontCare) {}
	inline Status getStatus() const { return _status; }
	inline Status getWant() const { return _want; }
	inline void setStatus(Status status) { _status =  status; if (status == _want) _want = Status::DontCare; }
	inline void setWant(Status want) { if (want != _status) _want =  want; }
	inline bool needSend() const { return _want != Status::DontCare && _want != _status; }
};

class Hub
{
	std::vector<Relay> _relays;
	std::string _id;
public:
	Hub(const std::string& id) : _relays(), _id(id) {}
	inline void add(const Relay& relay) { _relays.push_back(relay); }
	inline const std::string& getId() const { return _id; }
	void updateStatus(const std::string& text);
	bool needSend() const;
	void sendWant() const;
	Relay* getRelay(int id);
};

class HubManager
{
	HubManager() {}
	static HubManager* _instance;
	std::vector<Hub> _hubs;
	bool _addNewHub;
	void sendWant();
	void updateStatus(const std::string& text, bool addNew);
public:
	static HubManager* getInstance();
	void loop(int dt);
	void setAdNewhub(bool value) { _addNewHub = value; }
	Hub* getHub(const std::string& id);
	void setRelayWant(const std::string& hubId, int relayId, Relay::Status status);
};

//--------------------------

class Device
{
	std::string name;
	virtual void on() = 0;
	virtual void off() = 0;
	AlexaSwitch* alexa;
};

class Switch : public Device
{
	//hubid, relayid;
};

class Option : public Device
{
	//std::vector<Relay> relays;
};

class Group : public Device
{
	//std::vector<Switch> switchs;
};

#endif