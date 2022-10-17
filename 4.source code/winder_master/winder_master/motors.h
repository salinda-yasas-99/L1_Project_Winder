#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#define m1 0
#define m2 1
#define m3 2
#define m4 3
#define m5 0
#define m6 1
#define m7 2
#define m8 3
#define s1 4
#define s2 5
#define s3 6
#define s4 6
#define s5 7
#define s6 4
#define SM1_PORT PORTA
#define SM2_PORT PORTD
#define SERS_PORT PORTD
#define M1_PORT PORTA
#define M2_PORT PORTB

void stepper_1(int n)
{
	int period;
	period = 1000;
	DDRA = 0xFF;
	while (1)
	{
		M1_PORT = ((((M1_PORT&~(1<<m4))&~(1<<m3))&~(1<<m2))&~(1<<m1));
		for(int i=0;i<n;i++)
		{
			M1_PORT = ((((M1_PORT|(1<<m4))&~(1<<m3))&~(1<<m2))|(1<<m1));
			_delay_ms(period);
			M1_PORT = ((((M1_PORT|(1<<m4))|(1<<m3))&~(1<<m2))&~(1<<m1));
			_delay_ms(period);
			M1_PORT = ((((M1_PORT&~(1<<m4))|(1<<m3))|(1<<m2))&~(1<<m1));
			_delay_ms(period);
			M1_PORT = ((((M1_PORT&~(1<<m4))&~(1<<m3))|(1<<m2))|(1<<m1));
			_delay_ms(period);
		}
		break;
	}
}

void stepper_1_first_half()
{
	int period;
	period = 1000;
	DDRA = 0xFF;
	M1_PORT = ((((M1_PORT&~(1<<m4))&~(1<<m3))&~(1<<m2))&~(1<<m1));
	_delay_ms(100);
	M1_PORT = ((((M1_PORT|(1<<m4))&~(1<<m3))&~(1<<m2))|(1<<m1));
	_delay_ms(period);
	M1_PORT = ((((M1_PORT|(1<<m4))|(1<<m3))&~(1<<m2))&~(1<<m1));
	_delay_ms(period);
}

void stepper_1_first_half_reverse()
{
	int period;
	period = 1000;
	DDRA = 0xFF;
	M1_PORT = ((((M1_PORT&~(1<<m4))&~(1<<m3))&~(1<<m2))&~(1<<m1));
	M1_PORT = ((((M1_PORT|(1<<m4))&~(1<<m3))&~(1<<m2))|(1<<m1));
	_delay_ms(period);
	M1_PORT = ((((M1_PORT&~(1<<m4))&~(1<<m3))|(1<<m2))|(1<<m1));
	_delay_ms(period);
}

void stepper_1_second_half()
{
	int period;
	period = 1000;
	DDRA = 0xFF;
	M1_PORT = ((((M1_PORT&~(1<<m4))|(1<<m3))|(1<<m2))&~(1<<m1));
	_delay_ms(period);
	M1_PORT = ((((M1_PORT&~(1<<m4))&~(1<<m3))|(1<<m2))|(1<<m1));
	_delay_ms(period);
}

void stepper_1_second_half_reverse()
{
	int period;
	period = 1000;
	DDRA = 0xFF;
	M1_PORT = ((((M1_PORT|(1<<m4))|(1<<m3))&~(1<<m2))&~(1<<m1));
	_delay_ms(period);
	M1_PORT = ((((M1_PORT&~(1<<m4))|(1<<m3))|(1<<m2))&~(1<<m1));
	_delay_ms(period);
}


void stepper_1_reverse(int n)
{
	int period;
	period = 1000;
	DDRA = 0xFF;
	while (1)
	{
		M1_PORT = ((((M1_PORT&~(1<<m4))&~(1<<m3))&~(1<<m2))&~(1<<m1));
		for(int i=0;i<n;i++)
		{
			M1_PORT = ((((M1_PORT&~(1<<m4))|(1<<m3))|(1<<m2))&~(1<<m1));
			_delay_ms(period);
			M1_PORT = ((((M1_PORT|(1<<m4))|(1<<m3))&~(1<<m2))&~(1<<m1));
			_delay_ms(period);
			M1_PORT = ((((M1_PORT|(1<<m4))&~(1<<m3))&~(1<<m2))|(1<<m1));
			_delay_ms(period);
			M1_PORT = ((((M1_PORT&~(1<<m4))&~(1<<m3))|(1<<m2))|(1<<m1));
			_delay_ms(period);
		}
		break;
	}
}

void servo_half()
{
	DDRD |= (1<<PD5);	/* Make OC1A pin as output */
	TCNT1 = 0;		/* Set timer1 count zero */
	ICR1 = 2499;		/* Set TOP count for timer1 in ICR1 register */

	/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	while(1)
	{
		OCR1A = 300;	/* Set servo shaft at 90° position */
		_delay_ms(1500);
		OCR1A = 187;	/* Set servo at +0° position */
		_delay_ms(1500);
		break;
	}
}

void servo_half_stop()
{
	DDRD |= (1<<PD5);	/* Make OC1A pin as output */
	TCNT1 = 0;		/* Set timer1 count zero */
	ICR1 = 2499;		/* Set TOP count for timer1 in ICR1 register */

	/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	while(1)
	{
		OCR1A = 300;	/* Set servo shaft at 90° position */
		_delay_ms(1500);
		break;
	}
}

void servo_half_stop_reverse()
{
	DDRD |= (1<<PD5);	/* Make OC1A pin as output */
	TCNT1 = 0;		/* Set timer1 count zero */
	ICR1 = 2499;		/* Set TOP count for timer1 in ICR1 register */

	/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	while(1)
	{
		OCR1A = 187;	/* Set servo at +0° position */
		_delay_ms(1500);
		break;
	}
}



void stepper_2()
{
	int period;
	period = 1000;
	DDRB = 0xFF;
	while (1)
	{
		M2_PORT = ((((M2_PORT&~(1<<m8))&~(1<<m7))&~(1<<m6))&~(1<<m5));
		M2_PORT = ((((M2_PORT|(1<<m8))&~(1<<m7))&~(1<<m6))|(1<<m5));
		_delay_ms(period);
		M2_PORT = ((((M2_PORT&~(1<<m8))&~(1<<m7))|(1<<m6))|(1<<m5));
		_delay_ms(period);
		break;
	}
}
