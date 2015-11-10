#include "menuSetDate.h"

#include <stdio.h>
#include "../clock.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"

#define MENU_SETDATE_HIGHLIGHT_YEAR 0
#define MENU_SETDATE_HIGHLIGHT_MONTH 1
#define MENU_SETDATE_HIGHLIGHT_DAY 2
#define MENU_SETDATE_HIGHLIGHT_COUNT 3

static const dateTime_t* s_currentDateTime;
static uint8_t s_currentHighlight;

static uint8_t s_day;
static uint8_t s_month;
static uint16_t s_year;

#define menuSetDateNeedBlink() (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS)
#define menuSetDateWriteDay() { sprintf(menuBuffer, MENU_DAY_FORMAT, s_day); interfaceDisplayWrite(MENU_SETDATE_DAY_X, MENU_SETDATE_DAY_Y, menuBuffer); }
#define menuSetDateWriteMonth() { sprintf(menuBuffer, MENU_MONTH_FORMAT, s_month); interfaceDisplayWrite(MENU_SETDATE_MONTH_X, MENU_SETDATE_MONTH_Y, menuBuffer); }
#define menuSetDateWriteYear() { sprintf(menuBuffer, MENU_YEAR_FORMAT, s_year); interfaceDisplayWrite(MENU_SETDATE_YEAR_X, MENU_SETDATE_YEAR_Y, menuBuffer); }

void menuSetDateFixMaxDay();

void menuSetDate(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_currentDateTime = clockGetCurrentDateTimePointer();
		s_day = s_currentDateTime->day;
		s_month = s_currentDateTime->month;
		s_year = s_currentDateTime->year + 1900;
		s_currentHighlight = MENU_SETDATE_HIGHLIGHT_YEAR;
		interfaceMenuTimerReset(0);
	}

	interfaceDisplayWriteChar(MENU_SETDATE_YEAR_MONTH_SEPARATOR_X, MENU_SETDATE_YEAR_MONTH_SEPARATOR_Y, MENU_DATE_SEPARATOR);
	interfaceDisplayWriteChar(MENU_SETDATE_MONTH_DAY_SEPARATOR_X, MENU_SETDATE_MONTH_DAY_SEPARATOR_Y, MENU_DATE_SEPARATOR);

	stringsLoad(menuBuffer, stringsYearFormat);
	interfaceDisplayWrite(MENU_SETDATE_YEAR_FORMAT_X, MENU_SETDATE_YEAR_FORMAT_Y, menuBuffer);
	stringsLoad(menuBuffer, stringsMonthFormat);
	interfaceDisplayWrite(MENU_SETDATE_MONTH_FORMAT_X, MENU_SETDATE_MONTH_FORMAT_Y, menuBuffer);
	stringsLoad(menuBuffer, stringsDayFormat);
	interfaceDisplayWrite(MENU_SETDATE_DAY_FORMAT_X, MENU_SETDATE_DAY_FORMAT_Y, menuBuffer);

	interfaceDisplayWriteChar(MENU_SETDATE_YEAR_MONTH_FORMAT_SEPARATOR_X, MENU_SETDATE_YEAR_MONTH_FORMAT_SEPARATOR_Y, MENU_DATE_SEPARATOR);
	interfaceDisplayWriteChar(MENU_SETDATE_MONTH_DAY_FORMAT_SEPARATOR_X, MENU_SETDATE_MONTH_DAY_FORMAT_SEPARATOR_Y, MENU_DATE_SEPARATOR);

	menuSetDateWriteYear();
	menuSetDateWriteMonth();
	menuSetDateWriteDay();

	if (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS * 2)
	{
		interfaceMenuTimerReset(0);
	}

	switch (s_currentHighlight)
	{
	case MENU_SETDATE_HIGHLIGHT_YEAR:
		if (menuSetDateNeedBlink())
			interfaceDisplayWrite(MENU_SETDATE_YEAR_X, MENU_SETDATE_YEAR_Y, "    ");
		break;
	case MENU_SETDATE_HIGHLIGHT_MONTH:
		if (menuSetDateNeedBlink())
			interfaceDisplayWrite(MENU_SETDATE_MONTH_X, MENU_SETDATE_MONTH_Y, "  ");
		break;
	case MENU_SETDATE_HIGHLIGHT_DAY:
		if (menuSetDateNeedBlink())
			interfaceDisplayWrite(MENU_SETDATE_DAY_X, MENU_SETDATE_DAY_Y, "  ");
		break;
	}

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		interfaceButtonsLock();
		s_currentHighlight++;
		if (s_currentHighlight >= MENU_SETDATE_HIGHLIGHT_COUNT)
		{
			dateTime_t newDateTime = *s_currentDateTime;
			newDateTime.day = s_day;
			newDateTime.month = s_month;
			newDateTime.year = s_year - 1900;
			newDateTime.dayOfWeek = computeDayOfWeek(s_day, s_month, s_year);
			clockSetDateTime(&newDateTime);
			interfaceMenuPrevious();
		}
		else
		{
			menuSetDateWriteYear();
			menuSetDateWriteMonth();
			menuSetDateWriteDay();
		}
	}
	else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_UP))
	{
		switch (s_currentHighlight)
		{
		case MENU_SETDATE_HIGHLIGHT_YEAR: valueIncRange(s_year, MENU_YEAR_MIN, MENU_YEAR_MAX); menuSetDateFixMaxDay(); break;
		case MENU_SETDATE_HIGHLIGHT_MONTH: valueIncRange(s_month, MENU_MONTH_MIN, MENU_MONTH_MAX); menuSetDateFixMaxDay(); break;
		case MENU_SETDATE_HIGHLIGHT_DAY: valueIncRange(s_day, MENU_DAY_MIN, MENU_DAY_MAX(s_month, s_year)); break;
		}
	}
	else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_DOWN))
	{
		switch (s_currentHighlight)
		{
		case MENU_SETDATE_HIGHLIGHT_YEAR: valueDecRange(s_year, MENU_YEAR_MIN, MENU_YEAR_MAX); menuSetDateFixMaxDay(); break;
		case MENU_SETDATE_HIGHLIGHT_MONTH: valueDecRange(s_month, MENU_MONTH_MIN, MENU_MONTH_MAX); menuSetDateFixMaxDay(); break;
		case MENU_SETDATE_HIGHLIGHT_DAY: valueDecRange(s_day, MENU_DAY_MIN, MENU_DAY_MAX(s_month, s_year)); break;
		}
	}
}

void menuSetDateFixMaxDay()
{
	uint8_t maxDay = MENU_DAY_MAX(s_month, s_year);
	if (s_day > maxDay) s_day = maxDay;
	menuSetDateWriteDay();
}
