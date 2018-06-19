#ifndef __Arduino_h
#define __Arduino_h

#include <stddef.h>
#include "../stm8s/stm8s.h"
#include "Rand.h"
#include "Serial.h"

const uint8_t PORTA = 0x00;
const uint8_t PORTB = 0x10;
const uint8_t PORTC = 0x20;
const uint8_t PORTD = 0x30;
const uint8_t PORTMASK = 0x70;

const uint8_t PIN0 = 0x00;
const uint8_t PIN1 = 0x01;
const uint8_t PIN2 = 0x02;
const uint8_t PIN3 = 0x03;
const uint8_t PIN4 = 0x04;
const uint8_t PIN5 = 0x05;
const uint8_t PIN6 = 0x06;
const uint8_t PIN7 = 0x07;
const uint8_t PINMASK = 0x07;

/* Values of pin parameter of pinMode() etc. */
const uint8_t PA1 = (PORTA | PIN1);
const uint8_t PA2 = (PORTA | PIN2);
const uint8_t PA3 = (PORTA | PIN3); // PWM
const uint8_t PB4 = (PORTB | PIN4); // I2C SCL
const uint8_t PB5 = (PORTB | PIN5); // I2C SDA
const uint8_t PC3 = (PORTC | PIN3); // PWM
const uint8_t PC4 = (PORTC | PIN4); // PWM, AIN
const uint8_t PC5 = (PORTC | PIN5); // PWM
const uint8_t PC6 = (PORTC | PIN6); // PWM
const uint8_t PC7 = (PORTC | PIN7); // PWM
const uint8_t PD1 = (PORTD | PIN1); // SWIM
const uint8_t PD2 = (PORTD | PIN2); // AIN
const uint8_t PD3 = (PORTD | PIN3); // PWM, AIN
const uint8_t PD4 = (PORTD | PIN4); // BEEP
const uint8_t PD5 = (PORTD | PIN5); // TX, AIN
const uint8_t PD6 = (PORTD | PIN6); // RX, AIN

const uint8_t BUILTIN_LED = PB5;

/* Values of mode parameter for pinMode() */
const uint8_t INPUT = 0;
const uint8_t OUTPUT = 1;
const uint8_t INPUT_PULLUP = 2;

/* Values of value parameter for digitalWrite() */
const uint8_t LOW = false;
const uint8_t HIGH = true;

/* Values of bitOrder parameter for shiftIn() and shiftOut() */
const uint8_t LSBFIRST = 0;
const uint8_t MSBFIRST = 1;

extern volatile uint32_t __us;
extern volatile uint32_t __ms;

void pinMode(uint8_t pin, uint8_t mode);
bool digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, bool value);
int16_t analogRead(uint8_t pin);
void analogWrite(uint8_t pin, uint8_t value);

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);
void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout = 1000000L);

void tone(uint8_t pin, uint16_t freq);
void noTone();

inline uint32_t micros() {
  uint8_t x = TIM1->CNTRL;
  return (__us | x);
}

inline uint32_t millis() {
  return __ms;
}

void delay_us(volatile uint16_t us);
void delay_us(volatile uint32_t us);
void delayMicroseconds(volatile uint32_t us);
void delay(volatile uint32_t ms);

template<typename T> T min(T val, T minimum) {
  if (val < minimum)
    return val;
  else
    return minimum;
}

template<typename T> T max(T val, T maximum) {
  if (val > maximum)
    return val;
  else
    return maximum;
}

template<typename T> T abs(T val) {
  if (val < 0)
    return -val;
  else
    return val;
}

template<typename T> T constrain(T val, T minimum, T maximum) {
  if (val < minimum)
    return minimum;
  else if (val > maximum)
    return maximum;
  else
    return val;
}

template<typename T> T map(T val, T inmin, T inmax, T outmin, T outmax) {
  return ((val - inmin) * (outmax - outmin) / (inmax - inmin) + outmin);
}

#endif
