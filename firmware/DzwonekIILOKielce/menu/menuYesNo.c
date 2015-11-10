#include "menuYesNo.h"

#include "../interface.h"
#include "../strings.h"
#include "menu.h"

static uint8_t s_currentOption;
static const char* s_textProgmem;
static uint8_t s_textProgress;

#define menuYesNoNeedBlink() (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS)
#define menuYesNoWriteYes() { stringsLoad(menuBuffer, stringsYes); interfaceDisplayWrite(MENU_YESNO_YES_X, MENU_YESNO_YES_Y, menuBuffer); }
#define menuYesNoWriteNo() { stringsLoad(menuBuffer, stringsNo); interfaceDisplayWrite(MENU_YESNO_NO_X, MENU_YESNO_NO_Y, menuBuffer); }

void menuYesNo(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_textProgress = 0;
	}

	menuYesNoWriteYes();
	menuYesNoWriteNo();

	switch (s_currentOption)
	{
	case MENU_YESNO_YES:
		if (menuYesNoNeedBlink())
			interfaceDisplayWrite(MENU_YESNO_YES_X, MENU_YESNO_YES_Y, "   ");
		break;

	case MENU_YESNO_NO:
		if (menuYesNoNeedBlink())
			interfaceDisplayWrite(MENU_YESNO_NO_X, MENU_YESNO_NO_Y, "   ");
		break;
	}

	stringsLoad(stringsBuffer, s_textProgmem);
	menuProgressAnyText(stringsBuffer, menuBuffer, INTERFACE_DISPLAY_WIDTH, INTERFACE_MENU_LAST_TIMER, &s_textProgress);
	interfaceDisplayWrite(MENU_YESNO_TEXT_X, MENU_YESNO_TEXT_Y, menuBuffer);

	if (interfaceHandleButtonPressed(INTERFACE_BUTTON_SELECT))
	{
		interfaceMenuPrevious();
	}
	else if (interfaceHandleButtonPressed(INTERFACE_BUTTON_UP) || interfaceHandleButtonPressed(INTERFACE_BUTTON_DOWN))
	{
		if (s_currentOption == MENU_YESNO_YES)
			s_currentOption = MENU_YESNO_NO;
		else s_currentOption = MENU_YESNO_YES;
	}

	if (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS * 2)
	{
		interfaceMenuTimerReset(0);
	}
}

void menuYesNoSet(uint8_t defaultOption, const char* textProgmem)
{
	s_currentOption = defaultOption;
	if (s_currentOption > 1) s_currentOption = 1;

	s_textProgmem = textProgmem;
}

uint8_t menuYesNoGet()
{
	return s_currentOption;
}
