#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>



void buzzer()
{
	DDRA = DDRA | (1<<7);
	for(int i=0;i<2;i++)
	{
		PORTA = PORTA | (1<<7);
		
		_delay_ms(1000);
		PORTA = PORTA & (~(1<<7));
		
		_delay_ms(1000);
	}
}