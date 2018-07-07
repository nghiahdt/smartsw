#include "func.h"
#include <AdamRuddBase64.h>
#include <ESP8266WiFi.h>

const String& ID::get()
{
	static String sID = "12345";
	/*if (sID.length() <= 0)
	{
		String clientMac = "";
		unsigned char mac[6];
		WiFi.macAddress(mac);
		for (int i = 0; i < 6; ++i)
		{
			clientMac += String(mac[i], 16);
		}
		sID = clientMac;
	}*/
	return sID;
}

String base64Decode(const String& input)
{
	int inputLength = input.length() + 1;
	char c_input[inputLength];
	input.toCharArray(c_input, inputLength);
	int decodedLength = base64_dec_len(c_input, inputLength);
	char decodedOutput[decodedLength];
	base64_decode(decodedOutput, c_input, inputLength);
	return String(decodedOutput);
}

std::string toStdString(const String& input)
{
  int inputLength = input.length() + 1;
  char c_input[inputLength];
  input.toCharArray(c_input, inputLength);
  return c_input;
}
