#ifndef CUSTOMCHARS_H_
#define CUSTOMCHARS_H_

#include <stdint.h>

// chars id's
#define CUSTOMCHARS_POLISH_A_BIG 1
#define CUSTOMCHARS_POLISH_C_BIG 2
#define CUSTOMCHARS_POLISH_E_BIG 3
#define CUSTOMCHARS_POLISH_L_BIG 4
#define CUSTOMCHARS_POLISH_N_BIG 5
#define CUSTOMCHARS_POLISH_O_BIG 6
#define CUSTOMCHARS_POLISH_S_BIG 7
#define CUSTOMCHARS_POLISH_X_BIG 8
#define CUSTOMCHARS_POLISH_Z_BIG 9

#define CUSTOMCHARS_POLISH_A_SMALL 10
#define CUSTOMCHARS_POLISH_C_SMALL 11
#define CUSTOMCHARS_POLISH_E_SMALL 12
#define CUSTOMCHARS_POLISH_L_SMALL 13
#define CUSTOMCHARS_POLISH_N_SMALL 14
#define CUSTOMCHARS_POLISH_O_SMALL 15
#define CUSTOMCHARS_POLISH_S_SMALL 16
#define CUSTOMCHARS_POLISH_X_SMALL 17
#define CUSTOMCHARS_POLISH_Z_SMALL 18

#define CUSTOMCHARS_LEFT_SQUARE_BRACKET_FILLED 19
#define CUSTOMCHARS_RIGHT_SQUARE_BRACKET_FILLED 20
#define CUSTOMCHARS_VERTICAL_THICK_LINE 21
#define CUSTOMCHARS_MENU_POINTER 22
#define CUSTOMCHARS_HOURGLASS 23
#define CUSTOMCHARS_BELL 24
#define CUSTOMCHARS_ANTENNA 25
#define CUSTOMCHARS_FINDING_UP 26
#define CUSTOMCHARS_FINDING_DOWN 27
#define CUSTOMCHARS_ANTENNA_RECEIVED 28
#define CUSTOMCHARS_PROGRESS 29
#define CUSTOMCHARS_DOT_CENTER 30

// chars ansi codes must be > 128 to be detected by interface
#define CUSTOMCHARS_POLISH_A_BIG_ANSI '�'
#define CUSTOMCHARS_POLISH_C_BIG_ANSI '�'
#define CUSTOMCHARS_POLISH_E_BIG_ANSI '�'
#define CUSTOMCHARS_POLISH_L_BIG_ANSI '�'
#define CUSTOMCHARS_POLISH_N_BIG_ANSI '�'
#define CUSTOMCHARS_POLISH_O_BIG_ANSI '�'
#define CUSTOMCHARS_POLISH_S_BIG_ANSI '�'
#define CUSTOMCHARS_POLISH_X_BIG_ANSI '�'
#define CUSTOMCHARS_POLISH_Z_BIG_ANSI '�'

#define CUSTOMCHARS_POLISH_A_SMALL_ANSI '�'
#define CUSTOMCHARS_POLISH_C_SMALL_ANSI '�'
#define CUSTOMCHARS_POLISH_E_SMALL_ANSI '�'
#define CUSTOMCHARS_POLISH_L_SMALL_ANSI '�'
#define CUSTOMCHARS_POLISH_N_SMALL_ANSI '�'
#define CUSTOMCHARS_POLISH_O_SMALL_ANSI '�'
#define CUSTOMCHARS_POLISH_S_SMALL_ANSI '�'
#define CUSTOMCHARS_POLISH_X_SMALL_ANSI '�'
#define CUSTOMCHARS_POLISH_Z_SMALL_ANSI '�'

#define CUSTOMCHARS_LEFT_SQUARE_BRACKET_FILLED_ANSI 0x81
#define CUSTOMCHARS_RIGHT_SQUARE_BRACKET_FILLED_ANSI 0x83
#define CUSTOMCHARS_VERTICAL_THICK_LINE_ANSI 0x88
#define CUSTOMCHARS_MENU_POINTER_ANSI 0x90
#define CUSTOMCHARS_HOURGLASS_ANSI 0x98
#define CUSTOMCHARS_BELL_ANSI 0x91
#define CUSTOMCHARS_ANTENNA_ANSI 0x92
#define CUSTOMCHARS_FINDING_UP_ANSI 0x93
#define CUSTOMCHARS_FINDING_DOWN_ANSI 0x94
#define CUSTOMCHARS_ANTENNA_RECEIVED_ANSI 0x95
#define CUSTOMCHARS_PROGRESS_ANSI 0x96
#define CUSTOMCHARS_DOT_CENTER_ANSI 0x97

uint8_t customCharsAnsiToId(char ansi);

const uint8_t* customCharsGetIdBuffer(uint8_t id);

void customCharsUpdateProgress(uint8_t progress[8]);

#endif /* CUSTOMCHARS_H_ */
