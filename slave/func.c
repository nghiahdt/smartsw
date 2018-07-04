#include "func.h"

#define STM8S103F3
#ifdef STM8S103F3
typedef struct { uint16_t A, B, C, D, E, F; } UNIQUE_ID_t;
volatile __at(0x4865)  UNIQUE_ID_t UDID; //stm8s
uint16_t getId()
{
	return UDID.A ^ UDID.B ^ UDID.C ^ UDID.D ^ UDID.E ^ UDID.F;
}
#endif

const char* getIdString()
{
	static char buff[8];
	uitoa(getId(), buff, 8, 10);
	return buff;
}

static char masterIdString[8] = "0"; 
void setMasterId(uint16_t id)
{
	itoa(id, masterIdString, 8, 10);
}

const char* getMasterIdString()
{
	return masterIdString;
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

void uitoa32(uint32_t value, char* string, int length, char radix)
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

void itoa32(int32_t value, char* string, int length, char radix)
{
	if (value < 0 && radix == 10)
	{
		*string++ = '-';
		uitoa32(-value, string, length, radix);
	}
	else
	{
		uitoa32(value, string, length, radix);
	}
}