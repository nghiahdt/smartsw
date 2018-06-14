#include "stm8s.h"

#include "iic.h"
#include "io.h"
#include "modbus.h"

main()
{
	// Sets the SystemClock
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8); // fMASTER := 2 MHz
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV2); // fCPU = 1 MHz
	CLK_SWIMConfig(CLK_SWIMDIVIDER_2);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
	
	// Sets the I/O ports
	io_init();

	// Sets the Modbus
	modbus_init();

	// Sets the I2C
	iic_init();

	// Unused pins
	GPIO_Init(GPIOA, GPIO_PIN_2|GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(GPIOD, GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT);

	// Enable interrupts
	rim();

	while (1)
	{
	}
}