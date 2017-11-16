#define RTC_WRITE 0xD0 // I2C bus address of DS1307 RTC
#define RTC_READ 0xD1
#define SEC_REG 0x00
#define MIN_REG 0x01
#define HOURS_REG 0x02
#define DAYOFWK_REG 0x03
#define DAYS_REG 0x04
#define MONTHS_REG 0x05
#define YEARS_REG 0x06
#define CONTROL_REG 0x07
#define RAM_BEGIN 0x08
#define RAM_END 0x3F

typedef uint8_t byte;

void rtc_init();
void rtc_getTime(byte *hour, byte *min, byte *sec, byte *dayOW);
void rtc_getDate(byte *months, byte *days, byte *years);
void rtc_set(byte month, byte day, byte year, byte hour, byte min, byte sec);
