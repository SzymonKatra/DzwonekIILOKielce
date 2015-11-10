#include "menuDcf77Synchronization.h"

#include <stdio.h>
#include "../controller.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"
#include "menuDcf77LastSynchronization.h"

#define MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_HOUR 0
#define MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_MINUTE 1
#define MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_FOR 2

#define menuDcf77SynchronizationUpdateMenuOptionsCount() menuSetOptionsCount(&s_menu, (s_hour == 0xFF ? 3 : 5))
#define menuDcf77SynchronizationNeedBlink() (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS)
#define menuDcf77SynchronizationWriteHour() { sprintf(menuBuffer, MENU_HOUR_FORMAT, s_hour); interfaceDisplayWrite(MENU_DCF77SYNCHRONIZATION_HOUR_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), menuBuffer); }
#define menuDcf77SynchronizationWriteMinute() { sprintf(menuBuffer, MENU_MINUTE_FORMAT, s_minute); interfaceDisplayWrite(MENU_DCF77SYNCHRONIZATION_MINUTE_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), menuBuffer); }
#define menuDcf77SynchronizationWriteFor() { sprintf(menuBuffer, MENU_DCF77SYNCHRONIZATION_FOR_FORMAT, s_for); interfaceDisplayWrite(MENU_DCF77SYNCHRONIZATION_FOR_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), menuBuffer); }

static const settings_t* s_currentSettings;
static menu_t s_menu;
static uint16_t s_for;
static uint16_t s_editing;
static uint8_t s_currentHighlight;
static uint8_t s_hour;
static uint8_t s_minute;

static const char* menuDcf77SynchronizationGetString(uint8_t option, uint8_t* textWidth);

void menuDcf77Synchronization(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_currentSettings = controllerSettingsGetPointer();
		s_editing = 0;
		s_hour = s_currentSettings->dcf77SynchronizationTime / 60;
		s_minute = s_currentSettings->dcf77SynchronizationTime % 60;
		s_for = s_currentSettings->dcf77SynchronizationMaxLength;
		menuCreate(&s_menu, 0, &menuDcf77SynchronizationGetString, INTERFACE_MENU_LAST_TIMER);
		menuDcf77SynchronizationUpdateMenuOptionsCount();
	}

	menuWrite(&s_menu);
	if (s_editing)
	{
		if (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS * 2) interfaceMenuTimerReset(0);

		switch (s_currentHighlight)
		{
		case MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_HOUR:
			if (menuDcf77SynchronizationNeedBlink())
				interfaceDisplayWrite(MENU_DCF77SYNCHRONIZATION_HOUR_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), "  ");
			break;

		case MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_MINUTE:
			if (menuDcf77SynchronizationNeedBlink())
				interfaceDisplayWrite(MENU_DCF77SYNCHRONIZATION_MINUTE_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), "  ");
			break;

		case MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_FOR:
			if (menuDcf77SynchronizationNeedBlink())
				interfaceDisplayWrite(MENU_DCF77SYNCHRONIZATION_FOR_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), "    ");
			break;
		}
	}

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		if (s_editing)
		{
			if (s_currentHighlight >= MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_HOUR && s_currentHighlight <= MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_MINUTE)
			{
				s_currentHighlight++;

				menuDcf77SynchronizationWriteHour();
				menuDcf77SynchronizationWriteMinute();

				if (s_currentHighlight > MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_MINUTE)
				{
					s_editing = 0;
					menuDcf77SynchronizationUpdateMenuOptionsCount();
				}
			}
			else if (s_currentHighlight == MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_FOR)
			{
				menuDcf77SynchronizationWriteFor();

				s_editing = 0;
			}
		}
		else
		{
			settings_t tmpSettings = *s_currentSettings;
			tmpSettings.dcf77SynchronizationTime = (s_hour == 0xFF ? 0xFFFF : s_hour * 60 + s_minute);
			tmpSettings.dcf77SynchronizationMaxLength = s_for;

			if (s_hour == 0xFF)
			{
				switch (menuGetCurrentOption(&s_menu))
				{
				case 0:
					s_hour = 0;
					s_minute = 0;
					menuDcf77SynchronizationUpdateMenuOptionsCount();
					break;
				case 1: interfaceMenuOpen(&menuDcf77LastSynchronization); break;
				case 2:
					controllerSettingsApply(&tmpSettings);
					interfaceMenuPrevious();
					break;
				}
			}
			else
			{
				switch (menuGetCurrentOption(&s_menu))
				{
				case 0:
					s_currentHighlight = MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_HOUR;
					s_editing = 1;
					break;
				case 1:
					s_currentHighlight = MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_FOR;
					s_editing = 1;
					break;
				case 2: interfaceMenuOpen(&menuDcf77LastSynchronization); break;
				case 3:
					s_hour = 0xFF;
					menuDcf77SynchronizationUpdateMenuOptionsCount();
					break;
				case 4:
					controllerSettingsApply(&tmpSettings);
					interfaceMenuPrevious();
					break;
				}
			}
		}
	}
	else
	{
		if (s_editing)
		{
			if (interfaceHandleButtonPulse(INTERFACE_BUTTON_DOWN))
			{
				int32_t tmpFor = (int32_t)s_for; // to aviod uint16_t overflow
				switch(s_currentHighlight)
				{
				case MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_HOUR: valueDecRange(s_hour, MENU_HOUR_MIN, MENU_HOUR_MAX); break;
				case MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_MINUTE: valueDecRange(s_minute, MENU_MINUTE_MIN, MENU_MINUTE_MAX); break;
				case MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_FOR:
					if (tmpFor > 300)
						tmpFor -= 30;
					else if (tmpFor > 60)
						tmpFor -= 10;
					else if (tmpFor > 10)
						tmpFor -= 5;
					else tmpFor--;

					if (tmpFor < MENU_DCF77SYNCHRONIZATION_FOR_MIN) tmpFor = MENU_DCF77SYNCHRONIZATION_FOR_MAX;
					s_for = (uint16_t)tmpFor;
					break;
				}
			}
			else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_UP))
			{
				int32_t tmpFor = (int32_t)s_for;  // to aviod uint16_t overflow
				switch(s_currentHighlight)
				{
				case MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_HOUR: valueIncRange(s_hour, MENU_MINUTE_MIN, MENU_HOUR_MAX); break;
				case MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_MINUTE: valueIncRange(s_minute, MENU_MINUTE_MIN, MENU_MINUTE_MAX); break;
				case MENU_DCF77SYNCHRONIZATION_HIGHLIGHT_FOR:
					if (tmpFor >= 300)
						tmpFor += 30;
					else if (tmpFor >= 60)
						tmpFor += 10;
					else if (tmpFor >= 10)
						tmpFor += 5;
					else tmpFor++;

					if (tmpFor > MENU_DCF77SYNCHRONIZATION_FOR_MAX) tmpFor = MENU_DCF77SYNCHRONIZATION_FOR_MIN;
					s_for = (uint16_t)tmpFor;
					break;
				}
			}
		}
		else
		{
			if (interfaceHandleButtonPressed(INTERFACE_BUTTON_DOWN))
			{
				menuNextOption(&s_menu);
			}
			else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_UP))
			{
				menuPreviousOption(&s_menu);
			}
		}
	}
}

