#ifndef F_SCL
	#define F_SCL 50000U
#endif

#define READ 1
#define TW_START 0xA4
#define TW_STOP 0x94
#define TW_ACK 0xC4
#define TW_NACK 0x84
#define TW_SEND 0x84
#define TW_READY (TWCR & 0x80)
#define TW_STATUS (TWSR & 0xF8)

typedef uint8_t byte;

void i2c_init();
void i2c_start(byte slaveAddr);
void i2c_writeByte(byte busAddr, byte data);
void i2c_writeReg(byte busAddr, byte deviceReg, byte data);
byte i2c_detect(byte addr);
byte i2c_findDevice(byte start);
byte i2c_write(byte data);
byte i2c_readACK();
byte i2c_readNACK();
byte i2c_readReg(byte busAddr, byte deviceReg);
