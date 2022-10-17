#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "keypad.h"
#include "ultrasonic.h"
#include "buzzer.h"
#include "winding_process.h"


float V_PRI,    //V_PRI=Primary voltage
F_PRI,          //F_PRI=Primary voltage frequency
POW_SEC,        //secondary power in MA
POW_SEC_TOT,     //total secondary power
POW_PRI,             //Primary power
C_AREA,              //core area
A_PRI,               //Priamry ampearage
Turns_V,             //Turns per volt
PRI_SWGA,           //Primary Standard wire gauge
PRI_TSQR,            //Primary wire turns per square centimetres
TOT_PRI_AREA=0,        //Total area taken by primary coil
TOT_SEC_AREA=0,          //Total area taken by secondary coil
TOT_AREA=0,              //Total area taken by both primary and secondary coil
D_PRI                   //diameter of the primary coil
;


int ACOUNT=1,         //number of amps in sec
VCOUNT=0,            //number of volts in sec
SEC_VTOT=1,           //secondary total number of voltages
NOT_PRI,            //number of turns in primary
P_SWGA,				//primary SWGA location
EI_LAMIN       //EI_LAM = EI laminations model number
;

//bobbin measuring unit variables
     
int x1_distance,
x2_distance,
y1_distance,
y2_distance,
z1_distance,
cal_core,			//calculated core area using ultra-sonic sensors
x_measurement,				//core area width
y_measurement,				//core area length
z_measurement,				//Bobbin length
x_fix=30,
y_fix=30,
z_fix=30
;



int
Z_length;               //ultra-sonic length 3




// declare a ram array and initialize
float RAM_SWGA[34]={47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,
32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14};		//Standard wire gauge

float RAM_AMP[34]={8,10,13,16,19,24,30,36,39,50,72,90,110,
	132,151,182,221,240,290,344,422,508,628,760,940,1210,1600,2030,2570,3600, 4900,6430,8410,		//maximum ampearage in coil
10040};

float RAM_TURNS[34]={27546,20223,14392,11457,9337,7755,6543,5595,4838,3507,2800,2286,1902,		//Turns per square centimetres
1608,1308,1137,997,881,711,609,504,415,341,286,242,176,137,106,87.4,60.8,45.4,35.2,26.8,21.5};


char RAM_EILAM[30][4]={"17","12A","74","23","30","21","31","10","15","33","1", "14","11","34","3","9","9A","11A","4A","2","16",	//EI lamination model number
"5", "4AX","13","75","4","7","6","35A","8"};


float RAM_CORE[30]= {1.213,1.897,2.284,2.723,3.000,3.329,3.703,4.439,4.839,5.880,6.555,6.555,7.259,7.259,		//Core area of bobbin
	7.562,7.865,7.865,9.072,10.284,10.891,10.891,12.704,13.039,14.117,15.324,
15.865,18.969,19.356,39.316,40.803};


float RAM_DIA[34]={0.05,0.06,0.07,0.08,0.09,0.10,0.11,0.12,0.13,0.15,0.17,0.19,0.21,0.23,0.25,0.27,				//diameter of coils
0.29,0.31,0.34,0.37,0.41,0.45,0.50,0.55,0.61,0.71,0.81,0.91,1.01,1.21,1.42,1.62,1.82,2.03};












