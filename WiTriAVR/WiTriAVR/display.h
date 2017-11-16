/*
*	display.h
*	header file for Adafruit 7seg backpack
*/

#define LED_BKPK			0xE0
#define LED_BKPK_ON			0x21
#define LED_BKPK_STBY		0x20
#define LED_BKPK_DISPLAYON	0x81
#define LED_BKPK_DISPLAYOFF	0x80
#define LED_BKPK_BLINKON	0x85
#define LED_BKPK_BLINKOFF	0x81
#define LED_BKPK_DIM		0xE0

void disp_init();
void disp_digit(uint8_t digit, uint8_t data);
void disp_blank_digit(uint8_t digit);
void disp_colon(uint8_t data);
void disp_set_digit(uint8_t digit, uint8_t data);

static const uint8_t numberTable[] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x6F, // 9
	0x77, // A
	0x7C, // b
	0x39, // C
	0x5E, // d
	0x79, // E
	0x71 // F
};

void SS_Init();

