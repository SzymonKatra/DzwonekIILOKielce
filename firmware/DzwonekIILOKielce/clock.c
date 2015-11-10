#include "clock.h"

#include <util/atomic.h>
#include "controller.h"
#include "device/dcf77.h"
#include "device/ds3231.h"

static const settings_t* s_currentSettings;

static dateTime_t s_currentDateTime;
static uint8_t s_dateTimeUpdateCounter;

static volatile uint8_t s_dcf77DataReceived;
static volatile uint64_t s_dcf77Data;
static volatile uint8_t s_dcf77StartDelay;
static volatile uint8_t s_dcf77Failed;
static uint8_t s_dcf77SynchronizedToday;
static dateTime_t s_dcf77LastSynchronization;

static uint8_t s_previousDay;
static uint8_t s_previousMonth;
static uint8_t s_previousYear;
static uint16_t s_previousMinutes;
static uint8_t s_dayChanged;
static uint8_t s_timeTransitionState;

static void clockDcf77ReceivedCallback(uint64_t data);

void clockInit()
{
	s_dcf77DataReceived = 0;
	s_dcf77Data = 0;
	s_dcf77StartDelay = 0;
	s_dcf77Failed = 0;
	s_dcf77SynchronizedToday = 0;
	s_dcf77LastSynchronization.year = 0;

	s_previousDay = 0;
	s_previousMonth = 0;
	s_previousYear = 0;
	s_dayChanged = 1;
	s_timeTransitionState = CLOCK_TIME_TRANSITION_NO_ACTION;

	s_dateTimeUpdateCounter = CLOCK_UPDATE_TICKS;
	ds3231ReadDateTime(&s_currentDateTime);

	s_currentSettings = controllerSettingsGetPointer();

	dcf77RegisterReceivedCallback(&clockDcf77ReceivedCallback);
}