int main(void)
{
	
	
	key_enter();
	int num1=0;
	
	//priamary voltage
	num1 = key_in();//converts string to integer
	V_PRI=(float) num1;
	
	//Primary frequency
	num1 = key_in();
	F_PRI=(float) num1;
	
	//number of ampearages
	num1 = key_in();
	ACOUNT=num1;
	
	pass_int(ACOUNT);
	
	float A_SEC[ACOUNT];        //secondary ampearage array
	
	//total number of voltages
	num1 = key_in();
	SEC_VTOT=num1;
	
	int NO_OF_VOLTS[ACOUNT];        //Number of voltages for each ampearage
	float V_SEC[SEC_VTOT];          //secondary total voltages array
	int No_Tu_SEC[SEC_VTOT];        //secondary total no of turns array
	float SEC_SWGA[ACOUNT];             //Secondary standard wire gauge
	float SEC_TSQR[ACOUNT];             //Secondary turns per square centimetres
	float D_SEC[ACOUNT];                    //diameter array of secondary coil
	int final_turns_array[SEC_VTOT];      //final number of turns for winding
	int final[SEC_VTOT];
	
	int a=0;
	for(int i=0;i<ACOUNT;i++)
	{
		
		//assigning amearage value
		num1 = key_in();
		A_SEC[i]=(float)num1;
		
		

		//assigning number of voltages to relevant ampearage
		num1 = key_in();
		VCOUNT=num1;
		

		NO_OF_VOLTS[i]=VCOUNT;
		pass_int(VCOUNT);
		
		for(int j=0;j<VCOUNT;j++)
		{
			
			
			num1 = key_in();
			V_SEC[a]=(float)num1;
			a++;
		}
		
	}
	
	
	//calculating secondary total power
	int temp=0;
	for (int i = 0; i <ACOUNT ; i++)
	{
		for(int j=0; j<NO_OF_VOLTS[i] ; j++)
		{
			POW_SEC =POW_SEC + (A_SEC[i]*V_SEC[temp]);
			temp++;
		}

	}
	
	//total secondary power calculation
	POW_SEC_TOT=POW_SEC/1000;
	
	//Primary power calculation
	POW_PRI = POW_SEC_TOT * 1.15;  //0.15=energy loss
	
	//finding core area(Metric standad calculation)
	C_AREA = 1.152 * sqrt(POW_PRI); //unit is square centimetres
	
	//Finding primary ampearage
	A_PRI = (POW_PRI / V_PRI) * 1000; //I=P/V ampearage is in mA
	
	//Finding turns per volt
	Turns_V = 10000 / (4.4 * F_PRI * C_AREA * 1.3); //1.3= Magnetic flux
	
	//finding the turns per volt in Primary winding and secondary winding
	float PT = Turns_V * V_PRI;
	NOT_PRI = (int) PT + 1;
	
	//finding turns per volt in secondary
	for (int i = 0; i <SEC_VTOT ; i++)
	{
		float ST = Turns_V*V_SEC[i];
		No_Tu_SEC[i]=(int) ST + 1;
	}

	
	
	//selecting relevant primary ampearages
	for (int i= 0; i <34; i++)
	{
		if (RAM_AMP[i] >= A_PRI)
		{
			
			PRI_SWGA = RAM_SWGA[i];
			P_SWGA=i;
			break;
		}
		else if ((RAM_AMP[i] < A_PRI) && (RAM_AMP[i + 1] > A_PRI))
		{
			
			PRI_SWGA = RAM_SWGA[i + 1];
			P_SWGA=(i+1);
			break;
		}
	}
	
	//selecting the relevant ampearage and SWGA for secondary
	for (int j = 0; j < ACOUNT; j++)
	{
		for (int i = 0; i <= 34; i++)
		{
			if (RAM_AMP[i] >= A_SEC[j])
			{
				
				SEC_SWGA[j] = RAM_SWGA[i];
				break;
			}
			else if((RAM_AMP[i] < A_SEC[j]) && (RAM_AMP[i + 1] > A_SEC[j]))
			{
				
				SEC_SWGA[j] = RAM_SWGA[i + 1];
				break;
			}
		}
	}

	//selected diameter array for secondary coils
	for (int j = 0; j < ACOUNT; j++)
	{
		for (int i = 0; i <= 34; i++)
		{

			if (RAM_SWGA[i] == SEC_SWGA[j])
			{
				D_SEC[j] = RAM_DIA[i]/0.01;
			}
		}
	}
	
	//Finding primary wire square centimetres
	for (int i = 0; i < 34; i++)
	{
		if (RAM_SWGA[i] == PRI_SWGA)
		{
			PRI_TSQR =RAM_TURNS[i];
			break;
		}

	}
	
	//Finding primary wire diameter
	for (int i = 0; i < 34; i++)
	{
		if (RAM_SWGA[i] == PRI_SWGA)
		{
			D_PRI = RAM_DIA[i]/0.01;
			break;
		}

	}
	
	//Finding secondary turns per square centimetres
	for (int j = 0; j < ACOUNT; j++)
	{
		for (int i = 0; i <= 34; i++)
		{
			if (RAM_SWGA[i] == SEC_SWGA[j])
			{
				SEC_TSQR[j] = RAM_TURNS[i];
				break;
			}
		}
	}
	
	//calculating total area taken by the primary coil
	TOT_PRI_AREA = TOT_PRI_AREA + (NOT_PRI / PRI_TSQR);
	
	//secondary coil total area
	int d=0;
	float temp_SQR=0;
	for(int i=0;i<ACOUNT;i++)
	{
		for (int j = 0; j < NO_OF_VOLTS[i]; j++)
		{
			temp_SQR=No_Tu_SEC[d]/SEC_TSQR[i];
			TOT_SEC_AREA=TOT_SEC_AREA+temp_SQR;
			d++;
		}

	}
	
	//calculating total area taken by the both primary and secondary coil
	TOT_AREA = TOT_PRI_AREA + TOT_SEC_AREA;
	
	
	//selecting relevant EI lamination
	for (int i = 0; i < 30; i++)
	{
		if (RAM_CORE[i] >= TOT_AREA)
		{
			EI_LAMIN =i;
			break;
		}
		else if ((RAM_CORE[i] < TOT_AREA) && (RAM_CORE[i + 1] > TOT_AREA))
		{
			EI_LAMIN=(i +1);
			break;
		}
	}
	
	
	//secondary final number of turns array
	int f=0;
	for(int i=0;i<ACOUNT;i++)
	{
		for (int j = 0; j < NO_OF_VOLTS[i]; j++)
		{
			if(j==0)
			{
				final_turns_array[f] = No_Tu_SEC[f];
			}
			else
			{
				final_turns_array[f] = No_Tu_SEC[f] - No_Tu_SEC[f-1];
			}
			f++;
		}

	}
	
	//passing relevant values to LCD using SPI
	
	
	//printing primary SWGA
	pass_int(P_SWGA);
	key_enter();
	
	
	
	
	//printing secondary SWGA
	int S_VAL;
	for(int i=0;i<ACOUNT;i++)
	{
		
		S_VAL=SEC_SWGA[i];
		_delay_ms(500);
		pass_int(S_VAL);
		_delay_ms(1000);
		key_enter();
		_delay_ms(500);
		
	}
	
	//printing core area
	int CA;
	CA=(int) C_AREA;
	pass_int(CA);
	key_enter();
	
	//priting ei lamination
	pass_int(EI_LAMIN);
	key_enter();
	
	
	
	//Place the bobbin
	buzzer();
	key_enter();
	
	//Measuring x length
	x1_distance=ultrasonic_1();
	x2_distance=ultrasonic_2();
	
	x_measurement=x_fix-x1_distance-x2_distance;
	
	//rotate the bobbin
	buzzer();
	key_enter();
	
	//Measuring y length
	y1_distance=ultrasonic_1();
	y2_distance=ultrasonic_2();
	
	y_measurement=y_fix-y1_distance-y2_distance;
	
	//start measuring z length
	key_enter();
	
	
	
	//Measuring Z length
	z1_distance=ultrasonic_3();
	
	z_measurement=z_fix-z1_distance;
	
	
	
	//z_measurement=6;
	

	//Bobbin Measuring Ending
	cal_core= x_measurement*y_measurement;
	
	
	
	if(cal_core>CA)
	{
		int pass=1;
		pass_int(pass);
		key_enter();
	
	}
	
	else
	{
		int fail=2;
		pass_int(fail);
		key_enter();
		
	}
	
	key_enter();
	
	
	//press # to start winding
	key_enter();
	
	int primary_side= z_measurement/(D_PRI*0.01);
	
	primary_winding(D_PRI,NOT_PRI,primary_side);
	
	int q=0;
	int temp1=ACOUNT-1;
	
	for(int i=0;i<ACOUNT;i++)
	{
	
		
				int length=NO_OF_VOLTS[i];
				if(length>1)
				{
					_delay_ms(500);
					
					int final[length];
					for (int j = 0; j < length; j++)
					{

						final[j]=final_turns_array[q];
						_delay_ms(1000);
						
						q++;
						
					}
			
					for (int j = 0; j < NO_OF_VOLTS[i]; j++)
					{


						int secondary_side= z_measurement/(D_SEC[i]*0.01);
						secondary_winding(D_SEC[i],secondary_side,final,NO_OF_VOLTS[i]);
					}
			
					if(i==temp1)
					{
						int temp2=30;
						pass_int(temp2);
						buzzer();
						key_enter();
					}
					else
					{
						int temp2=10;
						pass_int(temp2);
						buzzer();
						key_enter();
					}
			
				}
				
				else
				{

					int a=0;
					int final[NO_OF_VOLTS[0]];
					for (int j = 0; j < NO_OF_VOLTS[0]; j++)
					{

						final[j]=final_turns_array[a];
						a++;

					}
					int secondary_side= z_measurement/(D_SEC[0]*0.01);
					
					secondary_winding_no_tap(D_SEC[0],final[0],secondary_side);
					
					
					buzzer();
					int temp3=20;
					pass_int(temp3);
					key_enter();
					
				}
						
	}
		
}

