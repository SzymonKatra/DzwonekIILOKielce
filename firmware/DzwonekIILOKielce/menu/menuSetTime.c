#include "menuSetTime.h"

#include <stdio.h>
#include "../clock.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"

#define MENU_SETTIME_HIGHLIGHT_HOUR 0
#define MENU_SETTIME_HIGHLIGHT_MINUTE 1
#define MENU_SETTIME_HIGHLIGH_COUNT 2

#define menuSetTimeNeedBlink() (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS)
#define menuSetTimeWriteHour() { sprintf(menuBuffer, MENU_HOUR_FORMAT, s_hour); interfaceDisplayWrite(MENU_SETTIME_HOUR_X, MENU_SETTIME_HOUR_Y, menuBuffer); }
#define menuSetTimeWriteMinute() { sprintf(menuBuffer, MENU_MINUTE_FORMAT, s_minute); interfaceDisplayWrite(MENU_SETTIME_MINUTE_X, MENU_SETTIME_MINUTE_Y, menuBuffer); }

static const dateTime_t* s_currentDateTime;
static uint8_t s_currentHighlight;

static uint8_t s_hour;
static uint8_t s_minute;

void menuSetTime(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_currentDateTime = clockGetCurrentDateTimePointer();
		s_hour = s_currentDateTime->hours;
		s_minute = s_currentDateTime->minutes;
		s_currentHighlight = MENU_SETTIME_HIGHLIGHT_HOUR;
		interfaceMenuTimerReset(0);
	}

	interfaceDisplayWriteChar(MENU_SETTIME_HOUR_MINUTE_SEPARATOR_X, MENU_SETTIME_HOUR_MINUTE_SEPARATOR_Y, MENU_TIME_SEPARATOR);

	stringsLoad(menuBuffer, stringsHourFormat);
	interfaceDisplayWrite(MENU_SETTIME_HOUR_FORMAT_X, MENU_SETTIME_HOUR_FORMAT_Y, menuBuffer);
	stringsLoad(menuBuffer, stringsMinuteFormat);
	interfaceDisplayWrite(MENU_SETTIME_MINUTE_FORMAT_X, MENU_SETTIME_MINUTE_FORMAT_Y, menuBuffer);

	interfaceDisplayWriteChar(MENU_SETTIME_HOUR_MINUTE_FORMAT_SEPARATOR_X, MENU_SETTIME_HOUR_MINUTE_FORMAT_SEPARATOR_Y, MENU_TIME_SEPARATOR);

	menuSetTimeWriteHour();
	menuSetTimeWriteMinute();

	if (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS * 2)
	{
		interfaceMenuTimerReset(0);
	}

	switch (s_currentHighlight)
	{
	case MENU_SETTIME_HIGHLIGHT_HOUR:
		if (menuSetTimeNeedBlink())
			interfaceDisplayWrite(MENU_SETTIME_HOUR_X, MENU_SETTIME_HOUR_Y, "  ");
		break;

	case MENU_SETTIME_HIGHLIGHT_MINUTE:
		if (menuSetTimeNeedBlink())
			interfaceDisplayWrite(MENU_SETTIME_MINUTE_X, MENU_SETTIME_MINUTE_Y, "  ");
		break;
	}

	if (interfaceHandleButtonPulse(INTERFACE_BUTTON_UP))
	{
		switch (s_currentHighlight)
		{
		case MENU_SETTIME_HIGHLIGHT_HOUR: valueIncRange(s_hour, MENU_HOUR_MIN, MENU_HOUR_MAX); break;
		case MENU_SETTIME_HIGHLIGHT_MINUTE: valueIncRange(s_minute, MENU_MINUTE_MIN, MENU_MINUTE_MAX); break;
		}
	}
	else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_DOWN))
	{
		switch (s_currentHighlight)
		{
		case MENU_SETTIME_HIGHLIGHT_HOUR: valueDecRange(s_hour, MENU_HOUR_MIN, MENU_HOUR_MAX); break;
		case MENU_SETTIME_HIGHLIGHT_MINUTE: valueDecRange(s_minute, MENU_MINUTE_MIN, MENU_MINUTE_MAX); break;
		}
	}

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		interfaceButtonsLock();
		s_currentHighlight++;
		if (s_currentHighlight >= MENU_SETTIME_HIGHLIGH_COUNT)
		{
			dateTime_t newDateTime = *s_currentDateTime;
			newDateTime.hours = s_hour;
			newDateTime.minutes = s_minute;
			newDateTime.seconds = 0;
			clockSetDateTime(&newDateTime);
			interfaceMenuPrevious();
		}
		else
		{
			menuSetTimeWriteHour();
			menuSetTimeWriteMinute();
		}
	}
}

