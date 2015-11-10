#include "customChars.h"

static const uint8_t s_polishABig[8] = { 0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x02 }; // •
static const uint8_t s_polishCBig[8] = { 0x02, 0x0E, 0x15, 0x10, 0x10, 0x11, 0x0E, 0x00 }; // ∆
static const uint8_t s_polishEBig[8] = { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F, 0x02 }; //  
static const uint8_t s_polishLBig[8] = { 0x10, 0x12, 0x14, 0x18, 0x10, 0x10, 0x1F, 0x00 }; // £
static const uint8_t s_polishNBig[8] = { 0x02, 0x15, 0x19, 0x15, 0x13, 0x11, 0x11, 0x00 }; // —
static const uint8_t s_polishOBig[8] = { 0x02, 0x0E, 0x15, 0x11, 0x11, 0x11, 0x0E, 0x00 }; // ”
static const uint8_t s_polishSBig[8] = { 0x02, 0x0F, 0x10, 0x0E, 0x01, 0x01, 0x1E, 0x00 }; // å
static const uint8_t s_polishXBig[8] = { 0x04, 0x1F, 0x09, 0x02, 0x04, 0x08, 0x1F, 0x00 }; // è
static const uint8_t s_polishZBig[8] = { 0x1F, 0x01, 0x02, 0x1F, 0x08, 0x10, 0x1F, 0x00 }; // Ø

static const uint8_t s_polishASmall[8] = { 0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x04 }; // π
static const uint8_t s_polishCSmall[8] = { 0x02, 0x04, 0x0E, 0x10, 0x10, 0x11, 0x0E, 0x00 }; // Ê
static const uint8_t s_polishESmall[8] = { 0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x04 }; // Í
static const uint8_t s_polishLSmall[8] = { 0x0C, 0x04, 0x06, 0x04, 0x0C, 0x04, 0x0E, 0x00 }; // ≥
static const uint8_t s_polishNSmall[8] = { 0x02, 0x04, 0x1E, 0x11, 0x11, 0x11, 0x11, 0x00 }; // Ò
static const uint8_t s_polishOSmall[8] = { 0x02, 0x04, 0x0E, 0x11, 0x11, 0x11, 0x0E, 0x00 }; // Û
static const uint8_t s_polishSSmall[8] = { 0x02, 0x04, 0x0E, 0x10, 0x0E, 0x01, 0x1E, 0x00 }; // ú
static const uint8_t s_polishXSmall[8] = { 0x02, 0x04, 0x1F, 0x02, 0x04, 0x08, 0x1F, 0x00 }; // ü
static const uint8_t s_polishZSmall[8] = { 0x04, 0x00, 0x1F, 0x02, 0x04, 0x08, 0x1F, 0x00 }; // ø

