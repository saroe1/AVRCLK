/*
*	WiTriHeader.h
*	This file contains all of the "#define" statements and global variables for main.
*
*/
#include <avr/io.h>
#include "i2c.h"
#include "rtc.h"
//#include "ssd.h"


#define true 1
#define false 0
typedef unsigned char bool;

const long Ring_for_minutes = 10;
const bool AlarmDays[7] = {true, true, true, true, true, false, false};
const bool Alarm_is_PM = false;
const bool Set_RTC = true; //Set time to following defines, compile and upload, then set to false and recompile and upload to hold time on RTC.
//TODO: Future Revision: Enable interrupts to use for setting time via external panel rather than code.

#define MONTH_SET 1
#define YEAR_SET 17
#define DAY_SET 2
#define	HOUR_SET 12
#define MIN_SET 25

#define Alarm_Hour 8
#define Alarm_Minute 30
#define Ring_Time 10
#define Snooze_Time 10

volatile uint8_t portchist = 0xFF;
bool snoozed = false;	//Snooze button hit status variable
bool alarm = false;		//Alarm should be ringing status variable
uint8_t snoozeMin = 0;

byte gHour;
byte gMin;
byte gSec;
byte gDay;