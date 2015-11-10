#ifndef MENU_SDCARD_H_
#define MENU_SDCARD_H_

#include <stdint.h>

#define MENU_SDCARD_MODE_LOAD 0
#define MENU_SDCARD_MODE_SAVE 1

#define MENU_SDCARD_INITIALIZE_TWO_CHANCES 1

#define MENU_SDCARD_PERCENTAGE_FORMAT "%3d %%"
#define MENU_SDCARD_PROGRESS_BORDER_CHAR '|'
#define MENU_SDCARD_PROGRESS_CHAR '='

void menuSdCard(uint8_t flags);
void menuSdCardSet(uint8_t mode);

#endif /* MENU_SDCARD_H_ */
