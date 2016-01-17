#include "menuMain.h"

#include <stdio.h>
#include <string.h>
#include "../clock.h"
#include "../controller.h"
#include "../customChars.h"
#include "../interface.h"
#include "../strings.h"
#include "../device/dcf77.h"
#include "menu.h"
#include "menuAutoTimeTransition.h"
#include "menuBellLength.h"
#include "menuChangeProfile.h"
#include "menuDcf77Synchronization.h"
#include "menuEditAlarm.h"
#include "menuManageSettings.h"
#include "menuMonths.h"
#include "menuProfiles.h"
#include "menuSetDate.h"
#include "menuSetTime.h"

#define MENU_MAIN_SET_DATE 0
#define MENU_MAIN_SET_TIME 1
#define MENU_MAIN_AUTO_TIME_TRANSITION 2
#define MENU_MAIN_DCF77_SYNCHRONIZATION 3
#define MENU_MAIN_BELL_LENGTH 4
#define MENU_MAIN_PROFILES 5
#define MENU_MAIN_CHANGE_PROFILE 6
#define MENU_MAIN_CALENDAR 7
#define MENU_MAIN_ALARM 8
#define MENU_MAIN_MANAGE_SETTINGS 9
#define MENU_MAIN_COUNT 10

#define MENU_MAIN_MODE_DEFAULT 0
#define MENU_MAIN_MODE_STATUS 1
#define MENU_MAIN_MODE_SPLASH 2

static menu_t s_menu;

static const dateTime_t* s_currentDateTime;
static const settings_t* s_currentSettings;
static uint8_t s_displaySeconds;
static uint8_t s_mode;
#define menuMainWrite(x, y, template, value) { sprintf(menuBuffer, template, value); interfaceDisplayWrite(x, y, menuBuffer); }

static void menuMainWriteStatus();

static const char* menuMainGetString(uint8_t option, uint8_t* textWidth);

void menuMain(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_currentDateTime = clockGetCurrentDateTimePointer();
		s_currentSettings = controllerSettingsGetPointer();
		menuCreate(&s_menu, MENU_MAIN_COUNT, &menuMainGetString, INTERFACE_MENU_LAST_TIMER);
		s_mode = MENU_MAIN_MODE_SPLASH;
	}
	if (flags & INTERFACE_MENU_WAKEUP)
	{
		s_mode = MENU_MAIN_MODE_SPLASH;
		interfaceMenuTimerReset(0);
	}

	if (s_mode == MENU_MAIN_MODE_SPLASH)
	{
		stringsLoad(stringsBuffer, stringsSplashText);
		interfaceDisplayWrite(MENU_MAIN_SPLASH_X, MENU_MAIN_SPLASH_Y, stringsBuffer);
		if (interfaceMenuTimerGet(0) >= MENU_MAIN_SPLASH_TICKS) s_mode = MENU_MAIN_MODE_STATUS;
	}
	else if (s_mode == MENU_MAIN_MODE_STATUS)
	{
		menuMainWriteStatus();
		if (interfaceHandleButtonPressed(INTERFACE_BUTTON_DOWN) | interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
		{
			menuSelectFirstOption(&s_menu);
			s_mode = 0;
		}
		else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_UP))
		{
			menuSelectLastOption(&s_menu);
			s_mode = 0;
		}
	}
	else
	{
		menuWrite(&s_menu);

		if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
		{
			switch (menuGetCurrentOption(&s_menu))
			{
			case MENU_MAIN_SET_DATE: interfaceMenuOpen(&menuSetDate); break;
			case MENU_MAIN_SET_TIME: interfaceMenuOpen(&menuSetTime); break;
			case MENU_MAIN_AUTO_TIME_TRANSITION: interfaceMenuOpen(&menuAutoTimeTransition); break;
			case MENU_MAIN_DCF77_SYNCHRONIZATION: interfaceMenuOpen(&menuDcf77Synchronization); break;
			case MENU_MAIN_BELL_LENGTH: interfaceMenuOpen(&menuBellLength); break;
			case MENU_MAIN_CHANGE_PROFILE: interfaceMenuOpen(&menuChangeProfile); break;
			case MENU_MAIN_PROFILES: interfaceMenuOpen(&menuProfiles); break;
			case MENU_MAIN_CALENDAR: interfaceMenuOpen(&menuMonths); break;
			case MENU_MAIN_ALARM: interfaceMenuOpen(&menuEditAlarm); break;
			case MENU_MAIN_MANAGE_SETTINGS: interfaceMenuOpen(&menuManageSettings); break;
			}
		}
		else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_DOWN))
		{
			if (menuNextOption(&s_menu)) s_mode = MENU_MAIN_MODE_STATUS;
		}
		else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_UP))
		{
			if (menuPreviousOption(&s_menu)) s_mode = MENU_MAIN_MODE_STATUS;
		}
	}
}

