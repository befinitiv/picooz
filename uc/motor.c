#include <avr/io.h>






void motor_init(void)
	{
	TCCR0A |= (1 << WGM00) | (1 << WGM01);
	TCCR0B |= (1 << CS02);
	OCR0A = 0;
	OCR0B = 0;
	DDRB |= (1 << PB1) | (1 << PB0);
	}



void motor_set_rear(unsigned char val)
	{
	//stop the timer with a value of 0 to avoid spikes
	if(val == 0)
		TCCR0A &= ~(1 << COM0B1);
	else
		TCCR0A |= (1 << COM0B1);

	OCR0B = val;
	}


void motor_set_main(unsigned char val)
	{
	//stop the timer with a value of 0 to avoid spikes
	if(val == 0)
		TCCR0A &= ~(1 << COM0A1);
	else
		TCCR0A |= (1 << COM0A1);

	OCR0A = val;
	}
