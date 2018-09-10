#include<3048.h>
#include "lcd.h"
#include "time.h"

#define BUTTON_PORT P4
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

#define CHANGE_DISPLAY BIT4
#define SETTING BIT5
#define DOWN BIT6
#define UP BIT7

#define TIME 0
#define DATE 1
#define DISPLAY 0
#define SETUP 1
#define BLINK_OFF 3
char c0 = 0,c; 							// for timer 0
char d0 = 0;							// for timer 1

char displayMode = 0, setupMode = 0;	// 0-time 1-date
char mode = 0;
struct time t, t0, t_old;
char position = 0;

void int_imia0 (void)
{
    ITU0.TSR.BIT.IMFA = 0;            	//clear interrupt flag
    c0+=1;
    if(c0==40)
	{	
		UpdateTime(&t);
		P5.DR.BIT.B0 = ~P5.DR.BIT.B0;	//togle led
        c0=0;
    }
}
void int_imia1 (void)					// for function 25ms timer
{
	ITU1.TSR.BIT.IMFA = 0; 				// clear interrupt flag
	d0 +=1;
}
char IsValidPress(char pin)
{
	ITU1.TCNT = 0;
	ITU.TSTR.BIT.STR1 = 1; 				// start timer 1;
	while(d0 <= 5)
	{
		if((BUTTON_PORT.DR.BYTE & pin) == pin)	// if button isn't pressed
			return 0;
	}
	ITU.TSTR.BIT.STR1 = 0;				// halted time 1
	d0 = 0;
	return 1;
}
void timer0_Init()
{
	ITU0.TCR.BIT.CCLR   = 1;       		//timer counter is cleared by GRA
    ITU0.TCR.BIT.TPSC   = 3;      	    //Source clock /8 = 16MHz/8=2MHz
    ITU0.GRA        	= 50000;            //2Mhz/50000=40Hz???25ms
    ITU0.TIER.BIT.IMIEA = 1;       	    //Timer interrupt enable with IMFA flag ( TSR)
}
void timer1_Init()
{
	ITU1.TCR.BIT.CCLR 	= 1; 			//TCNT is cleared by GRA
	ITU1.TCR.BIT.TPSC 	= 3;
    ITU1.GRA        	= 10000;        //2Mhz/10000=200Hz???5ms
	ITU1.TIER.BIT.IMIEA	= 1;			// enable interrupt compare mode flag A
}
void DisplayDateTime(char mode)
{
	if(mode == TIME )
	{	
		lcdxy(1,1);
		dspTime(t, BLINK_OFF);
		
	}
	else if(mode == DATE)
	{
		lcdxy(1,1);
		dspDate(t, BLINK_OFF);
	}	
}
void DisplaySetup(char mode, char blink)
{
	if(mode == TIME )
	{
		lcdxy(1,1);
		if(c0 < 20 || blink == BLINK_OFF) 
			dspTime(t0, BLINK_OFF);
		else
			dspTime(t0, position);
	}
	else
	{	
		lcdxy(1,1);
		if(c0 < 20 || blink == BLINK_OFF)
			dspDate(t0,BLINK_OFF);
		else
			dspDate(t0,position);
	}
}
void DisplayMode()
{
	int timePress = 0;
	while(1)
	{
		DisplayDateTime(displayMode);
		while((BUTTON_PORT.DR.BYTE&0xF0) != 0xF0)
		{
			unsigned char temp = BUTTON_PORT.DR.BYTE&0xF0;
			if((temp != 0xF0 - CHANGE_DISPLAY) && (temp!= 0xF0 - SETTING))
				break;
			else if((BUTTON_PORT.DR.BYTE & CHANGE_DISPLAY) != CHANGE_DISPLAY)
			{

				t_old = AssignTime(t);
				c = c0;
				timePress = 0;
				while((BUTTON_PORT.DR.BYTE & CHANGE_DISPLAY) != CHANGE_DISPLAY)
					if(c0==0)
						DisplayDateTime(displayMode);
				timePress = SubtractTime(t_old, t, c, c0);
				lcdxy(0,11);
				if(timePress < 500)
					displayMode = 1 - displayMode;
			}
			
			else
			{
				t_old = AssignTime(t);
				c = c0;
				timePress = 0;
				while((BUTTON_PORT.DR.BYTE & SETTING) != SETTING)
					if(c0==0)
						DisplayDateTime(displayMode);
				timePress = SubtractTime(t_old, t, c, c0);
				if(timePress < 2000)
				{
					mode = SETUP;
					setupMode = displayMode;
					t0 = AssignTime(t);	
					return;
				}
			}
		}
	}
	
}
void SetupMode()
{
	unsigned int timePress = 0;
	position = 0;
	while(1)
	{
		DisplaySetup(setupMode, 0);

		// if cancel button was pressed
		while((BUTTON_PORT.DR.BYTE & 0xF0)!= 0xF0)
		{
			unsigned char temp = BUTTON_PORT.DR.BYTE & 0xF0;
			if((temp != 0xF0 - CHANGE_DISPLAY) && (temp != 0xF0 - SETTING) && (temp != 0xF0 - UP) && (temp != 0xF0 - DOWN))
				break;
			else if((BUTTON_PORT.DR.BYTE & CHANGE_DISPLAY) != CHANGE_DISPLAY)
			{
				while((BUTTON_PORT.DR.BYTE & CHANGE_DISPLAY) != CHANGE_DISPLAY);
				mode = DISPLAY; // Display time mode
				return;
			}
			// if setup button was pressed
			
			else if((BUTTON_PORT.DR.BYTE & SETTING) != SETTING)
			{
				t_old = AssignTime(t);
				c = c0;
				timePress = 0;
				if(IsValidPress(SETTING))
				{
					while((BUTTON_PORT.DR.BYTE & SETTING) != SETTING);
					timePress = SubtractTime(t_old,t, c, c0);
					if(timePress < 2000)
					{
						position++;
						if(position == 2 && setupMode == DATE)
							while(t0.day > MaximumDaysOfMonth(t0.year, t0.month))
								t0.day--;
						if(position == 3)
						{
							mode = DISPLAY;
							if(setupMode == 1)
							{
								t0.hour = t.hour;
								t0.minute = t.minute;
								t0.second = t.second;
							}
							t = AssignTime(t0);
							position = 0;
							return;
						}
					}
				}
			}
			
			// if UP button was pressed
			else if((BUTTON_PORT.DR.BYTE & UP) != UP)
			{
				t_old = AssignTime(t);
				c = c0;
				timePress = 0;
				while((BUTTON_PORT.DR.BYTE & UP) != UP)
				{
					timePress = SubtractTime(t_old,t, c, c0);
					if((timePress >= 500) && ((timePress%100 == 0)))
					{
						IncreaseTime(&t0, position, setupMode);
						DisplaySetup(setupMode, BLINK_OFF);
					}
				}
				if(timePress < 500)
				{
					IncreaseTime(&t0, position, setupMode);
					DisplaySetup(setupMode, 0);
				}							
			}		
			// if DOWN button was pressed
			else 
			{
				t_old = AssignTime(t);
				c = c0;
				while((BUTTON_PORT.DR.BYTE & DOWN) != DOWN)
				{
					timePress = SubtractTime(t_old,t, c, c0);
					if((timePress >= 500) && ((timePress%100 == 0)))
					{
						DecreaseTime(&t0, position, setupMode);
						DisplaySetup(setupMode,BLINK_OFF);
					}
				}
				if(timePress < 500)
				{
					DecreaseTime(&t0, position, setupMode);
					DisplaySetup(setupMode,0);
				}	
			}	
		}	
	}
}
void main()
{
	//Initial time
	t.hour = 23;
	t.minute = 59;
	t.second = 55;
	t.year = 2018;
	t.month = 2;
	t.day = 1;
	
	lcdini();						//LCD Initiate
	timer0_Init();					//Timer 0 Initiate
	timer1_Init();					//Timer 1 Initiate
	
	//Input Pin Configure
	P5.DDR = 0xff;	
	BUTTON_PORT.DDR = 0x00;
	BUTTON_PORT.PCR.BYTE = 0xFF;
	//Display something
	lcdxy(0,0);
	dsp1g("VER 1 QUOC-CUONG");
	
	EI;  							// Enable Interrupt
	ITU.TSTR.BIT.STR0 = 1;             //Start timer0
	while(1)
	{
		if(mode == DISPLAY)
			DisplayMode();
		else
			SetupMode();
	}
}