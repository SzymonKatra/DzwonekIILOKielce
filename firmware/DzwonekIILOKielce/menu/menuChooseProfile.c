#include "menuChooseProfile.h"

#include <stdio.h>
#include "../common.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"

#define MENU_CHOOSEPROFILE_NONE PROFILE_COUNT
#define MENU_CHOOSEPROFILE_CANCEL (PROFILE_COUNT + 1)
#define MENU_CHOOSEPROFILE_COUNT (PROFILE_COUNT + 2)

static menu_t s_menu;
static uint8_t s_selectedProfile;

static const char* menuChooseProfileGetString(uint8_t option, uint8_t* textWidth);

void menuChooseProfile(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		menuCreate(&s_menu, MENU_CHOOSEPROFILE_COUNT, &menuChooseProfileGetString, INTERFACE_MENU_LAST_TIMER);

		menuSelectOption(&s_menu, (s_selectedProfile == 0xFF ? MENU_CHOOSEPROFILE_NONE : s_selectedProfile));
	}

	menuWrite(&s_menu);

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		uint8_t currentOption = menuGetCurrentOption(&s_menu);

		if (currentOption != MENU_CHOOSEPROFILE_CANCEL)
		{
			s_selectedProfile = (currentOption == MENU_CHOOSEPROFILE_NONE ? 0xFF : currentOption);
		}

		interfaceMenuPrevious();
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
void menuChooseProfileSet(uint8_t profile)
{
	s_selectedProfile = profile;
}
uint8_t menuChooseProfileGet()
{
	return s_selectedProfile;
}

static const char* menuChooseProfileGetString(uint8_t option, uint8_t* textWidth)
{
	if (option == MENU_CHOOSEPROFILE_CANCEL)
	{
		stringsLoad(stringsBuffer, stringsCancel);
		return stringsBuffer;
	}
	else if (option == MENU_CHOOSEPROFILE_NONE)
	{
		stringsLoad(stringsBuffer, stringsProfileNone);

		sprintf(menuBuffer, "%s (%c)", stringsBuffer, menuGetProfileChar(0xFF));

		return menuBuffer;
	}
	else if (option == MENU_PROFILE_NORMAL_NUMBER)
			stringsLoad(stringsBuffer, stringsProfileNormal);
	else if (option == MENU_PROFILE_SHORTENED_NUMBER)
			stringsLoad(stringsBuffer, stringsProfileShortened);
	else
	{
		stringsLoad(stringsBuffer, stringsProfileOther);
		char profileChar = menuGetProfileChar(option);
		sprintf(menuBuffer, "%s %c (%c)", stringsBuffer, profileChar, profileChar);
		return menuBuffer;
	}

	sprintf(menuBuffer, "%s (%c)", stringsBuffer, menuGetProfileChar(option));

	return menuBuffer;
}
