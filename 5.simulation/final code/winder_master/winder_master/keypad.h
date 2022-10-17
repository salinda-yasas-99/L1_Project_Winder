#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>


#define MOSI 5								/* Define SPI bus pins */
#define MISO 6
#define SCK 7
#define SS 4
#define SS_Enable PORTB &= ~(1<<SS)			/* Define Slave enable */
#define SS_Disable PORTB |= (1<<SS)			/* Define Slave disable */

#define KEY_PRT PORTC
#define KEY_DDR	DDRC
#define KEY_PIN	PINC

void SPI_Init()								/* SPI Initialize function */
{
	DDRB |= (1<<MOSI)|(1<<SCK)|(1<<SS);		/* Make MOSI, SCK, 0th pin direction as output pins */
	DDRB &= ~(1<<MISO);						/* Make MISO pin as input pin */
	PORTB |= (1<<SS);						/* Disable slave initially by making high on SS pin */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);	/* Enable SPI, Enable in master mode, with Fosc/16 SCK frequency */
	SPSR &= ~(1<<SPI2X);					/* Disable speed doubler */
}

void SPI_Write(char data)					/* SPI write data function */
{
	
	SPDR = data;							/* Write data to SPI data register */
	while(!(SPSR & (1<<SPIF)));				/* Wait till transmission complete */
}


unsigned char keypad[4][4] = {{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}};



unsigned char colloc, rowloc;

char keyfind()
{
	while(1)
	{
		KEY_DDR = 0xF0;           /* set port direction as input-output */
		KEY_PRT = 0xFF;

		do
		{
			KEY_PRT &= 0x0F;      /* mask PORT for column read only */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F); /* read status of column */
		}while(colloc != 0x0F);
		
		do
		{
			do
			{
				_delay_ms(20);             /* 20ms key debounce time */
				colloc = (KEY_PIN & 0x0F); /* read status of column */
			}
			while(colloc == 0x0F);        /* check for any key press */
			
			_delay_ms (40);	            /* 20 ms key debounce time */
			colloc = (KEY_PIN & 0x0F);
			
		}
		while(colloc == 0x0F);

		/* now check for rows */
		KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 0;
			break;
		}

		KEY_PRT = 0xDF;		/* check for pressed key in 2nd row */
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 1;
			break;
		}
		
		KEY_PRT = 0xBF;		/* check for pressed key in 3rd row */
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 2;
			break;
		}

		KEY_PRT = 0x7F;		/* check for pressed key in 4th row */
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 3;
			break;
		}
	}

	if(colloc == 0x0E)
	return(keypad[rowloc][0]);
	else if(colloc == 0x0D)
	return(keypad[rowloc][1]);
	else if(colloc == 0x0B)
	return(keypad[rowloc][2]);
	else
	return(keypad[rowloc][3]);
}

void key_enter()
{
	char key;
	while(1)
	{
		key=keyfind();
		if(key=='#')
		{
			SPI_Init();
			SS_Enable;
			SPI_Write(key);
			break;
		}
	}
}

int key_in()
{
	char temp[5]="0";
	int n=0;
	char key;
	while(1)
	{
		
		key=keyfind();
		if(key=='#')
		{
			SPI_Init();
			SS_Enable;
			SPI_Write(key);//writing pressed key to slave
			break;
		}
		else
		{
			
			SPI_Init();
			SS_Enable;
			SPI_Write(key);
			temp[n]=key;
			n++;
		}
		
		
	}
	int num = atoi(temp);
	return num;
}

void pass_int(int x)		 //Send string to LCD function
{
	int num;
	num=x;
	
	SPI_Init();
	SS_Enable;
	SPI_Write(num);
	
	
}