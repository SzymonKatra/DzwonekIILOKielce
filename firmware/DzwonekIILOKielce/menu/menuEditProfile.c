#include "menuEditProfile.h"

#include <stdio.h>
#include "../common.h"
#include "../controller.h"
#include "../interface.h"
#include "../internalStorage.h"
#include "../strings.h"
#include "menu.h"
#include "menuYesNo.h"

#define MENU_EDITPROFILE_ANY_BELL 0
#define MENU_EDITPROFILE_ADD 1
#define MENU_EDITPROFILE_CLEAR 2
#define MENU_EDITPROFILE_SAVE 3
#define MENU_EDITPROFILE_CANCEL 4

#define MENU_EDITPROFILE_OPTIONS_COUNT (s_data.count + 4)

#define MENU_EDITPROFILE_HIGHLIGHT_HOUR 0
#define MENU_EDITPROFILE_HIGHLIGHT_MINUTE 1
#define MENU_EDITPROFILE_HIGHLIGHT_BELLTYPE 2
#define MENU_EDITPROFILE_HIGHLIGHT_COUNT 3

#define MENU_EDITPROFILE_MODE_DEFAULT 0
#define MENU_EDITPROFILE_MODE_EDIT 1
#define MENU_EDITPROFILE_MODE_CLEAR 2

#define menuEditProfileNeedBlink() (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS)
#define menuEditProfileWriteHour() { sprintf(menuBuffer, MENU_HOUR_FORMAT, s_hour); interfaceDisplayWrite(MENU_EDITPROFILE_HOUR_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), menuBuffer); }
#define menuEditProfileWriteMinute() { sprintf(menuBuffer, MENU_MINUTE_FORMAT, s_minute); interfaceDisplayWrite(MENU_EDITPROFILE_MINUTE_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), menuBuffer); }
#define menuEditProfileWriteBellType() { interfaceDisplayWriteChar(MENU_EDITPROFILE_BELLTYPE_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), s_bellType ? MENU_CHAR_BREAK : MENU_CHAR_LESSON); }

// if number of bells is lower than maximum, user can add more ONLY if the last bell is set to earlier hour than 23 : 59
#define menuEditProfileCanAddMoreBells() (s_data.count < PROFILE_BELLS_COUNT && (s_data.count > 0 ? s_data.bells[s_data.count - 1] < 23 * 60 + 59 : 1))

static menu_t s_menu;

static uint8_t s_editingProfile;
static profile_t s_data;

static uint8_t s_mode;
static uint8_t s_currentHighlight;
static uint8_t s_hour;
static uint8_t s_minute;
static uint8_t s_bellType;

static const char* menuEditProfileGetString(uint8_t option, uint8_t* textWidth);
static uint8_t menuEditProfileParseOption(uint8_t option);

// menu option MUST BE in valid position
static void menuEditProfileEnterEditMode();

// if useSelectedHour == 1 then function will use s_hour to obtain min/max minute, otherwise it will use computed min/max hour
static void menuEditProfileComputeMinMaxInput(uint8_t bell, uint8_t useSelectedHour, uint8_t* minHour, uint8_t* maxHour, uint8_t* minMinute, uint8_t* maxMinute);

