#include "Arduino.h"

static GPIO_TypeDef* pinToGPIO(uint8_t pin) {
  pin &= PORTMASK;
  if (pin == PORTA)
    return GPIOA;
  else if (pin == PORTB)
    return GPIOB;
  else if (pin == PORTC)
    return GPIOC;
  else if (pin == PORTD)
    return GPIOD;
  else
    return NULL;
}

void pinMode(uint8_t pin, uint8_t mode) {
  if (pin == PD1) { // Disable SWIM
    CFG->GCR |= CFG_GCR_SWD;
  }

  GPIO_TypeDef* GPIOx = pinToGPIO(pin);
  pin = 1 << (pin & PINMASK);

  GPIOx->CR2 &= (uint8_t)(~pin); // No external interrupt or No slope control
  if (mode == OUTPUT) {
    GPIOx->DDR |= pin; // Set Output mode
    GPIOx->CR1 |= pin; // Push-Pull
  } else {
    GPIOx->DDR &= (uint8_t)(~pin); // Set Input mode
    if (mode == INPUT_PULLUP)
      GPIOx->CR1 |= pin; // Pull-Up
    else
      GPIOx->CR1 &= (uint8_t)(~pin); // Float
  }
}

bool digitalRead(uint8_t pin) {
  GPIO_TypeDef* GPIOx = pinToGPIO(pin);
  pin = 1 << (pin & PINMASK);

  if (GPIOx->DDR & pin)
    return (bool)((GPIOx->ODR & pin) != 0);
  else
    return (bool)((GPIOx->IDR & pin) != 0);
}

void digitalWrite(uint8_t pin, bool value) {
  if (pin == PC6) {
    TIM1->CCER1 &= (uint8_t)(~(TIM1_CCER1_CC1E | TIM1_CCER1_CC1NE)); // Disable the TIM1 Channel 1: Reset the CCE Bit
  } else if (pin == PC7) {
    TIM1->CCER1 &= (uint8_t)(~(TIM1_CCER1_CC2E | TIM1_CCER1_CC2NE)); // Disable the TIM1 Channel 2: Reset the CCE Bit
  } else if (pin == PC3) {
    TIM1->CCER2 &= (uint8_t)(~(TIM1_CCER2_CC3E | TIM1_CCER2_CC3NE)); // Disable the TIM1 Channel 3: Reset the CCE Bit
  } else if (pin == PC4) {
    TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC4E); // Disable the TIM1 Channel 4: Reset the CCE Bit
  } else if (pin == PC5) {
    TIM2->CCER1 &= (uint8_t)(~TIM2_CCER1_CC1E); // Disable the TIM2 Channel 1: Reset the CCE Bit
  } else if (pin == PD3) {
    TIM2->CCER1 &= (uint8_t)(~TIM2_CCER1_CC2E); // Disable the TIM2 Channel 2: Reset the CCE Bit
  } else if (pin == PA3) {
    TIM2->CCER2 &= (uint8_t)(~TIM2_CCER2_CC3E); // Disable the TIM2 Channel 3: Reset the CCE Bit
  }

  GPIO_TypeDef* GPIOx = pinToGPIO(pin);
  pin = 1 << (pin & PINMASK);

  if (value)
    GPIOx->ODR |= pin;
  else
    GPIOx->ODR &= (uint8_t)(~pin);
}

