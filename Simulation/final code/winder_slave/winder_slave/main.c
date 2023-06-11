#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "slave_SPI.h"
#include "LCD_8bit.h"


int
ACT,				//total no of ampearages
VCT,				//Relevant ampearage no of voltages
P_SWGA,				//Primary SWGA array location
S_SWGA,				//Secondary SWGA
C_AR,				//Core area
EI_LA,				//EI lamination array location
Bob_result,			//Bobbin measuring unit result
win_result			//next coil insertion result
;

//data arrays
int RAM_SWGA[34]={47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,
32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14};		//Standard wire gauge

char RAM_EILAM[30][4]={"17","12A","74","23","30","21","31","10","15","33","1", "14","11","34","3","9","9A","11A","4A","2","16",			//EI lamination model number
"5", "4AX","13","75","4","7","6","35A","8"};



void key_enter_receive()
{

	char key1='0';
	
	while(1)
	{
		
		SPI_Init();
		key1=SPI_Receive();
		if(key1=='#')
		{
			
			LCD_Clear();
			_delay_ms(500);
			break;
		}
		

	}
	

}


void key_receive()
{
	char value;
	while(1)
	{
		
		SPI_Init();
		value = SPI_Receive();//Receiveing the pressed key from master
		if(value=='#')
		{
			
			LCD_Clear();
			_delay_ms(500);
			break;
		}
		else
		{
			LCD_Char(value);//printing pressed key in LCD
		}
	}
	
}


int main(void)
{
	
	
	LCD_Init();
	

	LCD_String("    Welcome");
	LCD_Command(0xC0); //setting cursor to second line
	_delay_ms(300);
	LCD_String("   The Winder");
	key_enter_receive();
	//LCD_Clear();

	LCD_String("Enter primary vo");
	LCD_Command(0xC0);
	LCD_String("ltage :");
	LCD_Command(0x0F);
	key_receive();
	
	LCD_String("Enter primary fr");
	LCD_Command(0xC0);
	LCD_String("equency :");
	_delay_ms(300);
	LCD_Command(0x0F);
	key_receive();
	
	LCD_String("Enter NO of ");
	LCD_Command(0xC0);
	LCD_String("ampearages:");
	_delay_ms(300);
	LCD_Command(0x0F);
	key_receive();
	
	ACT=get_value();
	int num_vol_voltages[ACT]; //Number of voltages per ampearage in secondary coil
	
	LCD_String("Enter NO of ");
	LCD_Command(0xC0);
	LCD_String("total volts:");
	_delay_ms(300);
	LCD_Command(0x0F);
	key_receive();
	
	for(int i=0;i<ACT;i++)
	{
		LCD_String("Enter ampearage");
		LCD_Command(0xC0);
		LCD_String("(mA):");
		_delay_ms(300);
		LCD_Command(0x0F);
		key_receive();
		

		LCD_String("Enter no of");
		LCD_Command(0xC0);
		LCD_String("volts:");
		_delay_ms(300);
		LCD_Command(0x0F);
		key_receive();
		
		VCT=get_value();
		
		
		num_vol_voltages[i]=VCT;
		
		for(int j=0;j<VCT;j++)
		{
			
			LCD_String("Enter voltage");
			LCD_Command(0xC0);
			LCD_String(":");
			_delay_ms(300);
			LCD_Command(0x0F);
			key_receive();
			
		}
		
	}
	
	//printing values for user
	
	//printing primary SWGA
	LCD_String("PRI SWGA:");
	P_SWGA=get_value();
	char out_str[7]="0";
	itoa(RAM_SWGA[P_SWGA],out_str,10);
	LCD_Command(0xC0);
	LCD_String(out_str);
	key_enter_receive();
	
	
	//int t=0;
	//printing secondary SWGA
	for(int i=0;i<ACT;i++)
	{
		LCD_Command(0X80);
		LCD_String("SEC SWGA:");
		LCD_Command(0xC0);
		S_SWGA=get_value();
		_delay_ms(1000);
		char out_str3[7]="0";
		itoa(S_SWGA,out_str3,10);
		LCD_String(out_str3);
		key_enter_receive();
		_delay_ms(500);
		
		
	}
	
	
	
	//printing core area
	LCD_Command(0X80);
	LCD_String("core area:");
	C_AR=get_value();
	out_str[7]="0";
	itoa(C_AR,out_str,10);
	LCD_Command(0xC0);
	LCD_String(out_str);
	key_enter_receive();
	
	//printing ei lamination
	LCD_Command(0X80);
	LCD_String("EI LAMINATION:");
	EI_LA=get_value();
	LCD_Command(0xC0);
	LCD_String(RAM_EILAM[EI_LA]);
	key_enter_receive();
	
	
	//Bobbing measuring unit
	LCD_Command(0X80);
	LCD_String("Place the Bobbin");
	key_enter_receive();
	
	
	
	LCD_Command(0X0C);
	LCD_Command(0X80);
	LCD_String("Rotate Bobbin");
	key_enter_receive();
	
	
	//z length measuring
	LCD_Command(0X80);
	LCD_String("adjust bobbin");
	key_enter_receive();
	
	
	Bob_result=get_value();
	if(Bob_result==1)
	{
		LCD_Command(0X80);
		LCD_String("Bobbin is suitable");
		LCD_Command(0XC0);
		LCD_String("le");
		key_enter_receive();
	}
	else
	{
		LCD_Command(0X80);
		LCD_String("Bobbin isnot sui");
		LCD_Command(0XC0);
		LCD_String("table");
		key_enter_receive();
	}
	
	LCD_Command(0X80);
	LCD_String("Bobbin measuring");
	LCD_Command(0XC0);
	LCD_String("completed");
	key_enter_receive();
	

	//starting the winding
	LCD_Command(0X80);
	LCD_String("Press # to start");
	LCD_Command(0XC0);
	LCD_String("winding!");
	key_enter_receive();
	
	
	for(int i=0;i<ACT;i++)
	{
	int len=num_vol_voltages[i];
	if(len>1)
	{
	
		win_result=get_value();
		if(win_result==10)
		{
			LCD_Command(0X80);
			LCD_String("Insert next coil");
			key_enter_receive();
		}
		else if(win_result==30)
		{
			LCD_Command(0X80);
			LCD_String("Winding");
			LCD_Command(0XC0);
			LCD_String("completed!");
			key_enter_receive();
		}
	}
	else
	{
		win_result=get_value();
		LCD_Command(0X80);
		LCD_String("Winding");
		LCD_Command(0XC0);
		LCD_String("completed!");
		key_enter_receive();
	}
	
	}
	
	

	
}