void menuEditProfile(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		internalStorageProfileRead(s_editingProfile, &s_data);

		menuCreate(&s_menu, MENU_EDITPROFILE_OPTIONS_COUNT, &menuEditProfileGetString, INTERFACE_MENU_LAST_TIMER);

		s_mode = MENU_EDITPROFILE_MODE_DEFAULT;
	}

	if (flags & INTERFACE_MENU_RETURNED)
	{
		if (s_mode == MENU_EDITPROFILE_MODE_CLEAR)
		{
			if (menuYesNoGet())
			{
				s_data.count = 0;
				menuSetOptionsCount(&s_menu, MENU_EDITPROFILE_OPTIONS_COUNT);
				menuSelectOption(&s_menu, 0);
			}

			s_mode = MENU_EDITPROFILE_MODE_DEFAULT;
		}
	}

	menuWrite(&s_menu);

	if (s_mode == MENU_EDITPROFILE_MODE_EDIT)
	{
		if (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS * 2) interfaceMenuTimerReset(0);

		menuEditProfileWriteHour(); // to redraw old data written by menuWrite(&s_menu)
		menuEditProfileWriteMinute(); // same as above
		menuEditProfileWriteBellType();

		switch (s_currentHighlight)
		{
		case MENU_EDITPROFILE_HIGHLIGHT_HOUR:
			if (menuEditProfileNeedBlink())
				interfaceDisplayWrite(MENU_EDITPROFILE_HOUR_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), "  ");
			break;

		case MENU_EDITPROFILE_HIGHLIGHT_MINUTE:
			if (menuEditProfileNeedBlink())
				interfaceDisplayWrite(MENU_EDITPROFILE_MINUTE_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), "  ");
			break;

		case MENU_EDITPROFILE_HIGHLIGHT_BELLTYPE:
			if (menuEditProfileNeedBlink())
				interfaceDisplayWrite(MENU_EDITPROFILE_BELLTYPE_HIGHLIGHT_X, menuGetCurrentOptionY(&s_menu), " ");
			break;
		}
	}

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		if (s_mode == MENU_EDITPROFILE_MODE_EDIT)
		{
			s_currentHighlight++;

			uint8_t minHour, maxHour, minMinute, maxMinute;
			menuEditProfileComputeMinMaxInput(menuGetCurrentOption(&s_menu), 1, &minHour, &maxHour, &minMinute, &maxMinute);
			valueClampRange(s_hour, minHour, maxHour);
			valueClampRange(s_minute, minMinute, maxMinute);

			if (s_currentHighlight >= MENU_EDITPROFILE_HIGHLIGHT_COUNT)
			{
				uint8_t option = menuGetCurrentOption(&s_menu);

				s_mode = MENU_EDITPROFILE_MODE_DEFAULT;
				s_data.bells[option] = s_hour * 60 + s_minute;
				s_data.bellTypes &= ~((uint64_t)1 << option);
				if (s_bellType) s_data.bellTypes |= ((uint64_t)1 << option);
			}
		}
		else
		{
			uint8_t parsedOption = menuEditProfileParseOption(menuGetCurrentOption(&s_menu));

			switch (parsedOption)
			{
			case MENU_EDITPROFILE_ANY_BELL:
				menuEditProfileEnterEditMode();
				break;

			case MENU_EDITPROFILE_ADD:
				if (menuEditProfileCanAddMoreBells())
				{
					s_data.bells[s_data.count] = 0xFFFF;
					s_data.count++;

					menuSetOptionsCount(&s_menu, MENU_EDITPROFILE_OPTIONS_COUNT);

					// enter edit mode and refresh menu
					menuEditProfileEnterEditMode();
				}
				break;

			case MENU_EDITPROFILE_CLEAR:
				s_mode = MENU_EDITPROFILE_MODE_CLEAR;
				menuYesNoSet(0, stringsDoYouWantToClearBells);
				interfaceMenuOpen(&menuYesNo);
				break;

			case MENU_EDITPROFILE_SAVE:
				controllerProfileUpdate(s_editingProfile, &s_data);
				interfaceMenuPrevious();
				break;

			case MENU_EDITPROFILE_CANCEL: interfaceMenuPrevious(); break;
			}
		}
	}
	else
	{
		if (s_mode == MENU_EDITPROFILE_MODE_EDIT)
		{
			uint8_t minHour, maxHour, minMinute, maxMinute;
			menuEditProfileComputeMinMaxInput(menuGetCurrentOption(&s_menu), 1, &minHour, &maxHour, &minMinute, &maxMinute);

			if (interfaceHandleButtonPulse(INTERFACE_BUTTON_UP))
			{
				switch (s_currentHighlight)
				{
				case MENU_EDITPROFILE_HIGHLIGHT_HOUR: valueIncRange(s_hour, minHour, maxHour); break;
				case MENU_EDITPROFILE_HIGHLIGHT_MINUTE: valueIncRange(s_minute, minMinute, maxMinute); break;
				case MENU_EDITPROFILE_HIGHLIGHT_BELLTYPE: s_bellType ^= 1; break;
				}
			}
			else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_DOWN))
			{
				switch (s_currentHighlight)
				{
				case MENU_EDITPROFILE_HIGHLIGHT_HOUR: valueDecRange(s_hour, minHour, maxHour); break;
				case MENU_EDITPROFILE_HIGHLIGHT_MINUTE: valueDecRange(s_minute, minMinute, maxMinute); break;
				case MENU_EDITPROFILE_HIGHLIGHT_BELLTYPE: s_bellType ^= 1; break;
				}
			}
		}
		else
		{
			if (interfaceHandleButtonPulse(INTERFACE_BUTTON_UP))
			{
				menuPreviousOption(&s_menu);
			}
			else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_DOWN))
			{
				menuNextOption(&s_menu);
			}
		}
	}
}

void menuEditProfileSet(uint8_t profile)
{
	s_editingProfile = profile;
}

