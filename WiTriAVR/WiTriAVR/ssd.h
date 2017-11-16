#define HT16K33 0xE0 			// I2C bus address for Ht16K33 backpack
#define HT16K33_ON 0x21 		// turn device oscillator on
#define HT16K33_STANDBY 0x20 	// turn device oscillator off
#define HT16K33_DISPLAYON 0x81 	// turn on output pins
#define HT16K33_DISPLAYOFF 0x80 // turn off output pins
#define HT16K33_BLINKON 0x85 	// blink rate 1 Hz (-2 for 2 Hz)
#define HT16K33_BLINKOFF 0x81 	// same as display on
#define HT16K33_DIM 0xE0 		// add level (15=max) to byte

#ifndef F_CPU
	#define F_CPU 16000000U
#endif

static const byte numberTable[] = // convert number to lit-segments
{
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
	0x71, // F
	0x00, //<blank>
};

void ssd_init();
void ssd_setDigRaw(byte digit, byte data);
void ssd_blankDig(byte digit);
void ssd_setDig(byte digit, byte data);
void ssd_setColon(byte data);
void ssd_setDigits(byte d0, byte d1, byte d2, byte d3, byte colon);
void ssd_int(int data, byte base);
void ssd_intTest();
void ssd_circle();
void ssd_time(byte hour, byte min, byte sec, byte dayOW);
void msDelay(int delay);