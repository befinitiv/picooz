#include <avr/interrupt.h>
#include <avr/io.h>


#include "uart.h"
#include "motor.h"

unsigned char halfbitpos = 0;
unsigned short fail = 0;

unsigned char powerv = 0;
char trimmingv = 0;
char steeringv = 0;

void ir_init(void)
	{
	//every logical change generates an interrupt
	MCUCR |= (1 << ISC00);
	GIMSK |= (1 << INT0);

	//timer 1 1/64 prescaler
	TCCR1 |= (1 << CS10) | (1 << CS11) | (1 << CS12);

	//activate overflow interrupt
	TIMSK |= (1 << TOIE1);


	sei();
	}


ISR(TIM1_OVF_vect)
	{
	halfbitpos = 0;


	fail++;

	if(fail == 300)
		{
		powerv = 0;
		steeringv = 0;
		motor_set_main(0);
		fail = 0;
		
		}
	}


#define T1 87//87
#define TSTART 200//212

ISR(INT0_vect)
	{
	static unsigned char power = 0;
	static char trimming = 0;
	static char steering = 0;
	static unsigned char chksum = 0;

	unsigned char pinlevel;
	unsigned char timerval = TCNT1;
	TCNT1 = 0;

	pinlevel = PINB & (1 << PB2);


	if(halfbitpos == 0)
		{
		chksum = 0;

		if(pinlevel != 0)
			return;
		}

	//if the startbit is too short, we start again
	if(halfbitpos == 1 && timerval < TSTART)
		{
		goto cancel;
		}

	if(halfbitpos == 11)
		{
		if(timerval >= T1)
			chksum += 2;
		}


	if(halfbitpos == 13)
		{
		//if the c1 bit is zero, we are not on channel B => we cancel
		if(timerval < T1)
			{
			goto cancel;
			}
		else
			chksum++;
		}

	//power-bits
	if(halfbitpos == 15)
		{
		if(timerval < T1)
			power &= ~(1 << 3);
		else
			{
			power |= (1 << 3);
			chksum += 2;
			}
		}

	if(halfbitpos == 17)
		{
		if(timerval < T1)
			power &= ~(1 << 2);
		else
			{
			power |= (1 << 2);
			chksum++;
			}
		}

	if(halfbitpos == 19)
		{
		if(timerval < T1)
			power &= ~(1 << 1);
		else
			{
			power |= (1 << 1);
			chksum += 2;
			}
		}

	if(halfbitpos == 21)
		{
		if(timerval < T1)
			power &= ~(1 << 0);
		else
			{
			power |= (1 << 0);
			chksum++;
			}
		}


	//trimmig-bits
	if(halfbitpos == 23)
		{
		if(timerval < T1)
			trimming &= ~(1 << 3);
		else
			{
			trimming |= (1 << 3);
			chksum += 2;
			}
		}

	if(halfbitpos == 25)
		{
		if(timerval < T1)
			trimming &= ~(1 << 2);
		else
			{
			trimming |= (1 << 2);
			chksum++;
			}
		}

	if(halfbitpos == 27)
		{
		if(timerval < T1)
			trimming &= ~(1 << 1);
		else
			{
			trimming |= (1 << 1);
			chksum += 2;
			}
		}

	if(halfbitpos == 29)
		{
		if(timerval < T1)
			trimming &= ~(1 << 0);
		else
			{
			trimming |= (1 << 0);
			chksum++;
			}
		}


	//steeringsbits
	if(halfbitpos == 31)
		{
		if(timerval < T1)
			steering &= ~(1 << 2);
		else
			{
			steering |= (1 << 2);
			chksum += 2;
			}
		}

	if(halfbitpos == 33)
		{
		if(timerval < T1)
			steering &= ~(1 << 1);
		else
			{
			steering |= (1 << 1);
			chksum++;
			}
		}

	if(halfbitpos == 35)
		{
		if(timerval < T1)
			steering &= ~(1 << 0);
		else
			{
			steering |= (1 << 0);
			chksum += 2;
			}
		}

	if(halfbitpos == 37)
		{
		//create the two-complement
		chksum = ~chksum + 1;

		//if the checksum is wrong => cancel!
		if((timerval < T1 && (chksum & 1) == 1) || (timerval >= T1 && (chksum & 1) == 0))
			{
			goto cancel;
			}
		}

	if(halfbitpos == 39)
		{
		//if the checksum is wrong => cancel!
		if((timerval < T1 && (chksum & 2) == 2) || (timerval >= T1 && (chksum & 2) == 0))
			{
			goto cancel;
			}

		powerv = power;
		steeringv = steering;

		if(steeringv > 3)
			steeringv = steeringv - 8;


		if(trimming == 1)
			trimmingv++;
		if(trimming == 15)
			trimmingv--;


		fail = 0;

		if(powerv == 0)
			motor_set_main(0);
		else
			motor_set_main(47 + powerv * 16);


		}

	if(halfbitpos == 41)
		{
		goto cancel;
		}


	halfbitpos++;
	return;

cancel:
	halfbitpos = 0;
	}
