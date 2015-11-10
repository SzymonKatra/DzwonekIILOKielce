#ifndef MENU_EDITPROFILE_H_
#define MENU_EDITPROFILE_H_

#include <stdint.h>

#define MENU_EDITPROFILE_NUMBER_FORMAT "%2d"

#define MENU_EDITPROFILE_HOUR_HIGHLIGHT_X 7
#define MENU_EDITPROFILE_MINUTE_HIGHLIGHT_X 12
#define MENU_EDITPROFILE_BELLTYPE_HIGHLIGHT_X 15

void menuEditProfile(uint8_t flags);

void menuEditProfileSet(uint8_t profile);

#endif /* MENU_EDITPROFILE_H_ */
