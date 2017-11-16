#include <avr/io.h>
#include <avr/interrupt.h>
#include "AIOHeader.h"
#include "math.h"
#include "open_interface.h"
#include "lcd.h"
#include <stdlib.h>
#include <string.h>


volatile unsigned int timer2_tick;

int IR_arr[1024];

void timer2_start(char unit);
void timer2_stop();
void wait_ms(unsigned int time_val);

void UART_init();

volatile unsigned long time_diff;
volatile int time;
volatile int time_last;
volatile int time_current;
volatile int distance;
volatile int overflows = 0;
volatile int new_overflows = 0;

///ISR's//////////////////////////////////////////////////

/// Interrupt handler (runs every 1 ms)
/// Taken from given util.c
ISR (TIMER2_COMP_vect) {
	timer2_tick++;
}
ISR (TIMER1_OVF_vect)
{
	new_overflows++;
}
ISR (TIMER1_CAPT_vect)
{
	time_last = time_current;
	overflows = new_overflows;
	time_current = ICR1;
	new_overflows = 0;
	
	time_diff = (overflows<<16) + time_diff;
	
	TIFR |= 0b00100000;
	TCCR1B = 0b10000100;
}
///WAIT CODE////////////////////////////////////////////////

/// Blocks for a specified number of milliseconds
void wait_ms(unsigned int time_val) {
	//Seting OC value for time requested
	OCR2=250; 				//Clock is 16 MHz. At a prescaler of 64, 250 timer ticks = 1ms.
	timer2_tick=0;
	timer2_start(0);

	//Waiting for time
	while(timer2_tick < time_val);

	timer2_stop();
}

/// Start timer2 for use with wait_ms function
void timer2_start(char unit) {
	timer2_tick=0;
	if ( unit == 0 ) { 		//unit = 0 is for slow
		TCCR2=0b00001011;	//WGM:CTC, COM:OC2 disconnected,pre_scaler = 64
		TIMSK|=0b10000000;	//Enabling O.C. Interrupt for Timer2
	}
	if (unit == 1) { 		//unit = 1 is for fast
		TCCR2=0b00001001;	//WGM:CTC, COM:OC2 disconnected,pre_scaler = 1
		TIMSK|=0b10000000;	//Enabling O.C. Interrupt for Timer2
	}
	sei();
}

/// Stop timer2 for use with wait_ms funcion
void timer2_stop() {
	TIMSK&=~0b10000000;		//Disabling O.C. Interrupt for Timer2
	TCCR2&=0b01111111;		//Clearing O.C. settings
}

///UART CONTROL//////////////////////////////////////////////
/// Initialize UART
void UART_init() {

	//baud = (FCLK/8/baud)-1;
	
	//set up baud rate
	UBRR0H = (unsigned char) (BTBAUD >> 8);
	UBRR0L = (unsigned char) BTBAUD;

	//double speed mode
	UCSR0A = 0b00000010;
	//TX and RX enable
	UCSR0B = 0b00011000;
	//8 Data bits, 2 stop bits
	UCSR0C = 0b00001110;

}

/// Receive UART data
char UART_rec() {
	
	while((UCSR0A & 0b10000000) == 0);
	return UDR0;
	
}

/// Transmits a string to the robot
/**
 * Takes a string input into the function and transmits the string to the robot one character at a time
 * @param data[] a char array that indicates the string that is being transmitted to the robot
 */
void UART_transmit_str(char data[]) {
	int len = (strlen(data));
	
	for (int i = 0; i < len; i++) {
		while ((UCSR0A & 0b00100000) == 0);
		UDR0 = data[i];
	}
}

/// Transmits a character to the robot
/**
 * Takes a character input into the function and transmits the character to the robot
 * @param character a char that indicates the character that is being transmitted to the robot
 */
void UART_transmit_char(char character){
	while ((UCSR0A & 0b00100000) == 0);
	UDR0 = character;
};

///IR SENSOR CONTROL//////////////////////////////////////////
/// Initialize IR Sensor
void IR_init(void){
	ADCSRA = 0b10000111;
	ADMUX = 0b110000010;
}

///Read ADC values from IR sensor
unsigned int IR_read() {
	
	ADCSRA = (ADCSRA | 0b01000000);
	while(ADCSRA == 0b11000111);
	return ADC;
	
}

/// Creates a lookup table to calculate the distance at each IR Sensor value
void IR_lookup_init()
{
	for(int i = 0; i < 1024;)
	{
		IR_arr[i] = 15146 * pow(i,-1.066);
		i++;
	}
}

/// Smooths out the IR value by averaging the value of 5 IR readings and returns the value as a double
double IR_smooth()
{
	double sum = 0;
	for(int i = 0; i < 5;)
	{
		int quantization = IR_read();
		sum = sum + IR_arr[quantization]; // 15146 * pow(quantization,-1.066);
		i++;
	}
	return sum/5.0;
}