int16_t analogRead(uint8_t pin) {
  uint8_t channel;

  if (pin == PC4) {
    channel = 2;
  } else if (pin == PD2) {
    channel = 3;
  } else if (pin == PD3) {
    channel = 4;
  } else if (pin == PD5) {
    channel = 5;
  } else if (pin == PD6) {
    channel = 6;
  } else
    return -1;

  GPIO_TypeDef* GPIOx = pinToGPIO(pin);
  pin = 1 << (pin & PINMASK);

  GPIOx->CR2 &= (uint8_t)(~pin); // No external interrupt or No slope control
  GPIOx->DDR &= (uint8_t)(~pin); // Set Input mode
  GPIOx->CR1 &= (uint8_t)(~pin); // Float

  ADC1->TDRL |= (uint8_t)((uint8_t)0x01 << (uint8_t)channel); // ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_CHANNELn, DISABLE)
  ADC1->CSR &= (uint8_t)(~(ADC1_CSR_CH | ADC1_CSR_EOC)); // Clear the ADC1 channels
  ADC1->CSR |= (uint8_t)channel; // Select the ADC1 channel
  ADC1->CR1 |= ADC1_CR1_ADON; // ADC1_StartConversion()
  while (! (ADC1->CSR & ADC1_CSR_EOC)); // Wait for End Of Convertion

  uint8_t adcl, adch;

  adcl = ADC1->DRL; // Read LSB first
  adch = ADC1->DRH; // Then read MSB
//  ADC1->CR1 &= (uint8_t)(~ADC1_CR1_ADON); // ADC1_Cmd(DISABLE)

  return (int16_t)(adcl | (int16_t)(adch << 8));
}

