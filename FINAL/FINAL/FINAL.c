/*
 * FINAL.c
 * Created: 4/12/2016 4:01:32 PM
 *  Author: Fighting Mongooses
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "AIOHeader.h"
#include "math.h"
#include "open_interface.h"
#include "lcd.h"

#define CONST_SPEED 200

/**
 * Takes the specific characters typed and puts them into an array in order to move and rotate the robot. 
 * If the first character of the array is a letter then if it is an m, moves robot, r, rotates the robot clock-wise, l, rotates the robot counter clock-wise, s, sweep with the servo, or p, play music and stop the program.
 * If the second character of the array is a number, then that number is the corresponding value that the robot rotates or the speed at which the robot moves.
 */

int main(void)
{
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	lcd_init();
	IR_init();
	IR_lookup_init();
	UART_init();
	servo_init();
	ping_init();
	int finished = 0;
	char cmd_char;
	
    while(finished != 1)
    {
		cmd_char = UART_rec();
	
		lprintf("%c", cmd_char);
		
		if(cmd_char == 'm') {
			cmd_char = UART_rec();
			
			if(cmd_char == '1') {
				char confirm[20] = "\n\rMoving 5cm\n\r\0";
				UART_transmit_str(confirm);
				move_forward(sensor_data, CONST_SPEED, 50);
			}
			if(cmd_char == '2') {
				char confirm[20] = "\n\rMoving 15cm\n\r\0";
				UART_transmit_str(confirm);
				move_forward(sensor_data, CONST_SPEED, 150);
			}
			if(cmd_char == '3') {
				char confirm[20] = "\n\rMoving 25cm\n\r\0";
				UART_transmit_str(confirm);
				move_forward(sensor_data, CONST_SPEED, 250);
			}
			if(cmd_char == '4') {
				char confirm[20] = "\n\rMoving 50cm\n\r\0";
				UART_transmit_str(confirm);
				move_forward(sensor_data, CONST_SPEED, 500);
			}
			if(cmd_char == '5') {
				char confirm[20] = "\n\rMoving 100cm\n\r\0";
				UART_transmit_str(confirm);
				move_forward(sensor_data, CONST_SPEED, 1000);
				//turn_cw(sensor_data, 45);
			}	
		}
		
		if(cmd_char == 'r') {
			cmd_char = UART_rec();
			
			if(cmd_char == '1') {
				char confirm[20] = "\n\rRotate 10deg\n\r\0";
				UART_transmit_str(confirm);
				turn_cw(sensor_data,10);
			}
			if(cmd_char == '2') {
				char confirm[20] = "\n\rRotate 25deg\n\r\0";
				UART_transmit_str(confirm);
				turn_cw(sensor_data,25);
			}
			if(cmd_char == '3') {
				char confirm[20] = "\n\rRotate 45deg\n\r\0";
				UART_transmit_str(confirm);
				turn_cw(sensor_data,45);
			}
			if(cmd_char == '4') {
				char confirm[20] = "\n\rRotate 90deg\n\r\0";
				UART_transmit_str(confirm);
				turn_cw(sensor_data,90);
				
			}
			if(cmd_char == '5') {
				
				char confirm[20] = "\n\rRotate 180deg\n\r\0";
				UART_transmit_str(confirm);
				turn_cw(sensor_data,180);
			}
		}
		
		if(cmd_char == 'l') {
			cmd_char = UART_rec();
			
			if(cmd_char == '1') {
				char confirm[20] = "\n\rRotate -10deg\n\r\0";
				UART_transmit_str(confirm);
				turn_ccw(sensor_data,10);
			}
			if(cmd_char == '2') {
				char confirm[20] = "\n\rRotate -25deg\n\r\0";
				UART_transmit_str(confirm);
				turn_ccw(sensor_data,25);
			}
			if(cmd_char == '3') {
				char confirm[20] = "\n\rRotate -45deg\n\r\0";
				UART_transmit_str(confirm);
				turn_ccw(sensor_data,45);
			}
			if(cmd_char == '4') {
				char confirm[20] = "\n\rRotate -90deg\n\r\0";
				UART_transmit_str(confirm);
				turn_ccw(sensor_data,90);
			}
			if(cmd_char == '5') {
				char confirm[20] = "\n\rRotate -180deg\n\r\0";
				UART_transmit_str(confirm);
				turn_ccw(sensor_data,180);
			}
		}
		
		if(cmd_char == 's') {	
			UART_transmit_str("Sending Sweep\n\r\0");
			sensor_sweep_init();
			sensor_sweep();
			
			char confirm[20] = "\n\rSweep Complete\n\r\0";
			UART_transmit_str(confirm);
			
		}
	
		if(cmd_char == 'p') {
			UART_transmit_str("Confirm play? Y/N\n\r\0");
			cmd_char = UART_rec();
			if(cmd_char == 'y' || cmd_char == 'Y') {
				char confirm[20] = "\n\rPlaying Audio\n\r\0";
				UART_transmit_str(confirm);
				play_song();
				finished = 1;
			}	
		}
		
    }
	
	if(finished == 1) oi_free(sensor_data);
}
