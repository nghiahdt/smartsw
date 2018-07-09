#include "save.h"
#include <EEPROM.h>

Save* Save::_instance = nullptr;

Save* Save::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new Save();
	}
	return _instance;
}

Save::Save()
{
	EEPROM.begin(4096);
	readEeprom();
}

#define eepromShift()  do { address++; if (address >= 4096) return; } while(false)

void Save::writeEeprom()
{
	int address = 0;
	int sizeInt = _mapInt.size();
	{
		EEPROM.write(address, (sizeInt >> 8) & 0xFF);
		eepromShift();
		EEPROM.write(address, (sizeInt >> 0) & 0xFF);
		eepromShift();
	}
	for (const auto& p : _mapInt)
	{
		auto key = p.first;
		{
			EEPROM.write(address, (key >> 8) & 0xFF);
			eepromShift();
			EEPROM.write(address, (key >> 0) & 0xFF);
			eepromShift();
		}
		auto value = p.second;
		{
			EEPROM.write(address, (value >> 24) & 0xFF);
			eepromShift();
			EEPROM.write(address, (value >> 16) & 0xFF);
			eepromShift();
			EEPROM.write(address, (value >> 8) & 0xFF);
			eepromShift();
			EEPROM.write(address, (value >> 0) & 0xFF);
			eepromShift();
		}
	}
	int sizeString = _mapString.size();
	{
		EEPROM.write(address, (sizeString >> 8) & 0xFF);
		eepromShift();
		EEPROM.write(address, (sizeString >> 0) & 0xFF);
		eepromShift();
	}
	for (const auto& p : _mapString)
	{
		auto key = p.first;
		{
			EEPROM.write(address, (key >> 8) & 0xFF);
			eepromShift();
			EEPROM.write(address, (key >> 0) & 0xFF);
			eepromShift();
		}
		auto value = p.second;
		{
			EEPROM.write(address, value.length());
			eepromShift();
			for (auto c : value)
			{
			    EEPROM.write(address, c);
			    eepromShift();
			}
		}
	}
	EEPROM.commit();
}

void Save::clearAll()
{
	_mapInt.clear();
	_mapString.clear();
}

void Save::readEeprom()
{
	clearAll();
	if (EEPROM.read(0) == 255) return;
	int address = 0;
	int sizeInt = 0;
	{
		sizeInt = EEPROM.read(address);
		eepromShift();
		sizeInt <<= 8;
		sizeInt |= EEPROM.read(address);
		eepromShift();
	}
	for (int i = 0; i < sizeInt; i++)
	{
		int16_t key = 0;
		{
			key |= EEPROM.read(address);
			eepromShift();
			key <<= 8;
			key |= EEPROM.read(address);
			eepromShift();
		}
		int32_t value = 0;
		{
			value |= EEPROM.read(address);
			eepromShift();
			value <<= 8;
			value |= EEPROM.read(address);
			eepromShift();
			value <<= 8;
			value |= EEPROM.read(address);
			eepromShift();
			value <<= 8;
			value |= EEPROM.read(address);
			eepromShift();
		}
		_mapInt[key] = value;
	}
	int sizeString = 0;
	{
		sizeString = EEPROM.read(address);
		eepromShift();
		sizeString <<= 8;
		sizeString |= EEPROM.read(address);
		eepromShift();
	}
	for (int i = 0; i < sizeString; i++)
	{
		int16_t key = 0;
		{
			key |= EEPROM.read(address);
			eepromShift();
			key <<= 8;
			key |= EEPROM.read(address);
			eepromShift();
		}
		std::string value;
		{
			int valueSize = EEPROM.read(address);
			eepromShift();
			for (int valueI = 0; valueI < valueSize; valueI++)
			{
				value += (char)(EEPROM.read(address));
				eepromShift();
			}
		}
		_mapString[key] = value;
	}
}

int16_t Save::calcHash(const char* key)
{
	std::map<std::string, int16_t> map;
	if (map.find(key) != map.end())
	{
		return map[key];
	}
	auto hash = _md5.make_hash(const_cast<char*>(key));
	int16_t value = hash[0];
	value <<= 8;
	value |= hash[1];
	free(hash);
	map[key] = value;
	if (static_cast<int>(map.size()) > 100)
	{
		map.clear();
	}
	return value;
}

void Save::setInt(int16_t hash, int32_t value, bool flush)
{
	if (_mapInt.find(hash) == _mapInt.end() || _mapInt[hash] != value)
	{
		_mapInt[hash] = value;
		if (flush)
		{
			writeEeprom();
		}
	}
}

int32_t Save::getInt(int16_t hash, int32_t _default)
{
	if (_mapInt.find(hash) != _mapInt.end())
	{
		return _mapInt[hash];
	}
	return _default;
}

void Save::setInt(const char* key, int32_t value, bool flush)
{
	setInt(calcHash(key), value, flush);
}

int32_t Save::getInt(const char* key, int32_t _default)
{
	return getInt(calcHash(key), _default);
}

void Save::setString(const char* key, const char* value, bool flush)
{
	auto hash = calcHash(key);
	if (_mapString.find(hash) == _mapString.end() || _mapString[hash] != value)
	{
		_mapString[hash] = value;
		if (flush)
		{
			writeEeprom();
		}
	}
}

const char* Save::getString(const char* key, const char* _default)
{
	auto hash = calcHash(key);
	if (_mapString.find(hash) != _mapString.end())
	{
		return _mapString[hash].c_str();
	}
	return _default;
}