//#define NOASM

#include "../stm8s/stm8s.h"
#ifdef NOASM
#include "Arduino.h"
#endif

volatile uint32_t __us = 0;
volatile uint32_t __ms = 0;

void __preinit() {
/*** Set FCPU to 16 MHz */
  CLK->CKDIVR &= (uint8_t)(~(CLK_CKDIVR_HSIDIV | CLK_CKDIVR_CPUDIV));

/*** Calibrate LSI for AWU and BEEP */
  uint16_t ICValue1, ICValue2;
  uint32_t LSIFreq;

  AWU->CSR |= AWU_CSR_MSR; // Enable the LSI measurement: LSI clock connected to TIM1 Input Capture 1

  TIM1->CCER1 &= (uint8_t)(~(TIM1_CCER1_CC1E | TIM1_CCER1_CC1P)); // Disable the Channel 1: Reset the CCE Bit and Select the Polarity
  TIM1->CCMR1 &= (uint8_t)(~(TIM1_CCMR_CCxS | TIM1_CCMR_ICxF | TIM1_CCMR_ICxPSC));
  TIM1->CCMR1 |= (uint8_t)(0x01 | ((uint8_t)(0 << 4)) | (uint8_t)0x0C); // Select the Input and set the filter to 0 and prescaler to TIM1_ICPSC_DIV8
  TIM1->CCER1 |= TIM1_CCER1_CC1E; // Set the CCE Bit
  TIM1->CR1 |= TIM1_CR1_CEN; // TIM1_Cmd(ENABLE)

  while (! (TIM1->SR1 & TIM1_SR1_CC1IF)); // Wait a capture on CC1
  ICValue1 = (uint16_t)(TIM1->CCR1H << 8); // Get CCR1 value
  ICValue1 |= TIM1->CCR1L;
  TIM1->SR1 = (uint8_t)(~(uint8_t)TIM1_SR1_CC1IF); // TIM1_ClearFlag(TIM1_FLAG_CC1)

  while (! (TIM1->SR1 & TIM1_SR1_CC1IF)); // Wait a capture on CC1
  ICValue2 = (uint16_t)(TIM1->CCR1H << 8); // Get CCR1 value
  ICValue2 |= TIM1->CCR1L;
  TIM1->SR1 = (uint8_t)(~(uint8_t)TIM1_SR1_CC1IF); // TIM1_ClearFlag(TIM1_FLAG_CC1)

  TIM1->CR1 &= (uint8_t)(~TIM1_CR1_CEN); // TIM1_Cmd(DISABLE)
  TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1E); // Disable IC1 input capture
  TIM1->CCMR1 &= (uint8_t)(~(TIM1_CCMR_CCxS | TIM1_CCMR_ICxF | TIM1_CCMR_ICxPSC));

  AWU->CSR &= (uint8_t)(~AWU_CSR_MSR); // Disable the LSI measurement: LSI clock disconnected from TIM1 Input Capture 1

  LSIFreq = (HSI_VALUE * 8) / (ICValue2 - ICValue1);
  ICValue1 = (uint16_t)(LSIFreq / 1000); // Converts value to kHz

  // Calculation of AWU calibration value
  ICValue2 = (uint16_t)(ICValue1 >> 2); // Division by 4, keep integer part only
  if ((ICValue2 * 4) >= ((ICValue1 - (ICValue2 * 4)) * ((ICValue2 * 2) + 1)))
    AWU->APR = (uint8_t)(ICValue2 - 2);
  else
    AWU->APR = (uint8_t)(ICValue2 - 1);

  // Calculation of BEEP calibration value
  ICValue2 = (uint16_t)(ICValue1 >> 3); // Division by 8, keep integer part only
  BEEP->CSR &= (uint8_t)(~BEEP_CSR_BEEPDIV); // Clear bits
  if ((ICValue2 * 8) >= ((ICValue1 - (ICValue2 * 8)) * ((ICValue2 * 2) + 1)))
    BEEP->CSR |= (uint8_t)(ICValue2 - 2);
  else
    BEEP->CSR |= (uint8_t)(ICValue2 - 1);
  
