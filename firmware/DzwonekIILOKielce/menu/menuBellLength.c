#include "menuBellLength.h"

#define MENU_BELLLENGTH_FOR_LESSON 0
#define MENU_BELLLENGTH_FOR_BREAK 1
#define MENU_BELLLENGTH_SAVE 2
#define MENU_BELLLENGTH_CANCEL 3
#define MENU_BELLLENGTH_COUNT 4

#include <stdio.h>
#include "../controller.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"

#define menuBellLengthNeedBlink() (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS)

static menu_t s_menu;
static uint8_t s_forLesson;
static uint8_t s_forBreak;
static uint8_t s_editing;

static const char* menuBellLengthGetString(uint8_t option, uint8_t* textWidth);

void menuBellLength(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		menuCreate(&s_menu, MENU_BELLLENGTH_COUNT, &menuBellLengthGetString, INTERFACE_MENU_LAST_TIMER);
		s_editing = 0;

		const settings_t* settings = controllerSettingsGetPointer();
		s_forLesson = settings->bellLengthForLesson;
		s_forBreak = settings->bellLengthForBreak;
	}

	menuWrite(&s_menu);

	if (s_editing)
	{
		if (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS * 2) interfaceMenuTimerReset(0);

		if (menuBellLengthNeedBlink()) interfaceDisplayWrite(MENU_BELLLENGTH_VALUE_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), "  ");
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
			case MENU_BELLLENGTH_FOR_LESSON:
			case MENU_BELLLENGTH_FOR_BREAK:
				s_editing = 1;
				break;

			case MENU_BELLLENGTH_SAVE:
				settings = *controllerSettingsGetPointer();
				settings.bellLengthForLesson = s_forLesson;
				settings.bellLengthForBreak = s_forBreak;
				controllerSettingsApply(&settings);
				interfaceMenuPrevious();
				break;

			case MENU_BELLLENGTH_CANCEL:
				interfaceMenuPrevious();
				break;
			}
		}
	}

	if (s_editing)
	{
		if (interfaceHandleButtonPulse(INTERFACE_BUTTON_UP))
		{
			switch (menuGetCurrentOption(&s_menu))
			{
			case MENU_BELLLENGTH_FOR_LESSON: valueIncRange(s_forLesson, MENU_BELLLENGTH_VALUE_MIN, MENU_BELLLENGTH_VALUE_MAX); break;
			case MENU_BELLLENGTH_FOR_BREAK: valueIncRange(s_forBreak, MENU_BELLLENGTH_VALUE_MIN, MENU_BELLLENGTH_VALUE_MAX);  break;
			}
		}
		else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_DOWN))
		{
			switch (menuGetCurrentOption(&s_menu))
			{
			case MENU_BELLLENGTH_FOR_LESSON: valueDecRange(s_forLesson, MENU_BELLLENGTH_VALUE_MIN, MENU_BELLLENGTH_VALUE_MAX); break;
			case MENU_BELLLENGTH_FOR_BREAK: valueDecRange(s_forBreak, MENU_BELLLENGTH_VALUE_MIN, MENU_BELLLENGTH_VALUE_MAX);  break;
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

static const char* menuBellLengthGetString(uint8_t option, uint8_t* textWidth)
{
	switch (option)
	{
	case MENU_BELLLENGTH_FOR_LESSON:
		stringsLoad(stringsBuffer, stringsForLesson);
		sprintf(menuBuffer, "%s  "MENU_BELLLENGTH_VALUE_FORMAT" "MENU_BELLLENGTH_SECONDS_CHAR_STR, stringsBuffer, s_forLesson);
		return menuBuffer;

	case MENU_BELLLENGTH_FOR_BREAK:
		stringsLoad(stringsBuffer, stringsForBreak);
		sprintf(menuBuffer, "%s "MENU_BELLLENGTH_VALUE_FORMAT" "MENU_BELLLENGTH_SECONDS_CHAR_STR, stringsBuffer, s_forBreak);
		return menuBuffer;

	case MENU_BELLLENGTH_SAVE:
		stringsLoad(stringsBuffer, stringsSave);
		return stringsBuffer;

	case MENU_BELLLENGTH_CANCEL:
		stringsLoad(stringsBuffer, stringsCancel);
		return stringsBuffer;
	}

	return "";
}
