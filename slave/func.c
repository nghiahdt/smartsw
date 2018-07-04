#include "func.h"

uint16_t getId()
{
	return 12345;
}

const char* getIdString()
{
	static char buff[8];
	itoa(getId(), buff, 8, 10);
	return buff;
}

void setMasterId(const char* id)
{

}

const char* getMasterId()
{
	return "54321";
}

const char* intToString(uint16_t num)
{
	static char buff[8];
	itoa(num, buff, 8, 10);
	return buff;
}

void uitoa(uint16_t value, char* string, int length, char radix)
{
	unsigned char index, i;
	index = length;
	i = 0;
	do
	{
		string[--index] = '0' + (value % radix);
		if ( string[index] > '9') string[index] += 'A' - ':';   /* continue with A, B,.. */
		value /= radix;
	} while (value != 0);
	do
	{
		string[i++] = string[index++];
	} while ( index < length );
	string[i] = 0; /* string terminator */
}

void itoa(int16_t value, char* string, int length, char radix)
{
	if (value < 0 && radix == 10)
	{
		*string++ = '-';
		uitoa(-value, string, length, radix);
	}
	else
	{
		uitoa(value, string, length, radix);
	}
}