static const char* menuDcf77SynchronizationGetString(uint8_t option, uint8_t* textWidth)
{
	if (s_hour == 0xFF)
	{
		switch (option)
		{
		case 0:
			stringsLoad(stringsBuffer, stringsEnable);
			return stringsBuffer;
		case 1:
			stringsLoad(stringsBuffer, stringsLastSynchronization);
			return stringsBuffer;
		case 2:
			stringsLoad(stringsBuffer, stringsSave);
			return stringsBuffer;
		}
	}
	else
	{
		char* stringsHalfBuffer = (char*)(stringsBuffer + STRINGS_BUFFER_SIZE);
		switch (option)
		{
		case 0:
			stringsLoad(stringsBuffer, stringsAt);
			sprintf(menuBuffer, "%s "MENU_HOUR_FORMAT" "MENU_TIME_SEPARATOR_STR" "MENU_MINUTE_FORMAT, stringsBuffer, s_hour, s_minute);
			return menuBuffer;
		case 1:
			stringsLoad(stringsBuffer, stringsFor);
			stringsLoad(stringsHalfBuffer, stringsMinutesShortened);
			sprintf(menuBuffer, "%s "MENU_DCF77SYNCHRONIZATION_FOR_FORMAT" %s", stringsBuffer, s_for, stringsHalfBuffer);
			return menuBuffer;
		case 2:
			stringsLoad(stringsBuffer, stringsLastSynchronization);
			return stringsBuffer;
		case 3:
			stringsLoad(stringsBuffer, stringsDisable);
			return stringsBuffer;
		case 4:
			stringsLoad(stringsBuffer, stringsSave);
			return stringsBuffer;
		}
	}

	return "";
}
