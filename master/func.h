#ifndef _FUNC_H_
#define _FUNC_H_

#include <Arduino.h>
#include <string>

class ID
{
public:
	static const String& get();
};

String base64Decode(const String& input);
std::string toStdString(const String& input);

#endif
