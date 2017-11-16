#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include "i2c.h"
#include "rtc.h"
#include "ssd.h"
#include <util/delay.h>

void ssd_init() {
	i2c_writeByte(HT16K33,HT16K33_ON);
	i2c_writeByte(HT16K33,HT16K33_DISPLAYON);
	i2c_writeByte(HT16K33,HT16K33_DIM + 15);
}

void ssd_setDigRaw(byte digit, byte data) {
	if(digit > 4) return;
	if(digit > 1) digit++;
	digit <<= 1;
	i2c_writeReg(HT16K33,digit,data);
}

void ssd_blankDig(byte digit) {
	ssd_setDigRaw(digit,0x00);
}

void ssd_setDig(byte digit, byte data) {
	if(data > 0x10) return;
	ssd_setDigRaw(digit,numberTable[data]);
}

void ssd_setColon(byte data) {
	i2c_writeReg(HT16K33,0x04,data<<1);
}

void ssd_setDigits(byte d0, byte d1, byte d2, byte d3, byte colon) {
	ssd_setDig(0,d0);
	ssd_setDig(1,d1);
	ssd_setDig(2,d2);
	ssd_setDig(3,d3);
	ssd_setColon(colon);
	
}

void ssd_int(int data, byte base) {
	char st[5]="";
	itoa(data,st,base);
	byte len = strlen(st);
	if(len>4) return;
	for(byte digit = 0; digit < 4; digit++) {
		byte blanks = 4 - len;
		if(digit < blanks) ssd_setDig(digit,0x10);
		else {
			char ch = st[digit-blanks];
			if(ch >= 'a') ch-=87;
			else ch-='0';
			ssd_setDig(digit,ch);
		}
	}
}

void ssd_intTest() {
	for(int i = 0; i < 256; i++) {
		ssd_int(i,16);
		msDelay(150);
	}
}

void ssd_circle() {
	for(byte count = 0; count < 15; count ++) {
		for(byte i = 0; i < 6; i++) {
			ssd_setDigRaw(0, 1<<i);
			ssd_setDigRaw(1, 1<<i);
			ssd_setDigRaw(2, 1<<i);
			ssd_setDigRaw(3, 1<<i);
			msDelay(100);
		}
	}
}

void ssd_time(byte hour, byte min, byte sec, byte dayOW) {
	rtc_getTime(&hour, &min, &sec, &dayOW);
	ssd_setDigits(hour>>4, hour & 0x0F, min>>4, min & 0x0F, 1);
}

void msDelay(int delay) {
	for(int i = 0; i < delay; i++) {
		_delay_ms(1);
	}
}