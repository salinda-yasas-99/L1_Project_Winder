#define F_CPU 8000000UL		/* Define CPU Frequency 8MHz */
#include <avr/io.h>		/* Include AVR std. library file */
#include <util/delay.h>		/* Include delay header file */
#include "motors.h"

void bobbin();
void primary_hand(int g);
void secondary_hand(int g);
void primary_hand_reverse(int g);
void secondary_hand_reverse(int g);
void tape_fun_1(int count,int g);
void tape_arm_1_placing(int r);
void tape_arm_1(int g);
void tape_arm_1_reverse(int g);
void tape_fun_2(int count,int g);
void tape_arm_2_placing(int r);
void tape_arm_2(int g);
void tapping_process(int d,int shift,int side,int g);
void covering_tape(int selection,int g);
void primary_winding(int gauge,int round,int side);
void secondary_winding(int gauge,int side,int* passed_tap_arr,int no_of_taps);
void primary_winding_end(int d,int g);
void secondary_winding_end(int d,int g);



void bobbin()
{
	SM1_PORT = (((SM1_PORT&~(1<<s3))&~(1<<s2))&~(1<<s1));	//	selecting bobbin stepper
	stepper_1(1);	//	rotate one round of bobbin
}

void primary_hand(int g)
{
	SM1_PORT = (((SM1_PORT&~(1<<s3))&~(1<<s2))|(1<<s1));
	stepper_1(g);	//	g = gauge of wire / 0.01mm
	
}

void secondary_hand(int g)
{
	SM1_PORT = (((SM1_PORT&~(1<<s3))|(1<<s2))&~(1<<s1));	//selecting secondary hand stepper
	stepper_1(g);	//	g = gauge of wire / 0.01mm
	
}

void secondary_hand_reverse(int g)
{
	SM1_PORT = (((SM1_PORT&~(1<<s3))|(1<<s2))&~(1<<s1));	//selecting secondary hand stepper
	stepper_1_reverse(g);	//	g = gauge of wire / 0.01mm
	
}

void primary_hand_reverse(int g)
{
	SM1_PORT = (((SM1_PORT&~(1<<s3))&~(1<<s2))|(1<<s1));	////selecting primary hand stepper
	stepper_1_reverse(g);	//	g = gauge of wire / 0.01mm
	
}

void tape_fun_1(int count,int g)	//g=  how many times as 0.01
{
	int w=0.05;	//	w =width of the tape
	if(count*0.01<w)
	{
		bobbin();
		primary_hand(g);
	}
	else if ((count*0.01<w+0.1)&&(count*0.01<w+0.1))
	{
		bobbin();
		primary_hand(g);
		tape_arm_1_placing(2);	//	width of the tape+moving distance to the bobbin starting point)/0.01mm
		
	}
	else
	{
		bobbin();
		primary_hand(g);
		tape_arm_1(g);
	}
}

void tape_arm_1_placing(int r)
{
	SM1_PORT = (((SM1_PORT&~(1<<s3))|(1<<s2))|(1<<s1));		//	selecting tape arm_1
	stepper_1(r);	//	tape arm 1 is moving to the starting point
	SERS_PORT = ((SERS_PORT|(1<<s5))&~(1<<s4));		//	selecting tape arm_1 servo
	servo_half();	//	bend down to stick the tape
}

void tape_arm_1(int g)
{
	SM1_PORT = (((SM1_PORT&~(1<<s3))|(1<<s2))|(1<<s1));
	stepper_1(g);
}

void tape_arm_1_reverse(int g)
{
	SM1_PORT = (((SM1_PORT&~(1<<s3))|(1<<s2))|(1<<s1));
	stepper_1_reverse(g);
}

void tape_fun_2(int count,int g)
{
	int w=0.05;	//	w =width of the tape
	if(count*0.01<w)
	{
		bobbin();
		primary_hand_reverse(g);
	}
	else if ((count*0.01<w+0.1)&&(count*0.01<w+0.1))
	{
		bobbin();
		primary_hand_reverse(g);
		tape_arm_2_placing(2);	//	(width of the tape+moving distance to the bobbin starting point)/0.01mm
	}
	else
	{
		bobbin();
		primary_hand_reverse(g);
		tape_arm_2(g);
	}
}

void tape_arm_2_placing(int r)
{
	SM1_PORT = (((SM1_PORT|(1<<s3))&~(1<<s2))&~(1<<s1));		//	selecting tape arm_2
	stepper_1(r);	//	tape arm 2 is moving to the starting point
	SERS_PORT = ((SERS_PORT|(1<<s5))|(1<<s4));		//	selecting tape arm_2 servo
	servo_half();	//	bend down to stick the tape
}

