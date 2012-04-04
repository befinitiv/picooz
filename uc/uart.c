#include <avr/io.h>

#define F_CPU 8000000UL

#include <util/delay.h>



void uart_init(void)
	{
	DDRB |= 1 << PB0;
	PORTB |= 1 << PB0;
	_delay_loop_2(0xa000);
	}


void uart_send(unsigned char c)
	{
	unsigned char i;

	//start-bit
	PORTB &= ~(1 << PB0);
	_delay_loop_2(190);

	for(i=0; i<8; ++i)
		{
		if((c & (1 << i)))
			PORTB |= 1 << PB0;
		else
			PORTB &= ~(1 << PB0);
		_delay_loop_2(190);
		}

	//stop
	PORTB |= 1 << PB0;
	_delay_loop_2(190);
	}
