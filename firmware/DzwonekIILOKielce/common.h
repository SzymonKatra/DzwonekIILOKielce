#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#define TICKS_PER_SECOND 50

#define MONDAY 1
#define TUESDAY 2
#define WEDNESDAY 3
#define THURSDAY 4
#define FRIDAY 5
#define SATURDAY 6
#define SUNDAY 7

#define MONDAY_MASK (1 << MONDAY)
#define TUESDAY_MASK (1 << TUESDAY)
#define WEDNESDAY_MASK (1 << WEDNESDAY)
#define THURSDAY_MASK (1 << THURSDAY)
#define FRIDAY_MASK (1 << FRIDAY)
#define SATURDAY_MASK (1 << SATURDAY)
#define SUNDAY_MASK (1 << SUNDAY)

#define valueIncRange(value, min, max) { if ((value) >= (max)) (value) = (min); else (value)++; }
#define valueDecRange(value, min, max) { if ((value) <= (min)) (value) = (max); else (value)--; }
#define valueClampRange(value, min, max) { if ((value) > (max)) (value) = (max); else if ((value) < (min)) (value) = (min); }
#define valueIncRangeMore(value, inc, min, max) { (value) += (inc); if ((value) > (max)) (value) = (min); }
#define valueDecRangeMore(value, dec, min, max) { (value) -= (dec); if ((value) < (min)) (value) = (max); }
#define isLeapYear(year) (((year) % 4 == 0 && (year) % 100 != 0) || (year) % 400 == 0)

#define PROFILE_COUNT 4
#define PROFILE_BELLS_COUNT 40

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t dayOfWeek;
	uint8_t day;
	uint8_t month;
	uint8_t year; // valid year = year + 1900
} dateTime_t;

typedef struct
{
	uint16_t checksum;
} metadata_t;
typedef struct
{
	uint8_t bellLengthForLesson;
	uint8_t bellLengthForBreak;
	uint8_t schoolYear; // valid year = schoolYear + 1900
	uint8_t autoTimeTransition;
	uint16_t dcf77SynchronizationTime; // hour * 60 + minute, set to 0xFFFF to disable
	uint16_t dcf77SynchronizationMaxLength; // in minutes
} settings_t;
typedef struct
{
	uint8_t count;
	uint16_t bells[PROFILE_BELLS_COUNT]; // hour * 60 + minute
	uint64_t bellTypes; // bit mask, 0 - for lesson, 1 - for break
} profile_t;
typedef struct
{
	uint8_t days[32];
} month_t;

uint8_t bcdToUint8(uint8_t value);
uint8_t uint8ToBcd(uint8_t value);
uint8_t computeParity(uint8_t value);

void resetSystem();

uint8_t computeDayOfWeek(uint8_t day, uint8_t month, uint16_t year);
uint8_t monthDays(uint8_t month, uint16_t year);

void monthSetDefault(month_t* result, uint8_t month, uint16_t year, uint8_t profile, uint8_t daysOfWeekMask);

#endif /* COMMON_H_ */