static void menuMainWriteStatus()
{
	interfaceDisplayWriteChar(MENU_MAIN_HOUR_MINUTE_SEPARATOR_X, MENU_MAIN_HOUR_MINUTE_SEPARATOR_Y, MENU_TIME_SEPARATOR);
	interfaceDisplayWriteChar(MENU_MAIN_MINUTE_SECOND_SEPARATOR_X, MENU_MAIN_MINUTE_SECOND_SEPARATOR_Y, MENU_TIME_SEPARATOR);
	interfaceDisplayWriteChar(MENU_MAIN_DAY_MONTH_SEPARATOR_X, MENU_MAIN_DAY_MONTH_SEPARATOR_Y, MENU_DATE_SEPARATOR);
	interfaceDisplayWriteChar(MENU_MAIN_MONTH_YEAR_SEPARATOR_X, MENU_MAIN_MONTH_YEAR_SEPARATOR_Y, MENU_DATE_SEPARATOR);

	menuMainWrite(MENU_MAIN_HOUR_X, MENU_MAIN_HOUR_Y, MENU_HOUR_FORMAT, s_currentDateTime->hours);
	menuMainWrite(MENU_MAIN_MINUTE_X, MENU_MAIN_MINUTE_Y, MENU_MINUTE_FORMAT, s_currentDateTime->minutes);
	menuMainWrite(MENU_MAIN_SECOND_X, MENU_MAIN_SECOND_Y, MENU_SECOND_FORMAT, s_currentDateTime->seconds);

	menuMainWrite(MENU_MAIN_DAY_X, MENU_MAIN_DAY_Y, MENU_DAY_FORMAT, s_currentDateTime->day);
	menuMainWrite(MENU_MAIN_MONTH_X, MENU_MAIN_MONTH_Y, MENU_MONTH_FORMAT, s_currentDateTime->month);
	menuMainWrite(MENU_MAIN_YEAR_X, MENU_MAIN_YEAR_Y, MENU_YEAR_FORMAT, (uint16_t)s_currentDateTime->year + 1900);

	char timeTransitionChar = MENU_MAIN_TIME_TRANSITION_STATE_NONE_CHAR;
	switch (clockGetTimeTransitionState())
	{
	case CLOCK_TIME_TRANSITION_CET_TO_CEST: timeTransitionChar = MENU_MAIN_TIME_TRANSITION_STATE_CET_TO_CEST_CHAR; break;
	case CLOCK_TIME_TRANSITION_CEST_TO_CET: timeTransitionChar = MENU_MAIN_TIME_TRANSITION_STATE_CEST_TO_CET_CHAR; break;
	}
	interfaceDisplayWriteChar(MENU_MAIN_TIME_TRANSITION_STATE_X, MENU_MAIN_TIME_TRANSITION_STATE_Y, timeTransitionChar);

	menuMainWrite(MENU_MAIN_DAYOFWEEK_X, MENU_MAIN_DAYOFWEEK_Y, "%s", menuGetDayOfWeekName(s_currentDateTime->dayOfWeek));

	if (controllerGetBellRemainingTicks() > 0)
	{
		interfaceDisplayWriteChar(MENU_MAIN_NEXT_BELL_X + 1, MENU_MAIN_NEXT_BELL_Y, CUSTOMCHARS_BELL_ANSI);
	}
	else
	{
		uint8_t nextBell = controllerGetNextBell();
		if (nextBell != 0xFF)
		{
			sprintf(menuBuffer, MENU_MAIN_NEXT_BELL_FORMAT, nextBell + 1);
			interfaceDisplayWrite(MENU_MAIN_NEXT_BELL_X, MENU_MAIN_NEXT_BELL_Y, menuBuffer);
		}
		else
		{
			interfaceDisplayWrite(MENU_MAIN_NEXT_BELL_X, MENU_MAIN_NEXT_BELL_Y, MENU_MAIN_NEXT_BELL_NONE_TEXT);
		}
	}

	uint8_t profile = controllerProfileGetCurrent();

	interfaceDisplayWriteChar(MENU_MAIN_PROFILE_X, MENU_MAIN_PROFILE_Y, menuGetProfileChar(profile));

	if (dcf77IsEnabled())
	{
		interfaceDisplayWriteChar(MENU_MAIN_SYNCHRONIZATION_ICON_X, MENU_MAIN_SYNCHRONIZATION_ICON_Y, (dcf77GetSignalState() ? CUSTOMCHARS_ANTENNA_RECEIVED_ANSI : CUSTOMCHARS_ANTENNA_ANSI));

		uint8_t received = dcf77GetReceivedCount();
		if (dcf77IsSynchronized())
		{
			// total = 59 (bits in DCF77 packet frame)
			// maxProgress = 40 (number of pixels in character)
			// currentProgress = (received / total) * maxProgress
			// due to integer maths in must be transformed into this form:
			// currentProgress = (((received * 100) / total) * maxProgress) / 100

			uint8_t currentProgress = (uint8_t)(((((uint16_t)received * 100) / 59) * 40) / 100);
			uint8_t lines = currentProgress / 5;
			uint8_t remaining = currentProgress % 5;

			uint8_t progressChar[8];
			memset(&progressChar, 0, 8);

			memset(&progressChar, 0x1F, lines);
			uint8_t remainingLineValue = 0x00;
			for (uint8_t i = 0; i < remaining; i++) remainingLineValue |= (1 << (4 - i));
			progressChar[lines] = remainingLineValue;

			customCharsUpdateProgress(progressChar);
			interfaceDisplayInvalidateCustomChar(CUSTOMCHARS_PROGRESS);

			interfaceDisplayWriteChar(MENU_MAIN_SYNCHRONIZATION_STATE_X, MENU_MAIN_SYNCHRONIZATION_STATE_Y, CUSTOMCHARS_PROGRESS_ANSI);
		}
		else if (received == 59)
		{
			uint8_t progressChar[8];
			memset(&progressChar, 0x1F, 8);

			customCharsUpdateProgress(progressChar);
			interfaceDisplayInvalidateCustomChar(CUSTOMCHARS_PROGRESS);

			interfaceDisplayWriteChar(MENU_MAIN_SYNCHRONIZATION_STATE_X, MENU_MAIN_SYNCHRONIZATION_STATE_Y, CUSTOMCHARS_PROGRESS_ANSI);
		}
		else
		{
			uint16_t ticks = interfaceMenuTimerGet(2);
			interfaceDisplayWriteChar(MENU_MAIN_SYNCHRONIZATION_STATE_X, MENU_MAIN_SYNCHRONIZATION_STATE_Y,
									  (ticks < MENU_MAIN_SYNCHRONIZATION_FINDING_TOGGLE_TICKS ? CUSTOMCHARS_FINDING_UP_ANSI : CUSTOMCHARS_FINDING_DOWN_ANSI));

			if (ticks >= MENU_MAIN_SYNCHRONIZATION_FINDING_TOGGLE_TICKS * 2) interfaceMenuTimerReset(2);
		}
	}

	if (s_displaySeconds != s_currentDateTime->seconds)
	{
		interfaceMenuTimerReset(1);
		interfaceDisplayWriteChar(MENU_MAIN_MINUTE_SECOND_SEPARATOR_X, MENU_MAIN_MINUTE_SECOND_SEPARATOR_Y, ':');
	}

	s_displaySeconds = s_currentDateTime->seconds;
}

