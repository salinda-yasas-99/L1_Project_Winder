#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
//#include "motors.h"


#define M_PORT PORTB
#define m5 0
#define m6 1
#define m7 2
#define m8 3
#define s6 4
#define SM2_PORT PORTD

static volatile int pulse1 = 0; // integer  to access all though the program
int count_x = 0;
int count_y = 0;
int count_z = 0;

int ultrasonic_1()
{
	DDRD = (DDRD | (1<<0))| (1<<1);	// initializing D0 and D1 for select pins
	DDRD = DDRD | (1<<3);			// initializing D3 for trigger pin
	DDRD = DDRD & (~(1<<2));		// initializing D2 for echo pin
	PORTD &= (~(1<<PIND0)); //0
	PORTD &= (~(1<<PIND1));  //0

	_delay_ms(50);

	GICR |= (1<<INT0);    //enabling interrupt 0
	MCUCR|= (1<<ISC00);   //setting interrupt triggering logic change
	
	
	sei();
	
	for(int j=1;j<10;j++)
	{
		
		PORTD |= (1<<PIND3);
		
		_delay_us(15); //triggering the sensor for 15usec
		
		PORTD &= (~(1<<PIND3));
		
		count_x =(pulse1/58)+1;
	}
	return count_x;
}

int ultrasonic_2()
{
	DDRD = (DDRD | (1<<0))| (1<<1);	// initializing D0 and D1 for select pins
	DDRD = DDRD | (1<<3);			// initializing D3 for trigger pin
	DDRD = DDRD & (~(1<<2));		// initializing D2 for echo pin
	PORTD |= (1<<PIND0);  //1
	PORTD &= (~(1<<PIND1)); //0
	
	_delay_ms(50);

	GICR |= (1<<INT0);    //enabling interrupt 0
	MCUCR|= (1<<ISC00);   //setting interrupt triggering logic change
	
	

	sei();
	
	for(int j=1;j<10;j++)
	{
		
		PORTD |= (1<<PIND3);
		
		_delay_us(15); //triggering the sensor for 15usec
		
		PORTD &= (~(1<<PIND3));
		
		count_y =(pulse1/58)+1;
	}
	return count_y;
}

int ultrasonic_3()
{
	DDRD = (DDRD | (1<<0))| (1<<1);	// initializing D0 and D1 for select pins
	DDRD = DDRD | (1<<3);			// initializing D3 for trigger pin
	DDRD = DDRD & (~(1<<2));		// initializing D2 for echo pin
	PORTD &= (~(1<<PIND0)); //0
	PORTD |= (1<<PIND1);   //1
	
	_delay_ms(50);

	GICR |= (1<<INT0);    //enabling interrupt 0
	MCUCR|= (1<<ISC00);   //setting interrupt triggering logic change
	
	

	sei();
	
	while(1)
	{
		//All pins of PORTB as output
		PORTD |= (1<<PIND3);
		
		_delay_us(15); //triggering the sensor for 15usec
		
		PORTD &= (~(1<<PIND3));
		
		count_z =(pulse1/58)+1;
		
		int period =100;   //period for stepper motor
		DDRB = 0xFF;
		SM2_PORT = SM2_PORT&~(1<<s6);
		M_PORT = ((((M_PORT&~(1<<m8))&~(1<<m7))&~(1<<m6))&~(1<<m5));
		
		M_PORT = ((((M_PORT|(1<<m8))&~(1<<m7))&~(1<<m6))|(1<<m5));
		_delay_ms(period);
		M_PORT = ((((M_PORT|(1<<m8))|(1<<m7))&~(1<<m6))&~(1<<m5));
		_delay_ms(period);
		M_PORT = ((((M_PORT&~(1<<m8))|(1<<m7))|(1<<m6))&~(1<<m5));
		_delay_ms(period);
		M_PORT = ((((M_PORT&~(1<<m8))&~(1<<m7))|(1<<m6))|(1<<m5));
		_delay_ms(period);
		
		if(count_z < 35)
		{
			break;
		}
		else
		{
			continue;
		}
	}
	return count_z;
}

ISR(INT0_vect) //interrupt service routine when there is a change in logic level

{
	static volatile int i = 0;

	if (i==1)//when logic from HIGH to LOW

	{

		TCCR1B=0; //disabling counter

		pulse1=TCNT1;//count memory is updated to integer

		TCNT1=0;//resetting the counter memory

		i=0;

	}

	if (i==0)//when logic change from LOW to HIGH

	{

		TCCR1B|=(1<<CS10);//enabling counter

		i=1;

	}

}

