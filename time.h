#pragma once
struct time
{
	int year;
	char month;
	char date;	
	char hour;
	char minute;
	char second;
};

char IsLeapYear(int year)
{
	return ((year %4 == 0 && year%100 !=0)||(year%400 == 0));
}
char Is31DayMonth(char month)
{
	return (month== 1 || month == 3 || month == 5 || month == 7 || month == 8 || month ==10 || month == 12);
}
char Is30DayMonth(char month)
{
	return (month == 4 || month == 6 || month == 9 || month == 11);
}
char MaximumDaysOfMonth(int year, char month)
{
	if(Is31DayMonth(month))
		return 31;
	if(Is30DayMonth(month))
		return 30;
	if(!IsLeapYear(year) && month == 2)
		return 28;
	return 29;
}
void UpdateTime(struct time *t)
{
	if(++t->second == 60)
	{
		t->second = 0;
		if(++t->minute == 60)
		{
			t->minute = 0;
			if(++t->hour == 24)
			{
				t->hour = 0;
				if(++t->date == 1+ MaximumDaysOfMonth(t->year, t->month))
				{
					t->date = 1;
					if(++t->month == 13)
					{
						t->month = 1;
						t->year++;
					}
				}
				
			}
		}
	}
}

int SubtractTime(struct time t0, struct time t1, char c0, char c1)// t1 - t0
{
	if(t1.second >= t0.second)
		return ((t1.second - t0.second)* 1000 + (c1-c0)*25 );
	else
		if(t1.minute>=t0.minute)
			return ((t1.minute-t0.minute)*60000+(t1.second - t0.second)*1000 +(c1-c0)*25);
		else
			if(t1.hour >= t0.hour)
				return ((t1.hour-t0.hour)*3600000+(t1.minute-t0.minute)*60000+(t1.second - t0.second)*1000 +(c1-c0)*25);
			else
				return (3600000+(t1.minute-t0.minute)*60000 + (t1.second - t0.second)*1000 + (c1-c0)*25);
}
void IncreaseTime(struct time *t, char position, char mode) // plus time at specify position 
{
	if(position == 0)
	{
		if(mode == 0)	// time
		{
			if(t->hour == 23)
				t->hour = 0;
			else
				t->hour += 1;
		}
		else			//date
			t->year +=1;
	}
	
	else if(position == 1)		// month or minute	
	{
		if(mode == 0)	// time
		{
			if(t->minute == 59)
				t->minute = 0;
			else
				t->minute += 1;
		}
		else				//date
		{
			if(t->month == 12)
				t->month = 1;
			else
				t->month += 1;
		}
			
	}
		
	else 			// day or second
	{
		if(mode == 0)	// time
		{
			if(t->second == 59)
				t->second = 0;
			else
				t->second += 1;
		}
		else			//date
		{
			if(t->date < MaximumDaysOfMonth(t->year, t->month))
				t->date++;
			else
				t->date = 1;
		}	
	}			
}
void DecreaseTime(struct time *t, char position, char mode) // subtract time at specify position 
{
	if(position == 0)
	{
		if(mode == 0)	// time
			if(t->hour == 0)
				t->hour = 23;
			else
				t->hour -= 1;
		else 			//date
			t->year -=1;
	}
		
	else if(position == 1)		// month or minute	
	{
		if(mode == 0)	// time
			if(t->minute == 0)
				t->minute = 59;
			else
				t->minute -= 1;
		else 			//date
			if(t->month == 1)
				t->month = 12;
			else
				t->month -= 1;
	}
	else
	{
		if(mode == 0)	// time
			if(t->second == 0)
				t->second = 59;
			else
				t->second -= 1;
		else 				//date
		{
			if(t->date > 1)
				t->date--;
			else
				t->date = MaximumDaysOfMonth(t->year, t->month);
		}		
	}	
}