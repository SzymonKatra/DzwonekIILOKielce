#include "controller.h"

#include <util/atomic.h>
#include "clock.h"
#include "internalStorage.h"

#define bellInit() CONTROLLER_BELL_DDR |= (1 << CONTROLLER_BELL_BIT)
#define bellOn() CONTROLLER_BELL_PORT |= (1 << CONTROLLER_BELL_BIT)
#define bellOff() CONTROLLER_BELL_PORT &= ~(1 << CONTROLLER_BELL_BIT)

static const dateTime_t* s_currentDateTime;
static volatile uint16_t s_previousMinutes;

static settings_t s_currentSettings;

static uint8_t s_currentProfile;
static profile_t s_currentProfileData;
static volatile uint8_t s_reloadProfile;

static uint8_t s_currentMonth;
static month_t s_currentMonthData;
static volatile uint8_t s_reloadMonth;

static volatile uint8_t s_nextBell;
static volatile uint16_t s_bellRemainingTicks;

static void controllerForceReloadMonth();
static void controllerForceReloadProfile();

void controllerInit()
{
	bellInit();
	bellOff();

	internalStorageSettingsRead(&s_currentSettings);

	s_currentDateTime = clockGetCurrentDateTimePointer();
	s_previousMinutes = (s_currentDateTime->hours * 60) + s_currentDateTime->minutes;
	controllerForceReloadMonth();
	controllerForceReloadProfile();

	s_reloadMonth = 0;
	s_reloadProfile = 0;

	s_bellRemainingTicks = 0;
}

void controllerTick()
{
	if (s_bellRemainingTicks > 0)
	{
		s_bellRemainingTicks--;
		if (s_bellRemainingTicks == 0) bellOff();
	}

	if (s_reloadMonth) return;

	if (s_currentDateTime->month - 1 != s_currentMonth)
	{
		s_reloadMonth = 1;
		s_reloadProfile = 1;
	}

	uint16_t currentMinutes = (s_currentDateTime->hours * 60) + s_currentDateTime->minutes;

	if (s_currentMonthData.days[s_currentDateTime->day] != s_currentProfile) s_reloadProfile = 1;
	if (currentMinutes != s_previousMinutes) // time change detected
	{
		int16_t delta = currentMinutes - s_previousMinutes;
		if (delta < 0 || delta > 1) s_reloadProfile = 1; // user changed time
		s_previousMinutes = currentMinutes;
	}

	if (s_reloadProfile) return;

	if (s_nextBell < s_currentProfileData.count)
	{
		if (currentMinutes >= s_currentProfileData.bells[s_nextBell])
		{
			uint8_t bellLength = (s_currentProfileData.bellTypes & ((uint64_t)1 << s_nextBell)) ? s_currentSettings.bellLengthForBreak : s_currentSettings.bellLengthForLesson;
			s_bellRemainingTicks = bellLength * TICKS_PER_SECOND;
			bellOn();
			s_nextBell++;
		}
	}
}
void controllerProcess()
{
	if (s_reloadMonth)
	{
		controllerForceReloadMonth();
		s_reloadMonth = 0;
	}

	if (s_reloadProfile)
	{
		controllerForceReloadProfile();
		s_reloadProfile = 0;
	}
}

void controllerSettingsApply(const settings_t* settings)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		s_currentSettings = *settings;
	}
	internalStorageSettingsWrite(&s_currentSettings);
}
const settings_t* controllerSettingsGetPointer()
{
	return &s_currentSettings;
}

uint8_t controllerProfileGetCurrent()
{
	return s_currentProfile;
}
void controllerProfileUpdate(uint8_t profile, const profile_t* data)
{
	internalStorageProfileWrite(profile, data);
	if (profile == s_currentProfile) s_reloadProfile = 1;
}

uint8_t controllerMonthGetCurrent()
{
	return s_currentMonth;
}
void controllerMonthUpdate(uint8_t month, const month_t* data)
{
	internalStorageMonthWrite(month, data);
	if (month == s_currentMonth)
	{
		s_reloadMonth = 1;
		s_reloadProfile = 1;
	}
}

void controllerMarkProfileReload()
{
	s_reloadProfile = 1;
}
void controllerMarkMonthReload()
{
	s_reloadMonth = 1;
	s_reloadProfile = 1;
}

uint8_t controllerGetNextBell()
{
	if (s_nextBell >= s_currentProfileData.count) return 0xFF;
	return s_nextBell;
}
uint16_t controllerGetBellRemainingTicks()
{
	return s_bellRemainingTicks;
}

static void controllerForceReloadMonth()
{
	s_currentMonth = s_currentDateTime->month - 1;

	internalStorageMonthRead(s_currentMonth, &s_currentMonthData);
}
static void controllerForceReloadProfile()
{
	s_currentProfile = s_currentMonthData.days[s_currentDateTime->day];

	internalStorageProfileRead(s_currentProfile, &s_currentProfileData);

	s_nextBell = s_currentProfileData.count; // out of range - no bell
	uint16_t currentMinutes = (s_currentDateTime->hours * 60) + s_currentDateTime->minutes;
	for (int8_t i = s_currentProfileData.count - 1; i >= 0; i--)
	{
		if (s_currentProfileData.bells[i] > currentMinutes)
		{
			s_nextBell = (uint8_t)i;
		}
		else break;
	}
}
