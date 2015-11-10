#ifndef MENU_YESNO_H_
#define MENU_YESNO_H_

#include <stdint.h>

#define MENU_YESNO_YES 1
#define MENU_YESNO_NO 0

#define MENU_YESNO_TEXT_X 0
#define MENU_YESNO_TEXT_Y 0

#define MENU_YESNO_YES_X 3
#define MENU_YESNO_YES_Y 1

#define MENU_YESNO_NO_X 10
#define MENU_YESNO_NO_Y 1

void menuYesNo(uint8_t flags);
void menuYesNoSet(uint8_t defaultOption, const char* textProgmem);
uint8_t menuYesNoGet();

#endif /* MENU_YESNO_H_ */
