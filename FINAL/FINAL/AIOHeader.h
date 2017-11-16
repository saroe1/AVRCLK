#include "open_interface.h"
#define PI 3.14159
#define CALIBRATE 20.5 //bot 2
#define SERVO_BOT_VAL 600
#define FCLK 16000000
#define BTBAUD 34

//Cliff sensor calibrations, change for each robot
#define FRONTLEFT_OB 450 //
#define FRONTRIGHT_OB 550
#define RIGHTSIDE_OB 450 //
#define LEFTSIDE_OB 1000 //

extern volatile unsigned int timer2_tick;

extern int IR_arr[1024];

extern volatile unsigned long time_diff;
extern volatile int time;
extern volatile int time_last;
extern volatile int time_current;
extern volatile int distance;
extern volatile int overflows;
extern volatile int new_overflows;

void timer2_start(char unit);
void timer2_stop();
void wait_ms(unsigned int time_val);

void UART_init();
char UART_rec();
void UART_transmit_str(char data[]);
void UART_transmit_char(char character);

void IR_init();
unsigned int IR_read();
void IR_lookup_init();
double IR_smooth();

void ping_init();
void ping();
void time_to_dist();
void ping_read();

void servo_init();
void move_servo(int deg);

void move_forward(oi_t *sensor_data, int speed, int distance_mm);
void turn_cw(oi_t *sensor_data, int degrees);
void turn_ccw(oi_t *sensor_data, int degrees);

void sensor_sweep_init();
void sensor_sweep();

void play_song();