/*** Setup TIM1 to generate PWM output about 1 kHz */
  TIM1->PSCRH = (uint8_t)0; // Set the Prescaler value to 16
  TIM1->PSCRL = (uint8_t)15;
  TIM1->ARRH = (uint8_t)0; // Set the Autoreload value to 256
  TIM1->ARRL = (uint8_t)255; // (16 MHz / 16 = 1 MHz / 256 ~ 3906 Hz)
  TIM1->RCR = (uint8_t)0; // Set the Repetition Counter value
  TIM1->BKR |= (uint8_t)TIM1_BKR_MOE; // TIM1_CtrlPWMOutputs(ENABLE)
  TIM1->CCMR1 &= (uint8_t)(~TIM1_CCMR_OCM); // Reset the Output Compare Bits
  TIM1->CCMR1 |= (uint8_t)TIM1_CCMR_OCxPE; // Set the Output Compare Preload Enable
  TIM1->CCMR2 &= (uint8_t)(~TIM1_CCMR_OCM); // Reset the Output Compare Bits
  TIM1->CCMR2 |= (uint8_t)TIM1_CCMR_OCxPE; // Set the Output Compare Preload Enable
  TIM1->CCMR3 &= (uint8_t)(~TIM1_CCMR_OCM); // Reset the Output Compare Bits
  TIM1->CCMR3 |= (uint8_t)TIM1_CCMR_OCxPE; // Set the Output Compare Preload Enable
  TIM1->CCMR4 &= (uint8_t)(~TIM1_CCMR_OCM); // Reset the Output Compare Bits
  TIM1->CCMR4 |= (uint8_t)TIM1_CCMR_OCxPE; // Set the Output Compare Preload Enable
  TIM1->SR1 = (uint8_t)(~TIM1_SR1_UIF); // TIM1_ClearFlag(TIM1_FLAG_UPDATE)
  TIM1->IER |= TIM1_IER_UIE; // TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE)
  TIM1->CR1 &= (uint8_t)(~(TIM1_CR1_CMS | TIM1_CR1_DIR)); // Select the Counter Mode to TIM1_COUNTERMODE_UP
  TIM1->CR1 |= (uint8_t)TIM1_CR1_CEN; // TIM1_Cmd(ENABLE)

/*** Setup TIM2 to generate PWM output about 1 kHz */
  TIM2->PSCR = (uint8_t)0x04; // Set the Prescaler value to TIM2_PRESCALER_16
  TIM2->ARRH = (uint8_t)0; // Set the Autoreload value to 256
  TIM2->ARRL = (uint8_t)255; // (16 MHz / 16 = 1 MHz / 256 ~ 3906 Hz)
  TIM2->CCMR1 &= (uint8_t)(~TIM2_CCMR_OCM); // Reset the Output Compare Bits
  TIM2->CCMR1 |= (uint8_t)TIM2_CCMR_OCxPE; // Set the Output Compare Preload Enable
  TIM2->CCMR2 &= (uint8_t)(~TIM2_CCMR_OCM); // Reset the Output Compare Bits
  TIM2->CCMR2 |= (uint8_t)TIM2_CCMR_OCxPE; // Set the Output Compare Preload Enable
  TIM2->CCMR3 &= (uint8_t)(~TIM2_CCMR_OCM); // Reset the Output Compare Bits
  TIM2->CCMR3 |= (uint8_t)TIM2_CCMR_OCxPE; // Set the Output Compare Preload Enable
  TIM2->CR1 |= (uint8_t)TIM2_CR1_CEN; // TIM2_Cmd(ENABLE)

/*** Setup TIM4 to overflow each 1 ms */
  TIM4->PSCR = (uint8_t)0x07; // Set the Prescaler value to TIM4_PRESCALER_128
  TIM4->ARR = (uint8_t)124; // Set the Autoreload value to 125 (16 MHz / 128 = 125 kHz / 125 = 1 kHz = 1 ms)
  TIM4->SR1 = (uint8_t)(~TIM4_SR1_UIF); // TIM4_ClearFlag(TIM4_FLAG_UPDATE)
  TIM4->IER |= (uint8_t)TIM4_IER_UIE; // TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE)
  TIM4->CR1 |= TIM4_CR1_CEN; // TIM4_Cmd(ENABLE)

/*** Setup ADC */
  ADC1->CR2 &= (uint8_t)(~(ADC1_CR2_ALIGN | ADC1_CR2_EXTSEL | ADC1_CR2_EXTTRIG)); // Clear the Align bit and the External Trigger Selection bits and Disable the Selected External Trigger
  ADC1->CR2 |= (uint8_t)0x08; // Configure the data alignment to ADC1_ALIGN_RIGHT
  ADC1->CR1 &= (uint8_t)(~(ADC1_CR1_CONT | ADC1_CR1_SPSEL)); // Set the single conversion mode to ADC1_CONVERSIONMODE_SINGLE and Clear the SPSEL bits
  ADC1->CR1 |= (uint8_t)(0x20 | ADC1_CR1_ADON); // Select the prescaler division factor to ADC1_PRESSEL_FCPU_D4 and Wakeup ADC

  enableInterrupts();
}

void delay_us(volatile uint16_t us) {
  asm("TNZW X; 2\n"
    "JREQ exitproc; 1/2\n"
    "DECW X; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "JREQ exitproc; 1/2\n"
    "NOP; 1\n"
    "cmp:\n"
    "DECW X; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "JRNE cmp; 1/2\n"
    "NOP; 1\n"
    "exitproc:\n; +RET 4");
}

