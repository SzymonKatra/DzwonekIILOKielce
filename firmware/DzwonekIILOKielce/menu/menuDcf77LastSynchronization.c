#include "menuDcf77LastSynchronization.h"

#include <stdio.h>
#include "../clock.h"
#include "../common.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"

static const dateTime_t* s_lastSynchronization;

void menuDcf77LastSynchronization(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_lastSynchronization = clockGetLastSynchronizationPointer();
	}

	if (s_lastSynchronization->year != 0) // 0 means never
	{
		sprintf(menuBuffer, MENU_HOUR_FORMAT MENU_TIME_SEPARATOR_STR MENU_MINUTE_FORMAT, s_lastSynchronization->hours, s_lastSynchronization->minutes);
		interfaceDisplayWrite(MENU_DCF77LASTSYNCHRONIZATION_TIME_X, MENU_DCF77LASTSYNCHRONIZATION_TIME_Y, menuBuffer);

		sprintf(menuBuffer, MENU_DAY_FORMAT MENU_DATE_SEPARATOR_STR MENU_MONTH_FORMAT MENU_DATE_SEPARATOR_STR MENU_YEAR_FORMAT, s_lastSynchronization->day, s_lastSynchronization->month, (uint16_t)s_lastSynchronization->year + 1900);
		interfaceDisplayWrite(MENU_DCF77LASTSYNCHRONIZATION_DATE_X, MENU_DCF77LASTSYNCHRONIZATION_DATE_Y, menuBuffer);

		interfaceDisplayWrite(MENU_DCF77LASTSYNCHRONIZATION_DAYOFWEEK_X, MENU_DCF77LASTSYNCHRONIZATION_DAYOFWEEK_Y, menuGetDayOfWeekName(s_lastSynchronization->dayOfWeek));
	}
	else
	{
		stringsLoad(stringsBuffer, stringsNever);
		interfaceDisplayWrite(MENU_DCF77LASTSYNCHRONIZATION_NEVER_X, MENU_DCF77LASTSYNCHRONIZATION_NEVER_Y, stringsBuffer);
	}

	if (interfaceHandleAnyButtonPressed()) interfaceMenuPrevious();
}
