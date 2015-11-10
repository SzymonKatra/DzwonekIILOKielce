#include "menuManageSettings.h"

#include "../interface.h"
#include "../internalStorage.h"
#include "../strings.h"
#include "menu.h"
#include "menuSdCard.h"
#include "menuYesNo.h"

#define MENU_MANAGESETTINGS_LOAD_SDCARD_SETTINGS 0
#define MENU_MANAGESETTINGS_SAVE_SDCARD_SETTINGS 1
#define MENU_MANAGESETTINGS_FACTORY_SETTINGS 2
#define MENU_MANAGESETTINGS_RETURN 3
#define MENU_MANAGESETTINGS_COUNT 4

#define MENU_MAIN_RETURN_FACTORY_SETTINGS_CONFIRMATION 1
#define MENU_MAIN_RETURN_LOAD_SD_CONFIRMATION 2
#define MENU_MAIN_RETURN_SAVE_SD_CONFIRMATION 3

static menu_t s_menu;
static uint8_t s_returnFrom;

static const char* menuManageSettingsGetString(uint8_t option, uint8_t* textWidth);

void menuManageSettings(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_returnFrom = 0;
		menuCreate(&s_menu, MENU_MANAGESETTINGS_COUNT, &menuManageSettingsGetString, INTERFACE_MENU_LAST_TIMER);
	}

	if (flags & INTERFACE_MENU_RETURNED)
	{
		switch (s_returnFrom)
		{
		case MENU_MAIN_RETURN_FACTORY_SETTINGS_CONFIRMATION:
			if (menuYesNoGet() == MENU_YESNO_YES)
			{
				internalStorageInvalidateChecksum();
				resetSystem();
			}
			break;
		case MENU_MAIN_RETURN_LOAD_SD_CONFIRMATION:
			if (menuYesNoGet() == MENU_YESNO_YES)
			{
				menuSdCardSet(MENU_SDCARD_MODE_LOAD);
				interfaceMenuOpen(&menuSdCard);
			}
			break;
		case MENU_MAIN_RETURN_SAVE_SD_CONFIRMATION:
			if (menuYesNoGet() == MENU_YESNO_YES)
			{
				menuSdCardSet(MENU_SDCARD_MODE_SAVE);
				interfaceMenuOpen(&menuSdCard);
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
		case MENU_MANAGESETTINGS_LOAD_SDCARD_SETTINGS:
			s_returnFrom = MENU_MAIN_RETURN_LOAD_SD_CONFIRMATION;
			menuYesNoSet(MENU_YESNO_YES, stringsDoYouWantLoadFromSdCard);
			interfaceMenuOpen(&menuYesNo);
			break;
		case MENU_MANAGESETTINGS_SAVE_SDCARD_SETTINGS:
			s_returnFrom = MENU_MAIN_RETURN_SAVE_SD_CONFIRMATION;
			menuYesNoSet(MENU_YESNO_YES, stringsDoYouWantSaveToSdCard);
			interfaceMenuOpen(&menuYesNo);
			break;
		case MENU_MANAGESETTINGS_FACTORY_SETTINGS:
			s_returnFrom = MENU_MAIN_RETURN_FACTORY_SETTINGS_CONFIRMATION;
			menuYesNoSet(MENU_YESNO_NO, stringsDoYouWantFactorySettings);
			interfaceMenuOpen(&menuYesNo);
			break;
		case MENU_MANAGESETTINGS_RETURN: interfaceMenuPrevious(); break;
		}
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

static const char* menuManageSettingsGetString(uint8_t option, uint8_t* textWidth)
{
	switch(option)
	{
	case MENU_MANAGESETTINGS_LOAD_SDCARD_SETTINGS: stringsLoad(stringsBuffer, stringsLoadSdCardSettings); break;
	case MENU_MANAGESETTINGS_SAVE_SDCARD_SETTINGS: stringsLoad(stringsBuffer, stringsSaveSdCardSettings); break;
	case MENU_MANAGESETTINGS_FACTORY_SETTINGS: stringsLoad(stringsBuffer, stringsFactorySettings); break;
	case MENU_MANAGESETTINGS_RETURN: stringsLoad(stringsBuffer, stringsReturn); break;
	}

	return stringsBuffer;
}