void delay_us(volatile uint32_t us) {
  asm("LDW X, S:?w1; 2\n"
    "LDW Y, S:?w0; 2\n"
    "TNZW Y; 2\n"
    "JRNE skip; 1/2\n"
    "CPW X, #2; 2\n"
    "JRULT exitproc; 1/2\n"
    "JP skip2; 1\n"
    "skip:\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "skip2:\n"
    "SUBW X, #2; 2\n"
    "JRNC skip3; 1/2\n"
    "DECW Y; 1\n"
    "skip3:\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "cmp:\n"
    "TNZW Y; 2\n"
    "JRNE skip4; 1/2\n"
    "TNZW X; 2\n"
    "JREQ exitproc; 1/2\n"
    "JP skip5\n"
    "skip4:\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "skip5:\n"
    "TNZW X; 2\n"
    "JRNE skip6; 1/2\n"
    "DECW Y; 1\n"
    "skip6:\n"
    "DECW X; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "NOP; 1\n"
    "JP cmp; 1\n"
    "exitproc:; +RET 4\n");
}

void delayMicroseconds(volatile uint32_t us) {
#ifndef NOASM
  asm("LDW X, S:?w1\n"
    "LDW Y, S:?w0\n"
    "TNZW Y\n"
    "JRNE skip\n"
    "CPW X, #2\n"
    "JRULE exitproc\n"
    "CPW X, #$100\n"
    "JRUGE skip\n"
    "LD A, XL\n"
    "ADD A, $525F; TIM1_CNTRL\n"
    "cmp:\n"
    "CP A, $525F; TIM1_CNTRL\n"
    "JRSGT cmp\n"
    "JP exitproc\n"
    "skip:\n"
    "LD A, XL\n"
    "ADD A, $525F; TIM1_CNTRL\n"
    "JRNC skip3\n"
    "PUSH A\n"
    "LD A, XH\n"
    "INC A\n"
    "JRNE skip2\n"
    "INCW Y\n"
    "skip2:\n"
    "LD XH, A\n"
    "POP A\n"
    "skip3:\n"
    "LD XL, A\n"
    "ADDW X, __us + 2\n"
    "JRNC skip4\n"
    "INCW Y\n"
    "skip4:\n"
    "ADDW Y, __us\n"
    "loop:\n"
    "CPW Y, S:__us\n"
    "JRSLT exitproc\n"
    "JRSGT loop\n"
    "LD A, XH\n"
    "CP A, S:__us + 2\n"
    "JRULT exitproc\n"
    "JRUGT loop\n"
    "LD A, XL\n"
    "CP A, $525F; TIM1_CNTRL\n"
    "JRUGT loop\n"
    "exitproc:\n");
#else
  if (us > 2) {
    if (us < 256) {
      volatile uint8_t cntr = TIM1->CNTRL;
      cntr += (uint8_t)us;
      while ((int8_t)TIM1->CNTRL < (int8_t)cntr);
    } else {
      us += micros();
      while ((int32_t)micros() < (int32_t)us); // Wait us microseconds
    }
  }
#endif
}

void delay(volatile uint32_t ms) {
#ifndef NOASM
  asm("LDW X, S:?w1\n"
    "LDW Y, S:?w0\n"
    "TNZW Y\n"
    "JRNE skip\n"
    "TNZW X\n"
    "JREQ exitproc\n"
    "skip:\n"
    "LD A, $5346; TIM4_CNTR\n"
    "ADDW X, __ms + 2\n"
    "JRNC skip2\n"
    "INCW Y\n"
    "skip2:\n"
    "ADDW Y, __ms\n"
    "loop:\n"
    "CPW Y, S:__ms\n"
    "JRSLT exitproc\n"
    "JRSGT loop\n"
    "CPW X, S:__ms + 2\n"
    "JRUGT loop\n"
    "CP A, #2\n"
    "JRULT exitproc\n"
    "loop2:\n"
    "CP A, $5346; TIM4_CNTR\n"
    "JRUGT loop2\n"
    "exitproc:\n");
#else
  if (ms) {
    volatile uint8_t cntr = (uint8_t)TIM4->CNTR;
    ms += __ms;
    while ((int32_t)__ms < (int32_t)ms); // Wait ms whole milliseconds
    if (cntr > 1)
      while ((uint8_t)TIM4->CNTR < cntr); // Wait fraction millisecond
  }
#endif
}

INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11) {
#ifndef NOASM
  asm("INC S:__us + 2\n"
    "JRNE skip\n"
    "INC S:__us + 1\n"
    "JRNE skip\n"
    "INC S:__us\n"
    "skip:\n");
#else
  __us += 256;
#endif
  TIM1->SR1 = (uint8_t)(~TIM1_SR1_UIF); // TIM1_ClearITPendingBit(TIM1_IT_UPDATE)
}

INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) {
#ifndef NOASM
  asm("INC S:__ms + 3\n"
    "JRNE skip\n"
    "INC S:__ms + 2\n"
    "JRNE skip\n"
    "INC S:__ms + 1\n"
    "JRNE skip\n"
    "INC S:__ms\n"
    "skip:\n");
#else
  ++__ms;
#endif
  TIM4->SR1 = (uint8_t)(~TIM4_SR1_UIF); // TIM4_ClearITPendingBit(TIM4_IT_UPDATE)
}
