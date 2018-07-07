#ifndef _FUNC_H_
#define _FUNC_H_

#include <Arduino.h>
#include <string>

class ID
{
public:
	static const String& get();
};

int getDt();
std::string toStdString(const String& input);

#endif
