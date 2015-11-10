#ifndef MENU_SETTIME_H_
#define MENU_SETTIME_H_

#include <stdint.h>

#define MENU_SETTIME_HOUR_X 4
#define MENU_SETTIME_HOUR_Y 1
#define MENU_SETTIME_MINUTE_X 9
#define MENU_SETTIME_MINUTE_Y 1

#define MENU_SETTIME_HOUR_MINUTE_SEPARATOR_X 7
#define MENU_SETTIME_HOUR_MINUTE_SEPARATOR_Y 1

#define MENU_SETTIME_HOUR_FORMAT_X 4
#define MENU_SETTIME_HOUR_FORMAT_Y 0
#define MENU_SETTIME_MINUTE_FORMAT_X 9
#define MENU_SETTIME_MINUTE_FORMAT_Y 0

#define MENU_SETTIME_HOUR_MINUTE_FORMAT_SEPARATOR_X 7
#define MENU_SETTIME_HOUR_MINUTE_FORMAT_SEPARATOR_Y 0

void menuSetTime(uint8_t flags);

#endif /* MENU_SETTIME_H_ */
