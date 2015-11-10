#include "menuChangeProfile.h"

#include <stdio.h>
#include "../clock.h"
#include "../controller.h"
#include "../interface.h"
#include "../internalStorage.h"
#include "../strings.h"
#include "menu.h"
#include "menuChooseProfile.h"

#define MENU_CHANGEPROFILE_TODAY 0
#define MENU_CHANGEPROFILE_TOMMOROW 1
#define MENU_CHANGEPROFILE_RETURN 2
#define MENU_CHANGEPROFILE_COUNT 3

#define MENU_CHANGEPROFILE_RETURN_TODAY 1
#define MENU_CHANGEPROFILE_RETURN_TOMMOROW 2

typedef struct
{
	uint8_t day;
	uint8_t dayOfWeek;
	uint8_t month;
	uint8_t profile;
}menuChangeProfileData_t;

static menu_t s_menu;
static uint8_t s_returnFrom;
static menuChangeProfileData_t s_today;
static menuChangeProfileData_t s_tommorow;

static const char* menuChangeProfileGetString(uint8_t option, uint8_t* textWidth);

static void menuChangeProfileApplyProfile(menuChangeProfileData_t* data, uint8_t profile);

void menuChangeProfile(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_returnFrom = 0;

		menuCreate(&s_menu, MENU_CHANGEPROFILE_COUNT, &menuChangeProfileGetString, INTERFACE_MENU_LAST_TIMER);

		const dateTime_t* currentDateTime = clockGetCurrentDateTimePointer();
		month_t month;

		s_today.day = currentDateTime->day;
		s_today.dayOfWeek = currentDateTime->dayOfWeek;
		s_today.month = currentDateTime->month;
		internalStorageMonthRead(s_today.month - 1, &month);
		s_today.profile = month.days[s_today.day];

		s_tommorow.day = currentDateTime->day + 1;
		s_tommorow.dayOfWeek = currentDateTime->dayOfWeek + 1;
		s_tommorow.month = currentDateTime->month;
		if (s_tommorow.day >= monthDays(s_tommorow.month, currentDateTime->year))
		{
			s_tommorow.day = 1;
			s_tommorow.month++;
			if (s_tommorow.month > 12) s_tommorow.month = 1;
		}
		if (s_tommorow.dayOfWeek > SUNDAY) s_tommorow.dayOfWeek = MONDAY;
	}

	if (flags & INTERFACE_MENU_RETURNED)
	{
		switch (s_returnFrom)
		{
		case MENU_CHANGEPROFILE_RETURN_TODAY: menuChangeProfileApplyProfile(&s_today, menuChooseProfileGet()); break;
		case MENU_CHANGEPROFILE_RETURN_TOMMOROW: menuChangeProfileApplyProfile(&s_tommorow, menuChooseProfileGet()); break;
		}

		s_returnFrom = 0;
	}

	menuWrite(&s_menu);

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		switch (menuGetCurrentOption(&s_menu))
		{
		case MENU_CHANGEPROFILE_TODAY:
			s_returnFrom = MENU_CHANGEPROFILE_RETURN_TODAY;
			menuChooseProfileSet(s_today.profile);
			interfaceMenuOpen(&menuChooseProfile);
			break;
		case MENU_CHANGEPROFILE_TOMMOROW:
			s_returnFrom = MENU_CHANGEPROFILE_RETURN_TOMMOROW;
			menuChooseProfileSet(s_tommorow.profile);
			interfaceMenuOpen(&menuChooseProfile);
			break;
		case MENU_CHANGEPROFILE_RETURN: interfaceMenuPrevious(); break;
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

static void menuChangeProfileApplyProfile(menuChangeProfileData_t* data, uint8_t profile)
{
	month_t month;
	internalStorageMonthRead(data->month - 1, &month);

	month.days[data->day] = profile;
	data->profile = profile;
	controllerMonthUpdate(data->month - 1, &month);
}

static const char* menuChangeProfileGetString(uint8_t option, uint8_t* textWidth)
{
	switch (option)
	{
	case MENU_CHANGEPROFILE_TODAY:
		stringsLoad(stringsBuffer, stringsToday);
		sprintf(menuBuffer, "%s (%c)", stringsBuffer, menuGetProfileChar(s_today.profile));
		return menuBuffer;

	case MENU_CHANGEPROFILE_TOMMOROW:
		stringsLoad(stringsBuffer, stringsTommorow);
		sprintf(menuBuffer, "%s (%c)", stringsBuffer, menuGetProfileChar(s_tommorow.profile));
		return menuBuffer;

	case MENU_CHANGEPROFILE_RETURN:
		stringsLoad(stringsBuffer, stringsReturn);
		return stringsBuffer;
	}

	return "";
}
