#include "ds3231.h"

#include "i2c.h"

#define DS3231_READ 1
#define DS3231_WRITE 0

void ds3231ReadDateTime(dateTime_t* result)
{
	uint8_t tmp;

	i2cStart(DS3231_ADDRESS | DS3231_WRITE);
	i2cWrite(DS3231_SECONDS);
	i2cStart(DS3231_ADDRESS | DS3231_READ);
	// following data is read in BCD format
	result->seconds = bcdToUint8(i2cReadAck());
	result->minutes = bcdToUint8(i2cReadAck());
	tmp = i2cReadAck();
	if (tmp & 0b01000000) // 12 hour mode
	{
		result->hours = bcdToUint8(tmp & 0b00011111);
		if(tmp & 0b00100000) result->hours += 12; // PM
	}
	else result->hours = bcdToUint8(tmp & 0b00111111);
	result->dayOfWeek = bcdToUint8(i2cReadAck());
	result->day = bcdToUint8(i2cReadAck());
	tmp = i2cReadAck();
	result->month = bcdToUint8(tmp & 0b00011111);
	result->year = bcdToUint8(i2cReadNack());
	if (tmp & 0b10000000) result->year += 100;

	i2cStop();
}
void ds3231WriteDateTime(const dateTime_t* time)
{
	i2cStart(DS3231_ADDRESS | DS3231_WRITE);
	i2cWrite(DS3231_SECONDS);
	i2cWrite(uint8ToBcd(time->seconds));
	i2cWrite(uint8ToBcd(time->minutes));
	i2cWrite(uint8ToBcd(time->hours)); // writes in 24-hour format
	i2cWrite(uint8ToBcd(time->dayOfWeek));
	i2cWrite(uint8ToBcd(time->day));
	i2cWrite(uint8ToBcd(time->month) | (time->year >= 100 ? 0b10000000 : 0));
	i2cWrite(uint8ToBcd(time->year - (time->year >= 100 ? 100 : 0)));
	i2cStop();
}

int8_t ds3231ReadTemperature()
{
	i2cStart(DS3231_ADDRESS | DS3231_WRITE);
	i2cWrite(DS3231_TEMP_MSB);
	i2cStart(DS3231_ADDRESS | DS3231_READ);
	uint8_t result = i2cReadNack();
	i2cStop();

	return result;
}