static const char* menuEditProfileGetString(uint8_t option, uint8_t* textWidth)
{
	uint8_t hour, minute;
	switch(menuEditProfileParseOption(option))
	{
	case MENU_EDITPROFILE_ANY_BELL:
		hour = s_data.bells[option] / 60;
		minute = s_data.bells[option] % 60;
		char bellTypeChar = (s_data.bellTypes & ((uint64_t)1 << option)) ? MENU_CHAR_BREAK : MENU_CHAR_LESSON;
		sprintf(menuBuffer, " "MENU_EDITPROFILE_NUMBER_FORMAT" - "MENU_HOUR_FORMAT" "MENU_TIME_SEPARATOR_STR" "MENU_MINUTE_FORMAT" %c", option + 1, hour, minute, bellTypeChar);
		return menuBuffer;

	case MENU_EDITPROFILE_ADD: stringsLoad(stringsBuffer, stringsAddBell); break;
	case MENU_EDITPROFILE_CLEAR: stringsLoad(stringsBuffer, stringsClear); break;
	case MENU_EDITPROFILE_SAVE: stringsLoad(stringsBuffer, stringsSave); break;
	case MENU_EDITPROFILE_CANCEL: stringsLoad(stringsBuffer, stringsCancel); break;
	}

	return stringsBuffer;
}
static uint8_t menuEditProfileParseOption(uint8_t option)
{
	uint8_t count = menuGetOptionsCount(&s_menu);

	if (option == count - 1)
		return MENU_EDITPROFILE_CANCEL;
	else if (option == count - 2)
		return MENU_EDITPROFILE_SAVE;
	else if (option == count - 3)
		return MENU_EDITPROFILE_CLEAR;
	else if (option == count - 4)
		return MENU_EDITPROFILE_ADD;
	else
		return MENU_EDITPROFILE_ANY_BELL;
}
static void menuEditProfileEnterEditMode()
{
	s_currentHighlight = MENU_EDITPROFILE_HIGHLIGHT_HOUR;
	s_mode = MENU_EDITPROFILE_MODE_EDIT;

	uint8_t option = menuGetCurrentOption(&s_menu);

	uint16_t value = s_data.bells[menuGetCurrentOption(&s_menu)];
	if (value == 0xFFFF) // means new bell
	{
		uint8_t minHour, maxHour, minMinute, maxMinute;
		menuEditProfileComputeMinMaxInput(menuGetCurrentOption(&s_menu), 0, &minHour, &maxHour, &minMinute, &maxMinute);
		s_hour = minHour;
		s_minute = minMinute;
		s_data.bells[option] = s_hour * 60 + s_minute;
		if (option == 0)
		{
			s_bellType = 0;
		}
		else
		{
			s_bellType = (s_data.bellTypes & ((uint64_t)1 << (option - 1))) ? 0 : 1;
		}
	}
	else
	{
		s_hour = value / 60;
		s_minute = value % 60;
		s_bellType = (s_data.bellTypes & ((uint64_t)1 << option)) >> option;
	}

	interfaceMenuTimerReset(0);
}
static void menuEditProfileComputeMinMaxInput(uint8_t bell, uint8_t useSelectedHour, uint8_t* minHour, uint8_t* maxHour, uint8_t* minMinute, uint8_t* maxMinute)
{
	*minHour = MENU_HOUR_MIN;
	*minMinute = MENU_MINUTE_MIN;
	*maxHour = MENU_HOUR_MAX;
	*maxMinute = MENU_MINUTE_MAX;

	if (bell > 0) // 0 has no previous bell
	{
		uint16_t previousValue = s_data.bells[bell - 1];
		uint8_t previousHour = previousValue / 60;
		uint8_t previousMinute = previousValue % 60;

		*minHour = (previousMinute < 59 ? previousHour : previousHour + 1);
		uint8_t compareHour = (useSelectedHour ? s_hour : *minHour);
		*minMinute = (compareHour == previousHour ? previousMinute + 1 : MENU_MINUTE_MIN);
	}

	if (bell + 1 < s_data.count) // last bell has no next bell
	{
		uint16_t nextValue = s_data.bells[bell + 1];
		uint8_t nextHour = nextValue / 60;
		uint8_t nextMinute = nextValue % 60;

		*maxHour = (nextMinute > 0 ? nextHour : nextHour - 1);
		uint8_t compareHour = (useSelectedHour ? s_hour : *maxHour);
		*maxMinute = (compareHour == nextHour ? nextMinute - 1 : MENU_MINUTE_MAX);
	}
}
