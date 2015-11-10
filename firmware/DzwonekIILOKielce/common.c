#include "common.h"

#include <string.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

uint8_t bcdToUint8(uint8_t value)
{
	return value - 6 * (value >> 4);
}
uint8_t uint8ToBcd(uint8_t value)
{
	return value + 6 * (value / 10);
}
uint8_t computeParity(uint8_t value)
{
	uint8_t result = 0;
	for (uint8_t i = 0; i < 8; i++)
	{
		if (value & (1 << i)) result ^= 1;
	}

	return result;
}

void resetSystem()
{
	// prepare to reset
	// disable interrupts, set watchdog to minimum time and wait this time
	// mcu will reset
	// in main() function there should be wdt_reset() and wdt_disable() because watchdog is enabled
	// even after reset

	cli();
	wdt_enable(WDTO_15MS);
	while (1); // wait until mcu resets
}

uint8_t computeDayOfWeek(uint8_t day, uint8_t month, uint16_t year)
{
	// http://pl.wikipedia.org/wiki/Kalendarz_wieczny
	// Zeller algorithm, simplified by Mike Keith
	// ([23m/9] + d + 4 + y + [z/4] - [z/100] + [z/400] - c) mod 7

	uint16_t z, c;
	if (month < 3)
	{
		z = year - 1;
		c = 0;
	}
	else
	{
		z = year;
		c = 2;
	}

	uint16_t result = (((23 * month) / 9) + day + 4 + year + (z / 4) - (z / 100) + (z / 400) - c) % 7;

	if (result == 0) return 7;
	return (uint8_t)result;
}
uint8_t monthDays(uint8_t month, uint16_t year)
{
	if (month == 2)
	{
		return isLeapYear(year) ? 29 : 28;
	}
	else
	{
		if (month > 7) month--;

		return month % 2 == 0 ? 30 : 31;
	}
}

void monthSetDefault(month_t* result, uint8_t month, uint16_t year, uint8_t profile, uint8_t daysOfWeekMask)
{
	memset(result, 0, sizeof(month_t));
	uint8_t days = monthDays(month, year);
	uint8_t dWeek = computeDayOfWeek(1, month, year);
	for (uint8_t i = 1; i <= days; i++)
	{
		if (daysOfWeekMask & (1 << dWeek)) result->days[i] = profile; else result->days[i] = 0xFF;
		dWeek++;
		if (dWeek > SUNDAY) dWeek = MONDAY;
	}
}