///PING SENSOR CONTROL/////////////////////////////////////////////
/// Initialize the Ping Sensor
void ping_init()
{
	TCCR1A = TCCR1A & 0b11111100;
	TCCR1B = 0b11000010;
	TCCR1C = 0b00000000;
	TIMSK = 0b00100100;
	//ETIMSK = 0b00100100;
}

/// Sends out a single ping
void ping()
{
	TIMSK &= 0b11011111;
	DDRD |= 0x10;
	PORTD |= 0x10;
	wait_ms(1);
	PORTD &= 0xEF;
	TIFR |= 0b00100000;
	DDRD &= 0xEF;
	TIMSK |= 0b00100000;
	TCCR1B = 0b11000100;
	
	wait_ms(25);
}

/// Converts from time to distance
/*
 * Takes the input time and converts it to a distance value using the formula distance = velocity * time;
 * @param time a integer that indicates the desired time value
 */
void time_to_dist(int time)
{
	distance = (time * 0.000016)*17000;
	
}

/// Sends out a ping and calculates the distance between the object and the robot
void ping_read()
{
	ping();
	time = time_current-time_last;
	time_to_dist(time);
}

///SERVO CONTROL/////////////////////////////////////////////
/// Initializes the servo
void servo_init()
{
	TCCR3B = 0b00011010;
	TCCR3A = 0b10101011;
	OCR3A = 40000;
	OCR3B = 3000;
	
	DDRE = _BV(4);
}

/// Rotates the servo to a given angle
/**
 * Adjusts the value of OCR3B so the servo faces the given direction between 0 and 180 degrees
 * @param deg a int between 0 and 180 indicating the desired position
 */
void move_servo(int deg)
{
	if(deg <= 180 && deg >= 0)
	{
		OCR3B = CALIBRATE*deg + SERVO_BOT_VAL;
		//lprintf("Degree: %d\nDirection: %d\nRaw: %d",deg,dir,OCR3B);
		wait_ms(10);
	}
}

/////////////////////////////////////////////////////
/// Moves the robot the desired speed and distance
/*
 * Sets the wheels of the robot the desired speed for the distance given while constantly updating the sensor data to know when to stop the robot
 * @param sensor_data is a pointer that reads from the robots sensors and updates
 * @param speed a integer that indicates the desired speed of each wheel
 * @param distance_mm a integer that indicates the desired distance the robots needs to travel in millimeters 
 */
void move_forward(oi_t *sensor_data, int speed, int distance_mm) {
	
	int sum = 0;
	oi_set_wheels(speed, speed); // move forward; full speed
	int force_stop_status = 0;	
	
	while (sum < distance_mm) {
		oi_update(sensor_data);

		///BUMPER SENSORS///////////////////////////////////////////
		if(sensor_data->bumper_left) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rLeft Bumper Hit\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}	
		if(sensor_data->bumper_right) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rRight Bumper Hit\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		if(sensor_data->wheeldrop_caster == 1) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rFront Wheel Drop\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		///CLIFF SENSORS///////////////////////////////////////////
		if(sensor_data->cliff_frontleft_signal > FRONTLEFT_OB) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rFront Left OB\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		if(sensor_data->cliff_frontright_signal > FRONTRIGHT_OB) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rFront Right OB\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		if(sensor_data->cliff_left_signal > LEFTSIDE_OB) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rLeft Side OB\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		if(sensor_data->cliff_right_signal > RIGHTSIDE_OB) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rRight Side OB\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		///CRATER SENSORS/////////////////////////////////////////
		if(sensor_data->cliff_frontleft_signal <= 0) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rFrontLeft Crater\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		if(sensor_data->cliff_frontright_signal <= 0) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rFrontRight Crater\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		if(sensor_data->cliff_left_signal <= 0) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rLeft Side Crater\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		if(sensor_data->cliff_right_signal <= 0) {
			oi_set_wheels(0, 0);
			char warning[30] = "\n\rRight Side Crater\n\r";
			UART_transmit_str(warning);
			force_stop_status = 1;
			break;
		}
		///END IF STATEMENTS/////////////////////////////////////
		else sum += sensor_data->distance;
	}
	
	oi_set_wheels(0, 0); // stop
	
	if(force_stop_status == 1){
		int sum2 = 0;
		 oi_set_wheels(-50,-50);
		 while(sum2 > -50) {
			oi_update(sensor_data);	 
			sum2 += sensor_data->distance;
		 }
		oi_set_wheels(0,0);
		sum = sum + sum2;
	}
	
	char traveldist[20] = "";
	int cm_dist = sum/10;
	sprintf(traveldist, "\n\r%d\n\r", cm_dist);
	UART_transmit_str(traveldist);
	
}