void clockTick()
{
	s_dateTimeUpdateCounter++;

	if (s_dcf77StartDelay > 0)
	{
		s_dcf77StartDelay--;

		if (s_dcf77StartDelay == 0 && !dcf77IsEnabled()) dcf77Start();
	}
}
void clockProcess()
{
	if (s_dcf77DataReceived)
	{
		s_dcf77DataReceived = 0;

		uint64_t dataTmp;
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			dataTmp = s_dcf77Data;
		}

		dcf77Data_t result;

		if (dcf77Decode(dataTmp, &result))
		{
			dcf77Stop();
			ds3231WriteDateTime(&result.currentDateTime);
			s_dcf77LastSynchronization = result.currentDateTime;
			s_dcf77SynchronizedToday = 1;
			s_dayChanged = 1;
		}
	}

	if (dcf77IsEnabled() && dcf77GetFailCount() >= CLOCK_DCF77_FAILS_TO_RESET)
	{
		dcf77Stop();
		s_dcf77StartDelay = CLOCK_DCF77_RESET_DELAY;
	}

	if (s_dateTimeUpdateCounter >= CLOCK_UPDATE_TICKS)
	{
		s_dateTimeUpdateCounter = 0;
		s_previousDay = s_currentDateTime.day;
		s_previousMonth = s_currentDateTime.month;
		s_previousYear = s_currentDateTime.year;

		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			ds3231ReadDateTime(&s_currentDateTime);
		}

		if (s_currentDateTime.day != s_previousDay || s_currentDateTime.month != s_previousMonth || s_currentDateTime.year != s_previousYear)
		{
			s_dayChanged = 1;
		}
	}

	if (s_dayChanged)
	{
		s_dayChanged = 0;

		if (s_currentSettings->autoTimeTransition)
		{
			// source: https://pl.wikipedia.org/wiki/Czas_letni
			// CET - Central European Time (UTC + 1)
			// CEST - Central European Summer Time (UTC + 2)
			// across the UE time transitions are standarized
			// CET to CEST - last sunday of march
			// CEST to CET - last sunday of october
			// following code computes if there is need to change time today

			s_timeTransitionState = CLOCK_TIME_TRANSITION_NO_ACTION;

			if (s_currentDateTime.month == 3 && s_currentDateTime.hours < 2)
			{
				// march has 31 days, check if now is last sunday of march
				if (s_currentDateTime.dayOfWeek == SUNDAY && s_currentDateTime.day + 7 > 31) s_timeTransitionState = CLOCK_TIME_TRANSITION_CET_TO_CEST;
			}
			else if (s_currentDateTime.month == 10 && s_currentDateTime.hours < 2)
			{
				// october has 31 days, check if now is last sunday of october
				if (s_currentDateTime.dayOfWeek == SUNDAY && s_currentDateTime.day + 7 > 31) s_timeTransitionState = CLOCK_TIME_TRANSITION_CEST_TO_CET;
			}
		}
	}

	if (s_currentSettings->autoTimeTransition && s_timeTransitionState != CLOCK_TIME_TRANSITION_NO_ACTION)
	{
		if (s_timeTransitionState == CLOCK_TIME_TRANSITION_CET_TO_CEST)
		{
			if (s_currentDateTime.hours == 2)
			{
				s_currentDateTime.hours++;
				ds3231WriteDateTime(&s_currentDateTime);
				s_timeTransitionState = CLOCK_TIME_TRANSITION_NO_ACTION;
			}
		}
		else if (s_timeTransitionState == CLOCK_TIME_TRANSITION_CEST_TO_CET)
		{
			if (s_currentDateTime.hours == 3)
			{
				s_currentDateTime.hours--;
				ds3231WriteDateTime(&s_currentDateTime);
				s_timeTransitionState = CLOCK_TIME_TRANSITION_NO_ACTION;
			}
		}
	}

	uint16_t currentMinutes = (s_currentDateTime.hours * 60) + s_currentDateTime.minutes;
	uint16_t minSyncTime = s_currentSettings->dcf77SynchronizationTime;
	uint16_t maxSyncTime = minSyncTime + s_currentSettings->dcf77SynchronizationMaxLength;
	uint8_t canSynchronize = 0;

	if (minSyncTime != 0)
	{
		if (s_previousMinutes < minSyncTime && currentMinutes >= minSyncTime) s_dcf77SynchronizedToday = 0;
	}
	else
	{
		if (s_previousMinutes == 23 * 60 + 59 && currentMinutes == 0) s_dcf77SynchronizedToday = 0;
	}

	s_previousMinutes = currentMinutes;

	if (maxSyncTime < 1440)
	{
		if (currentMinutes >= minSyncTime && currentMinutes < maxSyncTime) canSynchronize = 1;
	}
	else
	{
		maxSyncTime -= 1440;
		if (currentMinutes >= minSyncTime || currentMinutes < maxSyncTime) canSynchronize = 1;
	}

	if (!s_dcf77SynchronizedToday && canSynchronize)
	{
		if (!dcf77IsEnabled() && s_dcf77StartDelay == 0)
		{
			dcf77Start();
		}
	}
	else
	{
		if (dcf77IsEnabled()) dcf77Stop(); else s_dcf77StartDelay = 0; // put 0 into start delay to be sure that DCF77 module wouldn't start
	}
}

const dateTime_t* clockGetCurrentDateTimePointer()
{
	return &s_currentDateTime;
}

void clockSetDateTime(const dateTime_t* dateTime)
{
	s_dayChanged = 1;
	s_dcf77SynchronizedToday = 0;
	s_timeTransitionState = CLOCK_TIME_TRANSITION_NO_ACTION;
	s_currentDateTime = *dateTime;
	ds3231WriteDateTime(dateTime);
}
uint8_t clockGetTimeTransitionState()
{
	return s_timeTransitionState;
}
const dateTime_t* clockGetLastSynchronizationPointer()
{
	return &s_dcf77LastSynchronization;
}

static void clockDcf77ReceivedCallback(uint64_t data)
{
	s_dcf77DataReceived = 1;
	s_dcf77Data = data;
}