void analogWrite(uint8_t pin, uint8_t value) {
  if (pin == PC6) { // AFR0 must be set in Option Bytes!
    TIM1->CCMR1 &= (uint8_t)(~TIM1_CCMR_OCM);
    if (value == 0) {
      TIM1->CCMR1 |= (uint8_t)0x40; // Force inactive level
    } else if (value == 255) {
      TIM1->CCMR1 |= (uint8_t)0x50; // Force active level
    } else {
      TIM1->CCMR1 |= (uint8_t)0x60; // PWM mode 1
    }
    TIM1->CCER1 &= (uint8_t)(~(TIM1_CCER1_CC1E | TIM1_CCER1_CC1NE | TIM1_CCER1_CC1P | TIM1_CCER1_CC1NP)); // Disable the TIM1 Channel 1: Reset the CCE Bit
    TIM1->CCER1 |= (uint8_t)TIM1_CCER1_CC1E; // Set the Output State to TIM1_OUTPUTSTATE_ENABLE
    TIM1->CCR1H = (uint8_t)0; // Set the Pulse value
    TIM1->CCR1L = (uint8_t)value;
  } else if (pin == PC7) { // AFR0 must be set in Option Bytes!
    TIM1->CCMR2 &= (uint8_t)(~TIM1_CCMR_OCM);
    if (value == 0) {
      TIM1->CCMR2 |= (uint8_t)0x40; // Force inactive level
    } else if (value == 255) {
      TIM1->CCMR2 |= (uint8_t)0x50; // Force active level
    } else {
      TIM1->CCMR2 |= (uint8_t)0x60; // PWM mode 1
    }
    TIM1->CCER1 &= (uint8_t)(~(TIM1_CCER1_CC2E | TIM1_CCER1_CC2NE | TIM1_CCER1_CC2P | TIM1_CCER1_CC2NP)); // Disable the TIM1 Channel 2: Reset the CCE Bit
    TIM1->CCER1 |= (uint8_t)TIM1_CCER1_CC2E; // Set the Output State to TIM1_OUTPUTSTATE_ENABLE
    TIM1->CCR2H = (uint8_t)0; // Set the Pulse value
    TIM1->CCR2L = (uint8_t)value;
  } else if (pin == PC3) {
    TIM1->CCMR3 &= (uint8_t)(~TIM1_CCMR_OCM);
    if (value == 0) {
      TIM1->CCMR3 |= (uint8_t)0x40; // Force inactive level
    } else if (value == 255) {
      TIM1->CCMR3 |= (uint8_t)0x50; // Force active level
    } else {
      TIM1->CCMR3 |= (uint8_t)0x60; // PWM mode 1
    }
    TIM1->CCER2 &= (uint8_t)(~(TIM1_CCER2_CC3E | TIM1_CCER2_CC3NE | TIM1_CCER2_CC3P | TIM1_CCER2_CC3NP)); // Disable the TIM1 Channel 3: Reset the CCE Bit
    TIM1->CCER2 |= (uint8_t)TIM1_CCER2_CC3E; // Set the Output State to TIM1_OUTPUTSTATE_ENABLE
    TIM1->CCR3H = (uint8_t)0; // Set the Pulse value
    TIM1->CCR3L = (uint8_t)value;
  } else if (pin == PC4) {
    TIM1->CCMR4 &= (uint8_t)(~TIM1_CCMR_OCM);
    if (value == 0) {
      TIM1->CCMR4 |= (uint8_t)0x40; // Force inactive level
    } else if (value == 255) {
      TIM1->CCMR4 |= (uint8_t)0x50; // Force active level
    } else {
      TIM1->CCMR4 |= (uint8_t)0x60; // PWM mode 1
    }
    TIM1->CCER2 &= (uint8_t)(~(TIM1_CCER2_CC4E | TIM1_CCER2_CC4P)); // Disable the TIM1 Channel 4: Reset the CCE Bit
    TIM1->CCER2 |= (uint8_t)TIM1_CCER2_CC4E; // Set the Output State to TIM1_OUTPUTSTATE_ENABLE
    TIM1->CCR4H = (uint8_t)0; // Set the Pulse value
    TIM1->CCR4L = (uint8_t)value;
  } else if (pin == PC5) { // AFR0 must be set in Option Bytes!
    TIM2->CCMR1 &= (uint8_t)(~TIM2_CCMR_OCM);
    if (value == 0) {
      TIM2->CCMR1 |= (uint8_t)0x40; // Force inactive level
    } else if (value == 255) {
      TIM2->CCMR1 |= (uint8_t)0x50; // Force active level
    } else {
      TIM2->CCMR1 |= (uint8_t)0x60; // PWM mode 1
    }
    TIM2->CCER1 &= (uint8_t)(~(TIM2_CCER1_CC1E | TIM2_CCER1_CC1P)); // Disable the TIM2 Channel 1: Reset the CCE Bit
    TIM2->CCER1 |= (uint8_t)TIM2_CCER1_CC1E; // Set the Output State to TIM2_OUTPUTSTATE_ENABLE
    TIM2->CCR1H = (uint8_t)0; // Set the Capture Compare1 Register value
    TIM2->CCR1L = (uint8_t)value;
  } else if (pin == PD3) {
    TIM2->CCMR2 &= (uint8_t)(~TIM2_CCMR_OCM);
    if (value == 0) {
      TIM2->CCMR2 |= (uint8_t)0x40; // Force inactive level
    } else if (value == 255) {
      TIM2->CCMR2 |= (uint8_t)0x50; // Force active level
    } else {
      TIM2->CCMR2 |= (uint8_t)0x60; // PWM mode 1
    }
    TIM2->CCER1 &= (uint8_t)(~(TIM2_CCER1_CC2E | TIM2_CCER1_CC2P)); // Disable the TIM2 Channel 2: Reset the CCE Bit
    TIM2->CCER1 |= (uint8_t)TIM2_CCER1_CC2E; // Set the Output State to TIM2_OUTPUTSTATE_ENABLE
    TIM2->CCR2H = (uint8_t)0; // Set the Capture Compare2 Register value
    TIM2->CCR2L = (uint8_t)value;
  } else if (pin == PA3) {
    TIM2->CCMR3 &= (uint8_t)(~TIM2_CCMR_OCM);
    if (value == 0) {
      TIM2->CCMR3 |= (uint8_t)0x40; // Force inactive level
    } else if (value == 255) {
      TIM2->CCMR3 |= (uint8_t)0x50; // Force active level
    } else {
      TIM2->CCMR3 |= (uint8_t)0x60; // PWM mode 1
    }
    TIM2->CCER2 &= (uint8_t)(~(TIM2_CCER2_CC3E | TIM2_CCER2_CC3P)); // Disable the TIM2 Channel 3: Reset the CCE Bit
    TIM2->CCER2 |= (uint8_t)TIM2_CCER2_CC3E; // Set the Output State to TIM2_OUTPUTSTATE_ENABLE
    TIM2->CCR3H = (uint8_t)0; // Set the Capture Compare3 Register value
    TIM2->CCR3L = (uint8_t)value;
  }
}

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
  GPIO_TypeDef* GPIOxData = pinToGPIO(dataPin);
  GPIO_TypeDef* GPIOxClock = pinToGPIO(clockPin);
  uint8_t result = 0;

  dataPin = 1 << (dataPin & PINMASK);
  clockPin = 1 << (clockPin & PINMASK);
  for (uint8_t i = 0; i < 8; ++i) {
    GPIOxClock->ODR |= clockPin; // digitalWrite(clockPin, HIGH);
    if (bitOrder == LSBFIRST)
      result |= ((uint8_t)((GPIOxData->IDR & dataPin) != 0) << i); // (digitalRead(dataPin) << i);
    else
      result |= ((uint8_t)((GPIOxData->IDR & dataPin) != 0) << (7 - i)); // (digitalRead(dataPin) << (7 - i));
    GPIOxClock->ODR &= (uint8_t)(~clockPin); // digitalWrite(clockPin, LOW);
  }

  return result;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
  GPIO_TypeDef* GPIOxData = pinToGPIO(dataPin);
  GPIO_TypeDef* GPIOxClock = pinToGPIO(clockPin);

  dataPin = 1 << (dataPin & PINMASK);
  clockPin = 1 << (clockPin & PINMASK);
  for (uint8_t i = 0; i < 8; ++i)  {
    if (bitOrder == LSBFIRST) {
      if (val & (1 << i)) // digitalWrite(dataPin, (val & (1 << i)) != 0);
        GPIOxData->ODR |= dataPin;
      else
        GPIOxData->ODR &= (uint8_t)(~dataPin);
    } else {
      if (val & (1 << (7 - i))) // digitalWrite(dataPin, (val & (1 << (7 - i))) != 0);
        GPIOxData->ODR |= dataPin;
      else
        GPIOxData->ODR &= (uint8_t)(~dataPin);
    }
    GPIOxClock->ODR |= clockPin; // digitalWrite(clockPin, HIGH);
    GPIOxClock->ODR &= (uint8_t)(~clockPin); // digitalWrite(clockPin, LOW);
  }
}

uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout) {
  GPIO_TypeDef* GPIOx = pinToGPIO(pin);

  pin = 1 << (pin & PINMASK);
  if (state)
    state = pin;
  uint32_t start = micros(), pulsestart;

  while ((GPIOx->IDR & pin) == state) { // Wait for pulse gap or timeout
    if ((int32_t)(micros() - start) >= (int32_t)timeout)
      return 0;
  }
  while ((GPIOx->IDR & pin) != state) { // Wait for pulse starts or timeout
    if ((int32_t)(micros() - start) >= (int32_t)timeout)
      return 0;
  }
  pulsestart = micros();
  while ((GPIOx->IDR & pin) == state) { // Wait for pulse ends or timeout
    if ((int32_t)(micros() - start) >= (int32_t)timeout)
      return 0;
  }

  return (micros() - pulsestart);
}

void tone(uint8_t pin, uint16_t freq) {
  if (pin == PD4) {
    uint8_t fr;

    if (freq <= 1000)
      fr = 0x00; // BEEP_FREQUENCY_1KHZ
    else if (freq <= 2000)
      fr = 0x40; // BEEP_FREQUENCY_2KHZ
    else if (freq <= 4000)
      fr = 0x80; // BEEP_FREQUENCY_4KHZ

    BEEP->CSR &= (uint8_t)(~BEEP_CSR_BEEPSEL);
    BEEP->CSR |= (uint8_t)(fr | BEEP_CSR_BEEPEN); // Select the output frequency and Enable the BEEP peripheral
  }
}

void noTone() {
  BEEP->CSR &= (uint8_t)(~BEEP_CSR_BEEPEN); // Disable the BEEP peripheral
}
