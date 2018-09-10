#pragma once
#include<3048.h>
#include"time.h"

#define  E_SIG  0x20       //E
#define  RS_SIG 0x10       //RS
#define line1 0x08
#define line2 0x0C
static char arr[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
void wait()                 //delay
{
    int t = 500;
    while(t--);
}

void lcdo8(unsigned char d)     //Ham truyen gui data
{
    d=d | E_SIG;                //Set Enable
    P3.DR.BYTE = d;             //out data to bus
    d = d & 0xdf;               //clear Enable
	P3.DR.BYTE = d;             //out data to bus
    wait();                     //delay
}

void lcdini(void)               //LCD Initial
{
	P3.DDR = 0xFF;
	wait();
    lcdo8(3);    
	wait();
    lcdo8(3); 
	wait();
    lcdo8(3); 
	wait();
    lcdo8(2); 
	wait();	
    lcdo8(2);                   
	wait();
    lcdo8(8);                   
	wait();
    lcdo8(0);      
	wait();
    lcdo8(0x0c);                
	wait();
    lcdo8(0);      
	wait();
    lcdo8(6);                   //index increase 1, no shift.
	wait();
}

void lcdclr(void)               //clear screen
{
    P3.DR.BYTE = 0;
    lcdo8(0);
    lcdo8(1);                   
    wait();
}

void lcdxy(unsigned char y,unsigned char x) //move cursor to line y, column x+1
{
    P3.DR.BYTE = 0;
	if(y == 0)
		lcdo8(line1);               //line 1 -> y=8  line 2 -> y=0x0c
	if(y == 1)
		lcdo8(line2);
    lcdo8(x);               //x : 0--> 15
    wait();
}
void lcdo4(unsigned char d)     //hien thi ky tu d
{
    unsigned dd;
    dd = d;                     // bien luu 4 bit cao
    d = d >> 4;                 //dich phai
    d = d & 0x0f;               //clear 4 bit cao
    d = d | RS_SIG;             //set send data
    lcdo8(d);                   //truyen 4 bit cao
    wait();                     //delay
    dd = dd & 0x0f;             //clear 4 bit cao
    dd = dd | RS_SIG;           //set send data
    lcdo8(dd);                  //truyen 4 bit thap
    wait();
}
void dsp1g(char *str)           //hien thi string
{
    int i = 0;                 
    while(str[i] !=0)
    {
        lcdo4(str[i]);
        i++;
    }
}
void dspNumber(int number)
{
	char i;
	if(number < 0 )
	{
		lcdo4('-');
		number = -number;
	}
	int mod = 1;
	while(number/mod !=0)
	{
		mod *= 10;
	}
	mod /= 10;
	while(mod !=0)
	{
		if(number !=0)
		{
			i = number/mod;
			lcdo4(i + 48);
			number -= mod*i;
		}
		else
		{
			lcdo4('0');
		}
		mod /= 10;
	}
}
void dspTime(struct time t, char position)
{
	if(position!=0)
	{
		if(t.hour == 0)
			dsp1g("00");
		else 
		{
			if(t.hour < 10)
				lcdo4('0');
			dspNumber(t.hour);
		}
		
	}
	else
		dsp1g("  ");
	lcdo4(':');
	
	if(position !=1)
	{
		if(t.minute == 0)
			dsp1g("00");
		else 
		{
			if(t.minute < 10)
				lcdo4('0');
			dspNumber(t.minute);
		}
	}
	else
		dsp1g("  ");
	lcdo4(':');
	
	if(position != 2)
	{
		if(t.second == 0)
			dsp1g("00");
		else 
		{
			if(t.second < 10)
				lcdo4('0');
			dspNumber(t.second);
		}
	}

	dsp1g("       ");
}

void dspDate(struct time t, char position)
{
	
	if(position != 0)
		dspNumber(t.year);
	else
		dsp1g("    ");
	lcdo4('/');
	t.year-= t.month < 3;
	char day = (t.year + t.year/4 - t.year/100 + t.year/400 + arr[t.month-1] + t.day) % 7;
	if(position != 1)
	{
		if(t.month < 10)
			lcdo4('0');
		dspNumber(t.month);
	}
	else
		dsp1g("  ");
	lcdo4('/');
	
	if(position != 2)
	{
		if(t.day < 10)
			lcdo4('0');
		dspNumber(t.day);		
	}	
	else
		dsp1g("  ");
	lcdo4('(');
	
	switch(day)
	{
		case 0: 
			dsp1g("SUN");
			break;
		case 1: 
			dsp1g("MON");
			break;
		case 2: 
			dsp1g("TUE");
			break;
		case 3: 
			dsp1g("WED");
			break;
		case 4: 
			dsp1g("THU");
			break;
		case 5: 
			dsp1g("FRI");
			break;
		case 6: 
			dsp1g("SAT");
			break;			
	}
	lcdo4(')');
}

