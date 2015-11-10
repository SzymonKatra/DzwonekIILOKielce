#include "menuAutoTimeTransition.h"

#include "../controller.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"

#define MENU_AUTOTIMETRANSITION_YES 0
#define MENU_AUTOTIMETRANSITION_NO 1
#define MENU_AUTOTIMETRANSITION_COUNT 2

static const settings_t* s_currentSettings;
static menu_t s_menu;

static const char* menuAutoTimeTransitionGetString(uint8_t option, uint8_t* textWidth);

void menuAutoTimeTransition(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_currentSettings = controllerSettingsGetPointer();

		menuCreate(&s_menu, MENU_AUTOTIMETRANSITION_COUNT, &menuAutoTimeTransitionGetString, INTERFACE_MENU_LAST_TIMER);

		menuSelectOption(&s_menu, (s_currentSettings->autoTimeTransition ? MENU_AUTOTIMETRANSITION_YES : MENU_AUTOTIMETRANSITION_NO));
	}

	menuWrite(&s_menu);

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		settings_t tmpSettings = *s_currentSettings;

		switch (menuGetCurrentOption(&s_menu))
		{
		case MENU_AUTOTIMETRANSITION_YES: tmpSettings.autoTimeTransition = 1; break;
		case MENU_AUTOTIMETRANSITION_NO: tmpSettings.autoTimeTransition = 0; break;
		}

		controllerSettingsApply(&tmpSettings);
		interfaceMenuPrevious();
	}
	else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_DOWN))
	{
		menuNextOption(&s_menu);
	}
	else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_UP))
	{
		menuPreviousOption(&s_menu);
	}
}

static const char* menuAutoTimeTransitionGetString(uint8_t option, uint8_t* textWidth)
{
	switch (option)
	{
	case MENU_AUTOTIMETRANSITION_YES: stringsLoad(stringsBuffer, stringsYes); break;
	case MENU_AUTOTIMETRANSITION_NO: stringsLoad(stringsBuffer, stringsNo); break;
	}

	return stringsBuffer;
}
