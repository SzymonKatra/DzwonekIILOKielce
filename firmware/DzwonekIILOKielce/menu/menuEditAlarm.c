#include "menuEditAlarm.h"

#include <stdio.h>
#include "../common.h"
#include "../controller.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"

#define MENU_EDITALARM_LENGTH 0
#define MENU_EDITALARM_INTERVAL 1
#define MENU_EDITALARM_SAVE 2
#define MENU_EDITALARM_CANCEL 3
#define MENU_EDITALARM_COUNT 4

#define menuEditAlarmeNeedBlink() (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS)

static const settings_t* s_currentSettings;

static menu_t s_menu;

static uint8_t s_length;
static uint8_t s_interval;

static uint8_t s_editing;

void menuEditAlarmWriteLength();
void menuEditAlarmWriteInterval();
static const char* menuEditAlarmGetString(uint8_t option, uint8_t* textWidth);

void menuEditAlarm(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_currentSettings = controllerSettingsGetPointer();

		s_length = s_currentSettings->alarmLength;
		s_interval = s_currentSettings->alarmDiscontinuousInterval;

		s_editing = 0;

		menuCreate(&s_menu, MENU_EDITALARM_COUNT, &menuEditAlarmGetString, INTERFACE_MENU_LAST_TIMER);
	}

	menuWrite(&s_menu);
	menuEditAlarmWriteLength();
	menuEditAlarmWriteInterval();

	if (s_editing)
	{
		if (interfaceMenuTimerGet(0) >= 2 * MENU_HIGHLIGHT_TOGGLE_TICKS) interfaceMenuTimerReset(0);

		if (menuEditAlarmeNeedBlink())
		{
			switch (menuGetCurrentOption(&s_menu))
			{
			case MENU_EDITALARM_LENGTH:
				interfaceDisplayWrite(MENU_EDITALARM_LENGTH_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), "   ");
				break;

			case MENU_EDITALARM_INTERVAL:
				interfaceDisplayWrite(MENU_EDITALARM_INTERVAL_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), "  ");
				break;
			}
		}
	}

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		if (s_editing)
		{
			s_editing = 0;
		}
		else
		{
			settings_t settings;

			switch (menuGetCurrentOption(&s_menu))
			{
			case MENU_EDITALARM_LENGTH:
			case MENU_EDITALARM_INTERVAL:
				s_editing = 1;
				interfaceMenuTimerReset(0);
				break;
			case MENU_EDITALARM_SAVE:
				settings = *s_currentSettings;
				settings.alarmLength = s_length;
				settings.alarmDiscontinuousInterval = s_interval;
				controllerSettingsApply(&settings);
				interfaceMenuPrevious();
				break;

			case MENU_EDITALARM_CANCEL:
				interfaceMenuPrevious();
				break;
			}
		}
	}
	else
	{
		if (s_editing)
		{
			if (interfaceHandleButtonPulse(INTERFACE_BUTTON_DOWN))
			{
				int32_t tmpLength = (int32_t)s_length;  // to aviod uint16_t overflow
				switch (menuGetCurrentOption(&s_menu))
				{
				case MENU_EDITALARM_LENGTH:
					if (tmpLength > 300)
						tmpLength -= 30;
					else if (tmpLength > 60)
						tmpLength -= 10;
					else if (tmpLength > 10)
						tmpLength -= 5;
					else tmpLength--;

					if (tmpLength < MENU_EDITALARM_LENGTH_MIN) tmpLength = MENU_EDITALARM_LENGTH_MAX;
					s_length = (uint16_t)tmpLength;
					break;

				case MENU_EDITALARM_INTERVAL: valueDecRange(s_interval, MENU_EDITALARM_INTERVAL_MIN, MENU_EDITALARM_INTERVAL_MAX); break;
				}
			}
			else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_UP))
			{
				int32_t tmpLength = (int32_t)s_length;  // to aviod uint16_t overflow
				switch (menuGetCurrentOption(&s_menu))
				{
				case MENU_EDITALARM_LENGTH:
					if (tmpLength >= 300)
						tmpLength += 30;
					else if (tmpLength >= 60)
						tmpLength += 10;
					else if (tmpLength >= 10)
						tmpLength += 5;
					else tmpLength++;

					if (tmpLength > MENU_EDITALARM_LENGTH_MAX) tmpLength = MENU_EDITALARM_LENGTH_MIN;
					s_length = (uint16_t)tmpLength;
					break;

				case MENU_EDITALARM_INTERVAL: valueIncRange(s_interval, MENU_EDITALARM_INTERVAL_MIN, MENU_EDITALARM_INTERVAL_MAX); break;
				}
			}
		}
		else
		{
			if (interfaceHandleButtonPressed(INTERFACE_BUTTON_UP))
			{
				menuPreviousOption(&s_menu);
			}
			else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_DOWN))
			{
				menuNextOption(&s_menu);
			}
		}
	}
}

void menuEditAlarmWriteLength()
{
	uint8_t y = menuGetOptionY(&s_menu, MENU_EDITALARM_LENGTH);
	if (y == 0xFF) return;

	sprintf(menuBuffer, MENU_EDITALARM_LENGTH_FORMAT" "MENU_EDITALARM_SECONDS_CHAR_STR, s_length);

	interfaceDisplayWrite(MENU_EDITALARM_LENGTH_HIGHLIGHT_X, y, menuBuffer);
}
void menuEditAlarmWriteInterval()
{
	uint8_t y = menuGetOptionY(&s_menu, MENU_EDITALARM_INTERVAL);
	if (y == 0xFF) return;

	sprintf(menuBuffer, MENU_EDITALARM_INTERVAL_FORMAT" "MENU_EDITALARM_SECONDS_CHAR_STR, s_interval);

	interfaceDisplayWrite(MENU_EDITALARM_INTERVAL_HIGHLIGHT_X, y, menuBuffer);
}
static const char* menuEditAlarmGetString(uint8_t option, uint8_t* textWidth)
{
	switch (option)
	{
	case MENU_EDITALARM_LENGTH:
		stringsLoad(stringsBuffer, stringsAlarmLength);
		*textWidth = 9;
		break;
	case MENU_EDITALARM_INTERVAL:
		stringsLoad(stringsBuffer, stringsAlarmDiscontinuousInterval);
		*textWidth = 10;
		break;
	case MENU_EDITALARM_SAVE: stringsLoad(stringsBuffer, stringsSave); break;
	case MENU_EDITALARM_CANCEL: stringsLoad(stringsBuffer, stringsCancel); break;
	}

	return stringsBuffer;
}
