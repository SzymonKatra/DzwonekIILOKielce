#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>
#include "../common.h"
#include "../customChars.h"
#include "../interface.h"

#define MENU_BUFFER_SIZE 64
#define MENU_OPTIONS_BUFFER_SIZE (INTERFACE_DISPLAY_WIDTH + 1)

#define MENU_HIGHLIGHT_TOGGLE_TICKS (TICKS_PER_SECOND / 2)
#define MENU_OPTION_DEFAULT_WIDTH (INTERFACE_DISPLAY_WIDTH - 1)
#define MENU_OPTION_PROGRESS_ENABLE_TICKS (3 * TICKS_PER_SECOND / 4)
#define MENU_OPTION_PROGRESS_TICKS (2 * TICKS_PER_SECOND / 5)

#define MENU_DAY_MIN 1
#define MENU_DAY_MAX(month, year) monthDays(month, year)
#define MENU_MONTH_MIN 1
#define MENU_MONTH_MAX 12
#define MENU_YEAR_MIN 1900
#define MENU_YEAR_MAX 2099

#define MENU_HOUR_MIN 0
#define MENU_HOUR_MAX 23
#define MENU_MINUTE_MIN 0
#define MENU_MINUTE_MAX 59

#define MENU_SELECT_CHAR CUSTOMCHARS_MENU_POINTER_ANSI // 0x7E is another nice arrow pointer
#define MENU_TIME_SEPARATOR ':'
#define MENU_TIME_SEPARATOR_STR ":"
#define MENU_DATE_SEPARATOR '-'
#define MENU_DATE_SEPARATOR_STR "-"
#define MENU_HOUR_FORMAT "%02d"
#define MENU_MINUTE_FORMAT "%02d"
#define MENU_SECOND_FORMAT "%02d"
#define MENU_DAY_FORMAT "%02d"
#define MENU_MONTH_FORMAT "%02d"
#define MENU_YEAR_FORMAT "%04d"
#define MENU_TEMPERATURE_FORMAT "%3d"

#define MENU_PROFILE_NORMAL_CHAR 'N'
#define MENU_PROFILE_SHORTENED_CHAR 'S'
#define MENU_PROFILE_NONE_CHAR CUSTOMCHARS_DOT_CENTER_ANSI

#define MENU_PROFILE_NORMAL_NUMBER 0
#define MENU_PROFILE_SHORTENED_NUMBER 1
#define MENU_PROFILE_SYNONYMS_COUNT 2

#define MENU_CHAR_YES 'T'
#define MENU_CHAR_NO 'N'

#define MENU_CHAR_BREAK 'P'
#define MENU_CHAR_LESSON 'L'

// returns pointer to string buffer
// (uint8_t option, uint8_t* textWidth)
// don't change textWidth if you don't want to write other text on the right side of display
typedef const char*(getStringInfoFunction_f)(uint8_t, uint8_t*);

typedef struct
{
	getStringInfoFunction_f* getStringInfo;
	uint8_t optionsCount;
	uint8_t currentOption;
	uint8_t progressOption;
	uint8_t progress;
	uint8_t timer;
}menu_t;

extern char menuBuffer[];

void menuCreate(menu_t* result, uint8_t optionsCount, getStringInfoFunction_f getString, uint8_t timer);
void menuWrite(menu_t* menu);
uint8_t menuGetOptionY(const menu_t* menu, uint8_t option); // returns 0xFF if invalid option is specified for current state of menu
uint8_t menuGetCurrentOptionY(const menu_t* menu);
uint8_t menuNextOption(menu_t* menu);
uint8_t menuPreviousOption(menu_t* menu);
void menuSelectOption(menu_t* menu, uint8_t option);
void menuSelectFirstOption(menu_t* menu);
void menuSelectLastOption(menu_t* menu);
uint8_t menuGetCurrentOption(const menu_t* menu);
uint8_t menuGetOptionsCount(const menu_t* menu);
void menuSetOptionsCount(menu_t* menu, uint8_t optionsCount);
void menuResetProgress(menu_t* menu);

void menuProgressAnyText(const char* inputBuffer, char* outputBuffer, uint8_t textWidth, uint8_t timer, uint8_t* progress);

const char* menuGetDayOfWeekName(uint8_t dayOfWeek);
char menuGetProfileChar(uint8_t profile);

#endif /* MENU_H_ */
