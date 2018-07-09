#ifndef _SAVE_H_
#define _SAVE_H_

#include <Arduino.h>
#include <map>
#include <string>
#include <MacVicarMD5.h>

class Save
{
	Save();
	static Save* _instance;
	std::map<int16_t, int32_t> _mapInt;
	std::map<int16_t, std::string> _mapString;
	void writeEeprom();
	void readEeprom();
	MD5 _md5;
	int16_t calcHash(const char* key);
public:
	static Save* getInstance();
	void clearAll();
	void setInt(int16_t hash, int32_t value, bool flush = true);
	int32_t getInt(int16_t hash, int32_t _default = 0);
	void setInt(const char* key, int32_t value, bool flush = true);
	int32_t getInt(const char* key, int32_t _default = 0);
	void setString(const char* key, const char* value, bool flush = true);
	const char* getString(const char* key, const char* _default = "");
	void commit() { writeEeprom(); }
};

#endif
