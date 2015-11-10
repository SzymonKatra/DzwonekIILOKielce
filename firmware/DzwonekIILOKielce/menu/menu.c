#include "menu.h"

char menuBuffer[MENU_BUFFER_SIZE];

static char menuOptionBuffer[MENU_OPTIONS_BUFFER_SIZE];

static void menuWriteOption(uint8_t x, uint8_t y, uint8_t option, menu_t* menu);
static void menuWriteProgressOption(uint8_t x, uint8_t y, uint8_t option, menu_t* menu);

void menuCreate(menu_t* result, uint8_t optionsCount, getStringInfoFunction_f getString, uint8_t timer)
{
	result->currentOption = 0;
	result->progress = 0;
	result->progressOption = 0xFF;
	result->optionsCount = optionsCount;
	result->getStringInfo = getString;
	result->timer = timer;
}
void menuWrite(menu_t* menu)
{
	uint8_t optionTop, optionBottom;

	if (menu->optionsCount == 1)
	{
		interfaceDisplayWriteChar(0, 0, MENU_SELECT_CHAR);
		interfaceDisplayWriteChar(0, 1, ' ');

		optionTop = 0;
		optionBottom = 0xFF;
	}
	else if (menu->currentOption == menu->optionsCount - 1)
	{
		interfaceDisplayWriteChar(0, 0, ' ');
		interfaceDisplayWriteChar(0, 1, MENU_SELECT_CHAR);

		optionTop = menu->currentOption - 1;
		optionBottom = menu->currentOption;
	}
	else
	{
		interfaceDisplayWriteChar(0, 0, MENU_SELECT_CHAR);
		interfaceDisplayWriteChar(0, 1, ' ');

		optionTop = menu->currentOption;
		optionBottom = menu->currentOption + 1;
	}

	if (optionTop == menu->currentOption)
		menuWriteProgressOption(1, 0, optionTop, menu);
	else menuWriteOption(1, 0, optionTop, menu);

	if(optionBottom != 0xFF)
	{
		if (optionBottom == menu->currentOption)
			menuWriteProgressOption(1, 1, optionBottom, menu);
		else menuWriteOption(1, 1, optionBottom, menu);
	}
	else interfaceDisplayWrite(1, 1, "               "); // 15 spaces to clear
}
uint8_t menuGetOptionY(const menu_t* menu, uint8_t option)
{
	if (option == menu->currentOption)
	{
		if (option == menu->optionsCount - 1)
			return 1;
		else return 0;
	}
	else if (option == menu->currentOption + 1)
	{
		return 1;
	}
	else if (option == menu->currentOption - 1 && menu->currentOption == menu->optionsCount - 1)
	{
		return 0;
	}

	return 0xFF;
}
uint8_t menuGetCurrentOptionY(const menu_t* menu)
{
	if (menu->optionsCount == 1) return 0;
	else if (menu->currentOption == menu->optionsCount - 1) return 1;
	else return 0;
}
uint8_t menuNextOption(menu_t* menu)
{
	menu->currentOption++;
	if (menu->currentOption >= menu->optionsCount)
	{
		menu->currentOption = 0;
		return 1;
	}
	return 0;
}
uint8_t menuPreviousOption(menu_t* menu)
{
	if (menu->currentOption == 0)
	{
		menu->currentOption = menu->optionsCount - 1;
		return 1;
	}
	else
	{
		menu->currentOption--;
		return 0;
	}
}
void menuSelectOption(menu_t* menu, uint8_t option)
{
	menu->currentOption = option;
}
void menuSelectFirstOption(menu_t* menu)
{
	menu->currentOption = 0;
}
void menuSelectLastOption(menu_t* menu)
{
	menu->currentOption = menu->optionsCount - 1;
}
uint8_t menuGetCurrentOption(const menu_t* menu)
{
	return menu->currentOption;
}
uint8_t menuGetOptionsCount(const menu_t* menu)
{
	return menu->optionsCount;
}
void menuSetOptionsCount(menu_t* menu, uint8_t optionsCount)
{
	menu->optionsCount = optionsCount;
	if (menu->currentOption >= menu->optionsCount) menu->currentOption = menu->optionsCount - 1;
}
void menuResetProgress(menu_t* menu)
{
	menu->progressOption = 0xFF;
}

