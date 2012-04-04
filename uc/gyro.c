#include <avr/interrupt.h>
#include <avr/io.h>


#include "uart.h"
#include "ir.h"
#include "motor.h"


void gyro_init(void)
	{
	//initialize adc

	//1v internal reference, 20x gain
	ADMUX |= /*(1 << REFS2) | */(1 << REFS1) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2);

	//adc on, int enabled, 1/16 clk
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2);

	//bipolar modus
	ADCSRB |= (1 << BIN);

	//disable digital pins
	DIDR0 |= (1 << ADC2D) | (1 << ADC3D);
	}


ISR(ADC_vect)
	{
	short ad = ADCL | (ADCH << 8);
	static short integral = 0;
	static short oldval = 0;
	short differential;
	short acutating;


	if(ad > 511)
		ad = ad - 1024;





	ad += ((short)steeringv) * 128;

	//trimming
	ad += ((short)trimmingv) * 16;

	integral += ad;
	differential = ad - oldval;

	oldval = ad;


	if(integral > 4095)
		integral = 4095;
	else
	if(integral < -4096)
		integral = -4096;



	acutating = ad  +  integral / 20 + differential / 2;

	if(acutating > 255)
		acutating = 255;
	else
	if(acutating < 0)
		acutating = 0;
	


	if(powerv != 0)
		motor_set_rear(acutating);
	else
		motor_set_rear(0);
	}
