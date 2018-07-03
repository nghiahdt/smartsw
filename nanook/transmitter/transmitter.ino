#include <SPI.h>
#include <RF24.h>
 
const uint64_t pipe = 0xD7D7D7D7D7LL;
RF24 radio(9,10);
 
void setup(){ 
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
}
 
void loop(){
  byte msg[32] = { 'a', 'b', 'c', 0 };
  radio.write(&msg, 32);
  delay(100);
  Serial.print(".");
}
