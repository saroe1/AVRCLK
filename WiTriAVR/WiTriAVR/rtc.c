#include <avr/io.h>
#include "rtc.h"
#include "i2c.h"

#define i2c_Stop() TWCR = TW_STOP

void rtc_init() {
	i2c_start(RTC_WRITE);
	i2c_write(CONTROL_REG);
	i2c_write(0x00);
	i2c_Stop();
}

void rtc_getTime(byte *hour, byte *min, byte *sec, byte *dayOW) {
	*hour = i2c_readReg(RTC_READ,HOURS_REG);
	*min = i2c_readReg(RTC_READ,MIN_REG);
	*sec = i2c_readReg(RTC_READ,SEC_REG);
	*dayOW = i2c_readReg(RTC_READ,DAYOFWK_REG);
	
	if (*hour & 0x40)
		*hour &=0x1F;
	else *hour &= 0x3F;
}

void rtc_getDate(byte *months, byte *days, byte *years) {
	*months = i2c_readReg(RTC_READ,MONTHS_REG);
	*days = i2c_readReg(RTC_READ,DAYS_REG);
	*years = i2c_readReg(RTC_READ,YEARS_REG);
}

void rtc_set(byte month, byte day, byte year, byte hour, byte min, byte sec) {
	i2c_writeReg(RTC_WRITE,MONTHS_REG,month);
	i2c_writeReg(RTC_WRITE,DAYS_REG,day);
	i2c_writeReg(RTC_WRITE,YEARS_REG,year);
	i2c_writeReg(RTC_WRITE,HOURS_REG,hour);
	i2c_writeReg(RTC_WRITE,MIN_REG,min);
	i2c_writeReg(RTC_WRITE,SEC_REG,sec);
}