#ifndef DS3231_H_
#define DS3231_H_

#include <stdint.h>
#include "../common.h"

#define DS3231_ADDRESS 0xD0

#define DS3231_SECONDS 0x00
#define DS3231_TEMP_MSB 0x11

void ds3231ReadDateTime(dateTime_t* result);
void ds3231WriteDateTime(const dateTime_t* time);

int8_t ds3231ReadTemperature();

#endif /* DS3231_H_ */
