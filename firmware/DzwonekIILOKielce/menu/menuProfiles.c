#include "menuProfiles.h"

#include <stdio.h>
#include "../common.h"
#include "../interface.h"
#include "../strings.h"
#include "menu.h"
#include "menuEditProfile.h"

#define MENU_PROFILES_CANCEL PROFILE_COUNT // because cancel is last option
#define MENU_PROFILES_COUNT (PROFILE_COUNT + 1) // profile options + cancel option

static menu_t s_menu;

static const char* menuProfilesGetString(uint8_t option, uint8_t* textWidth);

void menuProfiles(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		menuCreate(&s_menu, MENU_PROFILES_COUNT, &menuProfilesGetString, INTERFACE_MENU_LAST_TIMER);
	}

	menuWrite(&s_menu);

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		if (menuGetCurrentOption(&s_menu) == MENU_PROFILES_CANCEL) interfaceMenuPrevious();
		else
		{
			menuEditProfileSet(menuGetCurrentOption(&s_menu));
			interfaceMenuOpen(&menuEditProfile);
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

static const char* menuProfilesGetString(uint8_t option, uint8_t* textWidth)
{
	if (option == MENU_PROFILES_CANCEL)
	{
		stringsLoad(stringsBuffer, stringsReturn);
		return stringsBuffer;
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