/// Rotates the robots clock-wise the desired number of degrees
/*
 * Sets each wheel of the robot to equal and opposite speeds to rotate the robot the desired degrees while constantly updating the sensor data to know when to stop the robot 
 * @param sensor_data is a pointer that reads from the robots sensors and updates
 * @param degrees a integer that indicates the desired rotation angle
 */
void turn_cw(oi_t *sensor_data, int degrees) {
	
	int degrees_turned = 0;
	oi_set_wheels(-100,100);
	
	while(abs(degrees_turned) < degrees) {
		wait_ms(5);
		oi_update(sensor_data);
		degrees_turned += sensor_data->angle;
	}
	oi_set_wheels(0,0);
}

/// Rotates the robots counter clock-wise the desired number of degrees
/*
 * Sets each wheel of the robot to equal and opposite speeds to rotate the robot the desired degrees while constantly updating the sensor data to know when to stop the robot 
 * @param sensor_data is a pointer that reads from the robots sensors and updates
 * @param degrees a integer that indicates the desired rotation angle
 */
void turn_ccw(oi_t *sensor_data, int degrees) {
	int degrees_turned = 0;
	oi_set_wheels(100,-100);
	
	while(degrees_turned < degrees) {
		wait_ms(5);
		oi_update(sensor_data);
		degrees_turned += sensor_data->angle;
	}
	oi_set_wheels(0,0);
}

////////////////////////////////////////////////////

struct Object {
	double diameter;
	int angularSize;
	int firstDegrees;
	int finalDegrees;
	int objectDistance;
	};

/// Initializes the sweeping of the servo to print off the sensor data
/*
 * Displays the degrees, IR distance, and sonar distance column 
 * headers so the data received from the sensor can be displayed 
 * underneath their respected columns.
 */
void sensor_sweep_init() {
	
	char print1[] = "Degrees   ";
	char print2[] = "IR Distance (cm)   ";
	char print3[] = "Sonar Distance (cm)";
	
	UART_transmit_str(print1);
	UART_transmit_str(print2);
	UART_transmit_str(print3);
	
	UART_transmit_char('\n');
	UART_transmit_char('\r');
}

/// Sweeps the servo to collect data while running the ping sensor and IR sensor.
/*
 * Uses the move_servo function to move servo from 0 to 180 degrees to gather data from the sensors in order to 
 * know the amount of objects, size of objects, and the distance of the objects from the robot 
 */
void sensor_sweep() {
	
	int objectIndex = 0;
	int lastSeenDegrees;
	int startDegrees;
	int timesSeen = 0;
	int sonarDistance;
	struct Object objects[10];
	
	for(int deg = 0; deg <= 180; deg+=2) {
		
		move_servo(deg);
		double IR_dist = IR_smooth();
		ping_read();
		int Sonar_dist = distance;
		wait_ms(100);
		
		//if(IR_dist < 100)
		if ((abs(Sonar_dist - IR_dist) <= 30) && (Sonar_dist < 60) && (deg != 180))
		{
			if ((lastSeenDegrees + 2 == deg) && (deg != 178)) {
				lastSeenDegrees = deg;
				timesSeen++;
			} 
			else {
				timesSeen = 1;
				startDegrees = deg;
				lastSeenDegrees = deg;
				sonarDistance = Sonar_dist;	
			}
		}

		else if (timesSeen >= 1)
		{
			objects[objectIndex].firstDegrees = startDegrees;
			objects[objectIndex].finalDegrees = lastSeenDegrees;
			objects[objectIndex].angularSize = lastSeenDegrees - startDegrees + 2;
			objects[objectIndex].diameter = 2.0 * sonarDistance * tan((objects[objectIndex].angularSize/360.0) * PI);
			objects[objectIndex].objectDistance = sonarDistance;
			objectIndex++;
			timesSeen = 0;
		}
		char output[50] = "";
		sprintf(output, "%d       %4.2f               %4.2d\n\r", deg,IR_dist,Sonar_dist);
		UART_transmit_str(output);
	}
	
	for(int i = 0; i < objectIndex; i++)
	{
			char obj_ID[40] = "";
			sprintf(obj_ID, "width: %f, angle: %d, distance: %d\n\r", objects[i].diameter, objects[i].finalDegrees - objects[i].angularSize/2, objects[i].objectDistance);
			UART_transmit_str(obj_ID);
	}

}

///////////////////////////////////////////////////

/// Plays song of choice when function is called
void play_song() {
	int song_index = 0;
	int num_notes = 6; //Number of notes
	
	unsigned char notes[6] = {57,50,53,57,50,53};
	unsigned char duration[6] = {64,84,32,64,84,32};
	//AD_F AD_F (Song of Time - Zelda Ocarina of Time)
	
	oi_load_song(song_index, num_notes, notes, duration);
	oi_play_song(song_index);
}
