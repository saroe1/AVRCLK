#include <avr/io.h>
#include "i2c.h"

#define i2c_Stop() TWCR = TW_STOP
#ifndef F_CPU
	#define F_CPU 16000000U
#endif

void i2c_init() {
	TWSR = 0; //set prescalar to 0
	TWBR = ((F_CPU/F_SCL) - 16)/2; //Set SCL frequency	
}

void i2c_start(byte slaveAddr) {
	i2c_detect(slaveAddr);
}

void i2c_writeByte(byte busAddr, byte data) {
	i2c_start(busAddr);
	i2c_write(data);
	i2c_Stop();
}

void i2c_writeReg(byte busAddr, byte deviceReg, byte data) {
	i2c_start(busAddr);
	i2c_write(deviceReg);
	i2c_write(data);
	i2c_Stop();
}

byte i2c_detect(byte addr) {
	TWCR = TW_START;
	while (!TW_READY);
	TWDR = addr;
	TWCR = TW_SEND;
	while (!TW_READY);
	return(TW_STATUS == 0x18);
}

byte i2c_findDevice(byte start) {
	for (byte addr = start; addr < 0xFF; addr++) {
		if (i2c_detect(addr)) return addr;
	}
	return 0;
}

byte i2c_write(byte data) {
	TWDR = data;
	TWCR = TW_SEND;
	while (!TW_READY);
	return (TW_STATUS != 0x28);
}

byte i2c_readACK() {
	TWCR = TW_ACK;
	while (!TW_READY);
	return TWDR;
}

byte i2c_readNACK() {
	TWCR = TW_ACK;
	while (!TW_READY);
	return TWDR;
}

byte i2c_readReg(byte busAddr, byte deviceReg) {
	byte data = 0;
	i2c_start(busAddr);
	i2c_write(deviceReg);
	i2c_start(busAddr + READ);
	data = i2c_readNACK();
	i2c_Stop();
	return data;
}


	