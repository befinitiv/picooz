#include <avr/io.h>

#define F_CPU 8000000UL

#include <util/delay.h>


#include "uart.h"
#include "ir.h"
#include "gyro.h"
#include "motor.h"





int main(void)
	{
//	uart_init();
	ir_init();
	gyro_init();
	motor_init();


	for(;;)
		{
		//start conversion
		ADCSRA |= (1 <<  ADSC);
		_delay_loop_2(10000);
		}

	return 0;
	}