void tape_arm_2(int g)
{
	SM1_PORT = (((SM1_PORT|(1<<s3))&~(1<<s2))&~(1<<s1));
	stepper_1(g);
}


void tapping_process(int d,int shift,int side,int g)
{
	int l=0;
	if (shift%2==1)
	{
		l=side-d;	//d = distance move to the starting point of bobbin
	}
	SM1_PORT = (((SM1_PORT&~(1<<s3))|(1<<s2))&~(1<<s1));	//	selecting secondary hand stepper
	stepper_1_reverse(l+1);									// disatance 1=50000
	SERS_PORT = ((SERS_PORT&~(1<<s5))|(1<<s4));		//	selecting secondary hand servo
	servo_half_stop();
	SM1_PORT = (((SM1_PORT|(1<<s3))&~(1<<s2))|(1<<s1));		//	selecting tapping rotation stepper
	stepper_1_first_half();
	SM1_PORT = (((SM1_PORT|(1<<s3))|(1<<s2))&~(1<<s1));		//	selecting tapping movement stepper
	stepper_1(2);											// move pre-defined distance for the location(2000)
	SM1_PORT = (((SM1_PORT&~(1<<s3))|(1<<s2))&~(1<<s1));	//	selecting secondary hand stepper
	stepper_1(2);											// move pre-defined distance for the location(2000)
	SM1_PORT = (((SM1_PORT|(1<<s3))&~(1<<s2))|(1<<s1));		//	selecting tapping rotation stepper
	stepper_1_second_half();
	SERS_PORT = ((SERS_PORT&~(1<<s5))|(1<<s4));				//	selecting secondary hand servo
	servo_half_stop_reverse();
	SM1_PORT = (((SM1_PORT&~(1<<s3))&~(1<<s2))|(1<<s1));	//	selecting secondary hand stepper
	stepper_1_reverse(l+5);									//	move to the previous location (48000)
	SM1_PORT = (((SM1_PORT|(1<<s3))&~(1<<s2))|(1<<s1));		//	selecting tapping rotation stepper
	stepper_1_reverse(1);									// rotate to the previous location
	SM1_PORT = (((SM1_PORT|(1<<s3))|(1<<s2))&~(1<<s1));		//	selecting tapping movement stepper
	stepper_1_reverse(2);
	if (shift%2==0)
	{
		tape_arm_2_placing(d);	//placing tape arm 2
	}
	else
	{
		tape_arm_1_placing(d);	//placing tape arm 1
	}
}

void covering_tape(int selection,int g)
{
	if (selection ==1)	// check to use tape arm_1
	{
		tape_arm_1_reverse(g);
	}
	else
	{
		tape_arm_2(g);	// check to use tape arm_2
	}
}

void cutting_process()
{
	SM1_PORT = (((SM1_PORT|(1<<s3))|(1<<s2))|(1<<s1));
	stepper_1(2);										//	moving to pre-defined distance
	SM2_PORT = SM2_PORT|(1<<s6);
	stepper_2();										//	moving to cut the wire
	SM1_PORT = (((SM1_PORT|(1<<s3))|(1<<s2))|(1<<s1));
	stepper_1_reverse(2);								//	moving back to the previous location
}

void primary_winding_end(int d,int g)
{
	int x=1;			//	distance for the cutter from bobbin
	SM1_PORT = (((SM1_PORT&~(1<<s3))&~(1<<s2))|(1<<s1));
	stepper_1_reverse(d*g+x);
	SERS_PORT = ((SERS_PORT&~(1<<s5))&~(1<<s4));		//	selecting primary arm servo
	servo_half_stop();
	cutting_process();
	SERS_PORT = ((SERS_PORT&~(1<<s5))&~(1<<s4));		//	selecting primary arm servo
	servo_half_stop_reverse();
	SM1_PORT = (((SM1_PORT&~(1<<s3))&~(1<<s2))|(1<<s1));
	stepper_1(3);										// moving to the defined end point
}

void secondary_winding_end(int d,int g)
{
	int x=1;											//	distance for the cutter from bobbin
	SM1_PORT = (((SM1_PORT&~(1<<s3))|(1<<s2))&~(1<<s1));	//	selecting secondary arm
	stepper_1_reverse(d*g+x);
	SERS_PORT = ((SERS_PORT&~(1<<s5))|(1<<s4));			//	selecting secondary arm servo
	servo_half_stop();
	cutting_process();
	SERS_PORT = ((SERS_PORT&~(1<<s5))|(1<<s4));			//	selecting secondary arm servo
	servo_half_stop_reverse();
	SM1_PORT = (((SM1_PORT&~(1<<s3))|(1<<s2))&~(1<<s1));
	stepper_1(3);											// moving to the defined end point
}


