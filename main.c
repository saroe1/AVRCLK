/*
 * WiTriAlarm.c
 * Blink test program
 * Created: 9/19/2017 2:35:51 PM
 * Author : Eidolon
 */ 


#include <avr/io.h>
#include "RTC.h"
#include "display.h"
#include "I2C.h"

#define true 1
#define false 0
typedef unsigned char bool;

const long Ring_for_minutes = 10;
const bool AlarmDays[7] = {true, true, true, true, true, false, false};
const bool Alarm_is_PM = false;
const bool Set_RTC = true; //Set time to following defines, compile and upload, then set to false and recompile and upload to hold time on RTC.
//TODO: Future Revision: Enable interrupts to use for setting time via external panel rather than code.

#define DAY_SET 2
#define	HOUR_SET 12
#define MINUTE_SET 25

#define Alarm_Hour 8
#define Alarm_Minute 30
#define Ring_Time 10
//TODO: Future Revision: Enable alarm time set via external panel rather than code
//TODO: Allow Multiple alarms

rtc_t rtcReg;
void beep(uint8_t pitch);

int main(void) {
	//Adafruit_7segment clockDisplay = Adafruit_7segment();  
	//TODO: figure out how to port header files
	
	DDRB |= (1<<DDB5); //Set bit 6 on port B to 1 (set PB5 to output)	Speaker Pin 1/Blink Test
	DDRB |= (0<<DDB4); //Set bit 5 on port B to 0 (set PB4 to input)	Speaker Pin 2/Blink Test
	
	bool alarm = false;
	bool alarmToggle = true; 
	//TODO: create interrupt based on alarm toggle pin. When pulled low, set to false, else set true.
	
	uint8_t alarmMinuteEnd = Alarm_Minute + Ring_Time;
	if(alarmMinuteEnd >= 60) {
		alarmMinuteEnd -= 60;
	}
	
	I2C_Init();
	RTC_Init();
	
	if (Set_RTC) {
		//initialize the RTC values to the set time.
		rtcReg.min = MINUTE_SET;
		rtcReg.hour = HOUR_SET;
		rtcReg.weekDay = DAY_SET;
		RTC_SetDateTime(&rtcReg);
	}
	
    while (1) 
    {
		//Get time
		RTC_GetDateTime(&rtcReg);
		
		//Display time
		
		
		//Check/Ring/End alarm routine
		if(!alarm && (rtcReg.hour == Alarm_Hour) && (rtcReg.min == Alarm_Minute) && AlarmDays[rtcReg.weekDay] && alarmToggle) {
			alarm = true;
			//If alarm condition is met, set alarm variable.
		}
		if(alarm) {
			//If the alarm is ringing
			if(alarm && (rtcReg.min == alarmMinuteEnd)) {
				alarm = false;
			}
			//If alarm variable is set, ring alarm. (Ring by alternating high signal across the speaker pins)
		}

		
		
		
    }
	
	void beep(uint8_t pitch) {
		
		//beep for alarm.
	}
	
	
}