static const uint8_t s_leftSquareBracketFilled[8] = { 0x0E, 0x08, 0x0B, 0x0B, 0x0B, 0x08, 0x0E, 0x00 };
static const uint8_t s_rightSquareBracketFilled[8] = { 0x0E, 0x02, 0x1A, 0x1A, 0x1A, 0x02, 0x0E, 0x00 };
static const uint8_t s_verticalThickLine[8] = { 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E };
static const uint8_t s_menuPointer[8] = { 0x0C ,0x06, 0x03, 0x1F, 0x1F, 0x03, 0x06, 0x0C };
static const uint8_t s_hourglass[8] = { 0x1F, 0x11, 0x0E, 0x04, 0x0A, 0x1F, 0x1F, 0x00 };
static const uint8_t s_bell[8] = { 0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x04, 0x02, 0x00 };
static const uint8_t s_antenna[8] = { 0x15, 0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00 };
static const uint8_t s_findingUp[8] = { 0x00, 0x04, 0x0E, 0x04, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t s_findingDown[8] = { 0x00, 0x00, 0x00, 0x00, 0x04, 0x0E, 0x04, 0x00 };
static const uint8_t s_antennaReceived[8] = { 0x15, 0x15, 0x15, 0x0E, 0x04, 0x04, 0x04, 0x00 };
static uint8_t s_progress[8];
static const uint8_t s_dotCenter[8] = {0x00, 0x00, 0x0E, 0x0E, 0x0E, 0x00, 0x00, 0x00 };

uint8_t customCharsAnsiToId(char ansi)
{
	switch (ansi)
	{
	case CUSTOMCHARS_POLISH_A_BIG_ANSI: return CUSTOMCHARS_POLISH_A_BIG;
	case CUSTOMCHARS_POLISH_C_BIG_ANSI: return CUSTOMCHARS_POLISH_C_BIG;
	case CUSTOMCHARS_POLISH_E_BIG_ANSI: return CUSTOMCHARS_POLISH_E_BIG;
	case CUSTOMCHARS_POLISH_L_BIG_ANSI: return CUSTOMCHARS_POLISH_L_BIG;
	case CUSTOMCHARS_POLISH_N_BIG_ANSI: return CUSTOMCHARS_POLISH_N_BIG;
	case CUSTOMCHARS_POLISH_O_BIG_ANSI: return CUSTOMCHARS_POLISH_O_BIG;
	case CUSTOMCHARS_POLISH_S_BIG_ANSI: return CUSTOMCHARS_POLISH_S_BIG;
	case CUSTOMCHARS_POLISH_X_BIG_ANSI: return CUSTOMCHARS_POLISH_X_BIG;
	case CUSTOMCHARS_POLISH_Z_BIG_ANSI: return CUSTOMCHARS_POLISH_Z_BIG;

	case CUSTOMCHARS_POLISH_A_SMALL_ANSI: return CUSTOMCHARS_POLISH_A_SMALL;
	case CUSTOMCHARS_POLISH_C_SMALL_ANSI: return CUSTOMCHARS_POLISH_C_SMALL;
	case CUSTOMCHARS_POLISH_E_SMALL_ANSI: return CUSTOMCHARS_POLISH_E_SMALL;
	case CUSTOMCHARS_POLISH_L_SMALL_ANSI: return CUSTOMCHARS_POLISH_L_SMALL;
	case CUSTOMCHARS_POLISH_N_SMALL_ANSI: return CUSTOMCHARS_POLISH_N_SMALL;
	case CUSTOMCHARS_POLISH_O_SMALL_ANSI: return CUSTOMCHARS_POLISH_O_SMALL;
	case CUSTOMCHARS_POLISH_S_SMALL_ANSI: return CUSTOMCHARS_POLISH_S_SMALL;
	case CUSTOMCHARS_POLISH_X_SMALL_ANSI: return CUSTOMCHARS_POLISH_X_SMALL;
	case CUSTOMCHARS_POLISH_Z_SMALL_ANSI: return CUSTOMCHARS_POLISH_Z_SMALL;

	case CUSTOMCHARS_LEFT_SQUARE_BRACKET_FILLED_ANSI: return CUSTOMCHARS_LEFT_SQUARE_BRACKET_FILLED;
	case CUSTOMCHARS_RIGHT_SQUARE_BRACKET_FILLED_ANSI: return CUSTOMCHARS_RIGHT_SQUARE_BRACKET_FILLED;
	case CUSTOMCHARS_VERTICAL_THICK_LINE_ANSI: return CUSTOMCHARS_VERTICAL_THICK_LINE;
	case CUSTOMCHARS_MENU_POINTER_ANSI: return CUSTOMCHARS_MENU_POINTER;
	case CUSTOMCHARS_HOURGLASS_ANSI: return CUSTOMCHARS_HOURGLASS;
	case CUSTOMCHARS_BELL_ANSI: return CUSTOMCHARS_BELL;
	case CUSTOMCHARS_ANTENNA_ANSI: return CUSTOMCHARS_ANTENNA;
	case CUSTOMCHARS_FINDING_UP_ANSI: return CUSTOMCHARS_FINDING_UP;
	case CUSTOMCHARS_FINDING_DOWN_ANSI: return CUSTOMCHARS_FINDING_DOWN;
	case CUSTOMCHARS_ANTENNA_RECEIVED_ANSI: return CUSTOMCHARS_ANTENNA_RECEIVED;
	case CUSTOMCHARS_PROGRESS_ANSI: return CUSTOMCHARS_PROGRESS;
	case CUSTOMCHARS_DOT_CENTER_ANSI: return CUSTOMCHARS_DOT_CENTER;
	}

	return 0;
}

const uint8_t* customCharsGetIdBuffer(uint8_t id)
{
	switch(id)
	{
	case CUSTOMCHARS_POLISH_A_BIG: return s_polishABig;
	case CUSTOMCHARS_POLISH_C_BIG: return s_polishCBig;
	case CUSTOMCHARS_POLISH_E_BIG: return s_polishEBig;
	case CUSTOMCHARS_POLISH_L_BIG: return s_polishLBig;
	case CUSTOMCHARS_POLISH_N_BIG: return s_polishNBig;
	case CUSTOMCHARS_POLISH_O_BIG: return s_polishOBig;
	case CUSTOMCHARS_POLISH_S_BIG: return s_polishSBig;
	case CUSTOMCHARS_POLISH_X_BIG: return s_polishXBig;
	case CUSTOMCHARS_POLISH_Z_BIG: return s_polishZBig;

	case CUSTOMCHARS_POLISH_A_SMALL: return s_polishASmall;
	case CUSTOMCHARS_POLISH_C_SMALL: return s_polishCSmall;
	case CUSTOMCHARS_POLISH_E_SMALL: return s_polishESmall;
	case CUSTOMCHARS_POLISH_L_SMALL: return s_polishLSmall;
	case CUSTOMCHARS_POLISH_N_SMALL: return s_polishNSmall;
	case CUSTOMCHARS_POLISH_O_SMALL: return s_polishOSmall;
	case CUSTOMCHARS_POLISH_S_SMALL: return s_polishSSmall;
	case CUSTOMCHARS_POLISH_X_SMALL: return s_polishXSmall;
	case CUSTOMCHARS_POLISH_Z_SMALL: return s_polishZSmall;

	case CUSTOMCHARS_LEFT_SQUARE_BRACKET_FILLED: return s_leftSquareBracketFilled;
	case CUSTOMCHARS_RIGHT_SQUARE_BRACKET_FILLED: return s_rightSquareBracketFilled;
	case CUSTOMCHARS_VERTICAL_THICK_LINE: return s_verticalThickLine;
	case CUSTOMCHARS_MENU_POINTER: return s_menuPointer;
	case CUSTOMCHARS_HOURGLASS: return s_hourglass;
	case CUSTOMCHARS_BELL: return s_bell;
	case CUSTOMCHARS_ANTENNA: return s_antenna;
	case CUSTOMCHARS_FINDING_UP: return s_findingUp;
	case CUSTOMCHARS_FINDING_DOWN: return s_findingDown;
	case CUSTOMCHARS_ANTENNA_RECEIVED: return s_antennaReceived;
	case CUSTOMCHARS_PROGRESS: return s_progress;
	case CUSTOMCHARS_DOT_CENTER: return s_dotCenter;
	}

	return 0;
}

void customCharsUpdateProgress(uint8_t progress[8])
{
	for (uint8_t i = 0; i < 8; i++) s_progress[i] = progress[i];
}
