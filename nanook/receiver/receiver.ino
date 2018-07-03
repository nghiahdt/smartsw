#include <SPI.h>
#include <RF24.h>
 
const uint64_t pipe = 0xD7D7D7D7D7LL;
RF24 radio(9,10);
 
void setup(){
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}
 
void loop(){
  if (radio.available()){
    while (radio.available()){
      byte msg[32];
      radio.read(&msg, sizeof(msg));
      Serial.println((char*)msg);
    }
  }
}
