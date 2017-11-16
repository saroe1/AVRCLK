/*
*	display.c
*	Library for Adafruit 7seg backpack
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "display.h"
#include "I2C.h"

void SS_Init() {
	I2C_Write(LED_BKPK);			//Send backpack address over I2C
	I2C_Write(LED_BKPK_ON);			//Turn on backpack
	
	I2C_Write(LED_BKPK);			//Send backpack address
	I2C_Write(LED_BKPK_DISPLAYON);	//Turn on 7seg
	
	I2C_Write(LED_BKPK);			//Send backpack address
	I2C_Write(LED_BKPK_DIM + 15);	//Set 7seg brightness
}

void SS_set_digit(uint8_t digit, uint8_t data) {
	if(digit > 4) return;
	if(digit > 1) digit++;
	digit <<= 1;
	I2C
	
}