void primary_winding(int gauge,int round,int side)
{
	int shift=0;
	while (1)
	{
		if(round>side)
		{
			if((round-(shift*side))>side)
			{
				if((shift%2)==0)
				{
					for(int i=0;i<side;i++)
					{
						bobbin();
						primary_hand(gauge);
					}
					shift++;
				}
				else
				{
					for(int i=0;i<side;i++)
					{
						bobbin();
						primary_hand_reverse(gauge);
					}
					shift++;
				}
			}
			else
			{
				if((shift%2)==0)
				{
					for(int i=0;i<(round-(shift*side));i++)
					{
						tape_fun_1(i+1,gauge);
					}
					break;
				}
				else
				{
					for(int i=0;i<(round-(shift*side));i++)
					{
						tape_fun_2(i+1,gauge);
					}
					break;
				}
			}
		}
		else
		{
			for(int i=0;i<round;i++)
			{
				bobbin();
				primary_hand(gauge);
				tape_fun_1(i+1,gauge);
				break;
			}
		}
	}
	int start_distance;
	if(shift%2==1)
	{
		start_distance= (side-(round-(shift*side)));
	}
	else
	{
		start_distance= (round-(shift*side));
	}
	primary_winding_end(start_distance,gauge);
}

void secondary_winding_no_tap(int gauge,int round,int side)
{
	int shift=0;
	while (1)
	{
		if(round>side)
		{
			if((round-(shift*side))>side)
			{
				if((shift%2)==0)
				{
					for(int i=0;i<side;i++)
					{
						bobbin();
						secondary_hand(gauge);
					}
					shift++;
				}
				else
				{
					for(int i=0;i<side;i++)
					{
						bobbin();
						secondary_hand_reverse(gauge);
					}
					shift++;
				}
			}
			else
			{
				if((shift%2)==0)
				{
					for(int i=0;i<(round-(shift*side));i++)
					{
						tape_fun_1(i+1,gauge);
					}
					break;
				}
				else
				{
					for(int i=0;i<(round-(shift*side));i++)
					{
						tape_fun_2(i+1,gauge);
					}
					break;
				}
			}
		}
		else
		{
			for(int i=0;i<round;i++)
			{
				bobbin();
				secondary_hand(gauge);
				tape_fun_1(i+1,gauge);
				break;
			}
		}
	}
	int start_distance;
	if(shift%2==1)
	{
		start_distance= (side-(round-(shift*side)));
	}
	else
	{
		start_distance= (round-(shift*side));
	}
	secondary_winding_end(start_distance,gauge);
}


void secondary_winding(int gauge,int side,int* passed_tap_arr,int no_of_taps)
{
	int round=0;
	int shift = 0;
	int k = 0;
	for (int j = 0; j < no_of_taps; j++)
	{
		round = passed_tap_arr[j];
		while (1)
		{
			if (round > side)
			{
				if ((round - (shift * side)) > side)
				{
					if ((shift % 2) == 0)
					{
						for (int i = 0; i < side; i++)
						{
							if (k > 0)
							{
								bobbin();
								secondary_hand(gauge);
								covering_tape(2, gauge);
								k--;
							} else
							{
								bobbin();
								secondary_hand(gauge);
								k--;
							}

						}
						shift++;
					} else
					{
						for (int i = 0; i < side; i++)
						{
							if (k > 0)
							{
								bobbin();
								secondary_hand_reverse(gauge);
								covering_tape(1, gauge);
								k--;
							} else
							{
								bobbin();
								secondary_hand_reverse(gauge);
								k--;
							}
						}
						shift++;
					}
				}
				else
				{
					if ((shift % 2) == 0)
					{
						for (int i = 0; i < (round - (shift * side)); i++)
						{
							tape_fun_1(i + 1, gauge);
							k = i;
						}
						tapping_process(k, shift, side,gauge);    // k= remaining rounds to cover the crossed tapping wire
						break;
					} else
					{
						for (int i = 0; i < (round - (shift * side)); i++)
						{
							tape_fun_2(i + 1, gauge);
							k = i;
						}
						tapping_process(k, shift, side,
						gauge);    // k= remaining rounds to cover the crossed tapping wire
						break;
					}
				}

			}
			else
			{
				for (int i = 0; i < round; i++)
				{
					bobbin();
					secondary_hand(gauge);
					tape_fun_1(i + 1, gauge);
					k = i;
				}
				tapping_process(k, shift, side, gauge);    // k= remaining rounds to cover the crossed tapping wire
				break;
			}

		}

	}
	int start_distance;
	if(shift%2==1)
	{
		start_distance= (side-(round-(shift*side)));
	}
	else
	{
		start_distance= (round-(shift*side));
	}
	secondary_winding_end(start_distance,gauge);
}