static void menuWriteOption(uint8_t x, uint8_t y, uint8_t option, menu_t* menu)
{
	uint8_t textWidth = MENU_OPTION_DEFAULT_WIDTH;
	const char* inputBuffer = menu->getStringInfo(option, &textWidth);
	uint8_t i = 0;
	for (; i < textWidth; i++)
	{
		if (inputBuffer[i] == '\0') break;
		menuOptionBuffer[i] = inputBuffer[i];
	}
	for(;i < MENU_OPTION_DEFAULT_WIDTH; i++) menuOptionBuffer[i] = ' ';
	menuOptionBuffer[i] = '\0';
	interfaceDisplayWrite(x, y, menuOptionBuffer);
}
static void menuWriteProgressOption(uint8_t x, uint8_t y, uint8_t option, menu_t* menu)
{
	if (option != menu->progressOption)
	{
		menu->progressOption = option;
		menu->progress = 0;
		interfaceMenuTimerReset(menu->timer);
	}

	uint8_t textWidth = MENU_OPTION_DEFAULT_WIDTH;
	const char* inputBuffer = menu->getStringInfo(option, &textWidth);

	menuProgressAnyText(inputBuffer, menuOptionBuffer, textWidth, menu->timer, &(menu->progress));
	interfaceDisplayWrite(x, y, menuOptionBuffer);

	/*
	uint8_t outputIndex = 0;
	uint8_t inputIndex = menu->progress;

	while (outputIndex < textWidth && inputBuffer[inputIndex] != '\0')
	{
		menuOptionBuffer[outputIndex] = inputBuffer[inputIndex];

		outputIndex++;
		inputIndex++;
	}
	// when control leave this loop, end of string is signaled by inputBuffer[inputIndex] == '\0'

	// assuming MENU_OPTION_PROGRESS_ENABLE_TICKS > MENU_OPTION_PROGRESS_TICKS
	// when string is starting progressing or reached end, wait more time - MENU_OPTION_PROGRESS_ENABLE_TICKS, otherwise wait MENU_OPTION_PROGRESS_TICKS
	uint8_t progressWait = (menu->progress == 0 || inputBuffer[inputIndex] == '\0' ? MENU_OPTION_PROGRESS_ENABLE_TICKS : MENU_OPTION_PROGRESS_TICKS);

	if (interfaceMenuTimerGet(menu->timer) >= progressWait)
	{
		// if there isn't end of string start from next char in the next write
		// otherwise, start from beginning
		if (inputBuffer[inputIndex] != '\0') menu->progress++; else menu->progress = 0;
		interfaceMenuTimerReset(menu->timer);
	}

	while (outputIndex < MENU_OPTION_DEFAULT_WIDTH) menuOptionBuffer[outputIndex++] = ' ';
	menuOptionBuffer[outputIndex] = '\0';*/

}

void menuProgressAnyText(const char* inputBuffer, char* outputBuffer, uint8_t textWidth, uint8_t timer, uint8_t* progress)
{
	uint8_t outputIndex = 0;
	uint8_t inputIndex = *progress;

	while (outputIndex < textWidth && inputBuffer[inputIndex] != '\0')
	{
		outputBuffer[outputIndex] = inputBuffer[inputIndex];

		outputIndex++;
		inputIndex++;
	}
	// when control leave this loop, end of string is signaled by inputBuffer[inputIndex] == '\0'

	// assuming MENU_OPTION_PROGRESS_ENABLE_TICKS > MENU_OPTION_PROGRESS_TICKS
	// when string is starting progressing or reached end, wait more time - MENU_OPTION_PROGRESS_ENABLE_TICKS, otherwise wait MENU_OPTION_PROGRESS_TICKS
	uint8_t progressWait = (*progress == 0 || inputBuffer[inputIndex] == '\0' ? MENU_OPTION_PROGRESS_ENABLE_TICKS : MENU_OPTION_PROGRESS_TICKS);

	if (interfaceMenuTimerGet(timer) >= progressWait)
	{
		// if there isn't end of string start from next char in the next write
		// otherwise, start from beginning
		if (inputBuffer[inputIndex] != '\0') (*progress)++; else *progress = 0;
		interfaceMenuTimerReset(timer);
	}

	while (outputIndex < MENU_OPTION_DEFAULT_WIDTH) outputBuffer[outputIndex++] = ' ';
	outputBuffer[outputIndex] = '\0';
}

const char* menuGetDayOfWeekName(uint8_t dayOfWeek)
{
	switch(dayOfWeek)
	{
	case MONDAY: return "Pn";
	case TUESDAY: return "Wt";
	case WEDNESDAY: return "Œr";
	case THURSDAY: return "Cz";
	case FRIDAY: return "Pt";
	case SATURDAY: return "So";
	case SUNDAY: return "Nd";
	}
	return "\0";
}
char menuGetProfileChar(uint8_t profile)
{
	if (profile == MENU_PROFILE_NORMAL_NUMBER)
		return MENU_PROFILE_NORMAL_CHAR;
	else if (profile == MENU_PROFILE_SHORTENED_NUMBER)
		return MENU_PROFILE_SHORTENED_CHAR;
	else if (profile == 0xFF)
		return MENU_PROFILE_NONE_CHAR;
	else return (profile - MENU_PROFILE_SYNONYMS_COUNT + 1) + '0';
}