static const char* menuMainGetString(uint8_t option, uint8_t* textWidth)
{
	switch (option)
	{
	case MENU_MAIN_SET_DATE: stringsLoad(stringsBuffer, stringsSetDate); break;
	case MENU_MAIN_SET_TIME: stringsLoad(stringsBuffer, stringsSetTime); break;
	case MENU_MAIN_AUTO_TIME_TRANSITION: stringsLoad(stringsBuffer, stringsAutoTimeTransition); break;
	case MENU_MAIN_DCF77_SYNCHRONIZATION: stringsLoad(stringsBuffer, stringsDcf77Synchronization); break;
	case MENU_MAIN_BELL_LENGTH: stringsLoad(stringsBuffer, stringsBellLength); break;
	case MENU_MAIN_CHANGE_PROFILE: stringsLoad(stringsBuffer, stringsChangeProfile); break;
	case MENU_MAIN_PROFILES: stringsLoad(stringsBuffer, stringsBellEnableHours); break;
	case MENU_MAIN_CALENDAR: stringsLoad(stringsBuffer, stringsCalendar); break;
	case MENU_MAIN_ALARM: stringsLoad(stringsBuffer, stringsAlarm); break;
	case MENU_MAIN_MANAGE_SETTINGS: stringsLoad(stringsBuffer, stringsManageSettings); break;
	}

	return stringsBuffer;
}
