/*
 * WiTriAlarm.c
 * Blink test program
 * Created: 9/19/2017 2:35:51 PM
 * Author : Eidolon
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "rtc.h"
#include "ssd.h"
#include "i2c.h"
#include "WiTriHeader.h"

#ifndef F_CPU
	#define F_CPU 16000000U
#endif

//TODO: Future Revision: Enable alarm time set via external panel rather than code
//TODO: Allow Multiple alarms

void beep(uint8_t delay);
void avr_init(); //pin mapping done here
void clock_init(); //Misc. setup for clock done here

int main(void) {
	avr_init();
	clock_init();

	bool alarmSet = true; 
	bool dstSet = false;
	
	byte alarmMinuteEnd = Alarm_Minute + Ring_Time;
	
	if(alarmMinuteEnd >= 60) {
		alarmMinuteEnd -= 60;
	}
	
    while (1) 
    {		
		
		if(PINB0 == 0) alarmSet = false;
		if(PINB1 == 0) dstSet = true;
		
		//Get time
		rtc_getTime(&gHour, &gMin, &gSec, &gDay);
		if(dstSet) gHour++; //add one hour if daylight savings time is toggled
		
		//Display time
		ssd_time(gHour, gMin, gSec, gDay);
		
		//Check/Ring/End alarm routine
		if(!alarm && (gHour == Alarm_Hour) && (gMin == Alarm_Minute) && AlarmDays[gDay] && alarmSet) {
			alarm = true;
			//If alarm condition is met, set alarm variable.
		}
		if(snoozed && (gMin == snoozeMin)) {
			alarm = true;
			snoozed = false;
		}
		if(alarm) {
			//If the alarm is ringing
			if(alarm && (gMin == alarmMinuteEnd)) {
				alarm = false;
			}
			//If alarm variable is set, ring alarm. (Ring by alternating high signal across the speaker pins)
			beep(100);	
		}

    }//End while(1)

}//End Main

void beep(uint8_t delay) {
	PINB = (1<<PINB2);
	PINB = (0<<PINB1);
	msDelay(delay);
	PINB = (0<<PINB2);
	PINB = (1<<PINB1);
	msDelay(delay);
	PINB = (0<<PINB1);
	//beep for alarm.
}

void avr_init() {
	DDRC &= ~((1 << DDC2) | (1 << DDC3)); //Clear PC2, PC3, set as inputs.
	PORTC |= ((1<<PORTC2) | (PORTC3)); //Set internal pull-ups on PC2 and PC3
	
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT10);	//Pin Change Interrupt for Alarm_Cancel
	PCMSK1 |= (1 << PCINT11);	//Pin Change Interrupt for Alarm_Snooze
	
	DDRB |= (1 << DDB1);		//Speaker Output pin 1
	DDRB |= (1 << DDB2);		//Speaker Output pin 2
	
	DDRC |= (1 << DDC1);		//Alarm Indicator LED set as output
	
	DDRD &= ~(1 << DDD1);		//DST toggle switch set as input
	DDRC &= ~(1 << DDD2);		//Alarm toggle switch set as input
	
	PORTD |= ((1 << PORTD1) | (1 << PORTD2)); //set internal pull ups on PD1 and PD2
	
	sei();
	
}

void clock_init() {
	i2c_init();
	rtc_init();
	if (Set_RTC) {
		//initialize the RTC values to the set time.
		rtc_set(MONTH_SET, DAY_SET, YEAR_SET, HOUR_SET, MIN_SET, 0);
	}
}

//ISR for Port C, used for pins PC2, PC3
ISR (PCINT1_vect) {
	uint8_t changedBits;
	
	changedBits = PINC ^ portchist;
	portchist = PINC;
	
	rtc_getTime(&gHour, &gMin, &gSec, &gDay);
	
	if(changedBits & (1 << PINC2)) {
		//if PCINT10
		//alarmCancel routine
		alarm = !alarm;
	}
	if(changedBits & (1 << PINC3)) {
		//if PCINT11
		//alarmSnooze routine
		snoozed = true;
		alarm = !alarm;
		snoozeMin = gMin + Snooze_Time;
		if(snoozeMin >= 60) snoozeMin -= 60;
	}
}
