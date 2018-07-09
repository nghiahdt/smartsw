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
		DontCare = 2,
		Offline = 3,
		X = 4
	};
	inline static char statusToString(Status status) { if (status < Status::X) return '0' + (char)status - (char)Status::Off; return '4'; }
	inline static Status stringToStatus(char c) { if ('0' <= c && c < '4') return (Status)(c - '0'); return Status::X; }
private:
	Status _status;
	Status _want;
public:
	Relay() : _status(Status::Off), _want(Status::DontCare) {}
	inline Status getStatus() const { return _status; }
	inline Status getWant() const { return _want; }
	inline void setStatus(Status status) { _status =  status; if (status == _want) _want = Status::DontCare; }
	inline void setWant(Status want) { _want = want; if (_want == _status) _want = Status::DontCare;  }
	inline bool needSend() const { return _want != Status::DontCare && _want != _status && _status != Status::Offline; }
};

class Hub
{
	std::vector<Relay> _relays;
	std::string _id;
	int _timeout;
	bool needSend() const;
	void sendWant() const;
public:
	Hub(const std::string& id) : _relays(), _id(id), _timeout(0) {}
	inline void add(const Relay& relay) { _relays.push_back(relay); }
	inline const std::string& getId() const { return _id; }
	void updateStatus(const std::string& text);
	void loop(int dt);
	int getRelayCount();
	Relay* getRelay(int id);
};

class HubManager
{
	HubManager() {}
	static HubManager* _instance;
	std::vector<Hub> _hubs;
	uint16_t _addNewHub;
	void updateStatus(const std::string& text, bool addNew);
public:
	static HubManager* getInstance();
	void loop(int dt);
	void setAdNewhub(bool value) { _addNewHub = value ? 60000 : 0; }
	Hub* getHub(const std::string& id);
	void setRelayWant(const std::string& hubId, int relayId, Relay::Status status);
	void setAllRelayWant(Relay::Status status);
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