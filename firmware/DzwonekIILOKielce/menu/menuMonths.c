#include "menuMonths.h"

#include <stdio.h>
#include "../clock.h"
#include "../controller.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"
#include "menuEditMonth.h"
#include "menuYesNo.h"

#define MENU_MONTHS_SEPTEMBER 0
#define MENU_MONTHS_OCTOBER 1
#define MENU_MONTHS_NOVEMBER 2
#define MENU_MONTHS_DECEMBER 3
#define MENU_MONTHS_JANUARY 4
#define MENU_MONTHS_FEBUARY 5
#define MENU_MONTHS_MARCH 6
#define MENU_MONTHS_APRIL 7
#define MENU_MONTHS_MAY 8
#define MENU_MONTHS_JUNE 9
#define MENU_MONTHS_JULY 10
#define MENU_MONTHS_AUGUST 11
#define MENU_MONTHS_NEW_CALENDAR 12
#define MENU_MONTHS_RETURN 13
#define MENU_MONTHS_COUNT 14

#define MENU_MONTHS_MONTH_OPTION_MIN 0
#define MENU_MONTHS_MONTH_OPTION_MAX 11

#define MENU_MONTHS_RETURN_FROM_NEW_CALENDAR_CONFIRMATION 1

#define menuMonthsGetSelectedMonth() ((menuGetCurrentOption(&s_menu) + 8) % 12)

static menu_t s_menu;
static uint16_t s_schoolYear;
static uint8_t s_returnFrom;

static const char* menuMonthsGetString(uint8_t option, uint8_t* textWidth);

void menuMonths(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		menuCreate(&s_menu, MENU_MONTHS_COUNT, &menuMonthsGetString, INTERFACE_MENU_LAST_TIMER);
		s_schoolYear = (uint16_t)controllerSettingsGetPointer()->schoolYear + 1900;
		s_returnFrom = 0;
	}

	if (flags & INTERFACE_MENU_RETURNED)
	{
		switch (s_returnFrom)
		{
		case MENU_MONTHS_RETURN_FROM_NEW_CALENDAR_CONFIRMATION:
			if (menuYesNoGet() == MENU_YESNO_YES)
			{
				stringsLoad(stringsBuffer, stringsWait);

				interfaceDisplayWrite(0, 0, stringsBuffer);
				interfaceDisplayWriteChar(15, 0, CUSTOMCHARS_HOURGLASS_ANSI);

				interfaceDisplaySwapBuffers();

				settings_t settings = *controllerSettingsGetPointer();
				settings.schoolYear = clockGetCurrentDateTimePointer()->year;
				controllerSettingsApply(&settings);

				s_schoolYear = (uint16_t)settings.schoolYear + 1900;

				month_t month;
				for (uint8_t i = 0; i < 12; i++)
				{
					// 7 is august if you count from 0
					monthSetDefault(&month, i + 1, (i <= 7 ? s_schoolYear + 1 : s_schoolYear), 0, 0b00111110);
					controllerMonthUpdate(i, &month);
				}
			}
			break;
		}

		s_returnFrom = 0;
	}

	menuWrite(&s_menu);

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		switch (menuGetCurrentOption(&s_menu))
		{
		case MENU_MONTHS_NEW_CALENDAR:
			s_returnFrom = MENU_MONTHS_RETURN_FROM_NEW_CALENDAR_CONFIRMATION;
			menuYesNoSet(MENU_YESNO_YES, stringsDoYouWantNewCalendar);
			interfaceMenuOpen(&menuYesNo);
			break;
		case MENU_MONTHS_RETURN: interfaceMenuPrevious(); break;
		default:
			menuEditMonthSet(menuMonthsGetSelectedMonth());
			interfaceMenuOpen(&menuEditMonth);
			break;
		}
	}
	else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_UP))
	{
		menuPreviousOption(&s_menu);
	}
	else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_DOWN))
	{
		menuNextOption(&s_menu);
	}
}

static const char* menuMonthsGetString(uint8_t option, uint8_t* textWidth)
{
	switch(option)
	{
	case MENU_MONTHS_SEPTEMBER: stringsLoad(stringsBuffer, stringsSeptember); break;
	case MENU_MONTHS_OCTOBER: stringsLoad(stringsBuffer, stringsOctober); break;
	case MENU_MONTHS_NOVEMBER: stringsLoad(stringsBuffer, stringsNovember); break;
	case MENU_MONTHS_DECEMBER: stringsLoad(stringsBuffer, stringsDecember); break;
	case MENU_MONTHS_JANUARY: stringsLoad(stringsBuffer, stringsJanuary); break;
	case MENU_MONTHS_FEBUARY: stringsLoad(stringsBuffer, stringsFebuary); break;
	case MENU_MONTHS_MARCH: stringsLoad(stringsBuffer, stringsMarch); break;
	case MENU_MONTHS_APRIL: stringsLoad(stringsBuffer, stringsApril); break;
	case MENU_MONTHS_MAY: stringsLoad(stringsBuffer, stringsMay); break;
	case MENU_MONTHS_JUNE: stringsLoad(stringsBuffer, stringsJune); break;
	case MENU_MONTHS_JULY: stringsLoad(stringsBuffer, stringsJuly); break;
	case MENU_MONTHS_AUGUST: stringsLoad(stringsBuffer, stringsAugust); break;

	case MENU_MONTHS_NEW_CALENDAR: stringsLoad(stringsBuffer, stringsNewCalendar); break;
	case MENU_MONTHS_RETURN: stringsLoad(stringsBuffer, stringsReturn); break;
	}

	if (option >= MENU_MONTHS_NEW_CALENDAR) return stringsBuffer;

	uint16_t year = s_schoolYear;
	if (option >= MENU_MONTHS_JANUARY && option <= MENU_MONTHS_AUGUST) year++;
	sprintf(menuBuffer, MENU_MONTHS_MONTH_TEMPLATE, stringsBuffer, year);

	return menuBuffer;
}
