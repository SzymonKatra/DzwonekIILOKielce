#include "interface.h"

#include <util/atomic.h>
#include "customChars.h"
#include "device/hd44780.h"

#define upInit() INTERFACE_UP_DDR &= ~(1 << INTERFACE_UP_BIT); INTERFACE_UP_PORT |= (1 << INTERFACE_UP_BIT)
#define upIsPressed() (!(INTERFACE_UP_PIN & (1 << INTERFACE_UP_BIT)))

#define selectInit() INTERFACE_SELECT_DDR &= ~(1 << INTERFACE_SELECT_BIT); INTERFACE_SELECT_PORT |= (1 << INTERFACE_SELECT_BIT)
#define selectIsPressed() (!(INTERFACE_SELECT_PIN & (1 << INTERFACE_SELECT_BIT)))

#define downInit() INTERFACE_DOWN_DDR &= ~(1 << INTERFACE_DOWN_BIT); INTERFACE_DOWN_PORT |= (1 << INTERFACE_DOWN_BIT)
#define downIsPressed() (!(INTERFACE_DOWN_PIN & (1 << INTERFACE_DOWN_BIT)))

#define INTERFACE_DISPLAY_BUFFER_ID 0
#define INTERFACE_DISPLAY_BACK_BUFFER_ID 1
#define INTERFACE_CGRAM_SIZE 8

typedef struct
{
	uint8_t pulseCount;
	uint8_t pressed;
	uint8_t debounceTicks;
	uint8_t nextPressTicks;
	uint8_t locked;
}interfaceButtonState_t;

static volatile interfaceButtonState_t s_buttons[3];
static volatile uint16_t s_disableCounter;
static volatile uint8_t s_interfaceEnabled;

static menuFunction_f s_menusStack[INTERFACE_MENU_STACK_SIZE];
static uint8_t s_menusStackTop;
static uint8_t s_menuFlags;
static uint16_t s_menuTimers[INTERFACE_MENU_TIMERS_COUNT];

static uint8_t s_cgramStatus[INTERFACE_CGRAM_SIZE];
static uint8_t s_currentCgramAddress = 0;
static char s_displayBuffer[INTERFACE_DISPLAY_WIDTH][INTERFACE_DISPLAY_HEIGHT][2];

static void interfaceDisplayDataBuffered(uint8_t x, uint8_t y, char data);
static void interfaceDisplayPutChar(uint8_t x, uint8_t y, char value);

static void interfaceButtonStateReset(volatile interfaceButtonState_t* state);
static void interfaceButtonStateTick(volatile interfaceButtonState_t* state, uint8_t isPressed);

void interfaceInit(menuFunction_f startupMenu)
{
	s_menusStackTop = 0;
	s_menusStack[s_menusStackTop] = startupMenu;
	s_menuFlags |= INTERFACE_MENU_INITIALIZE | INTERFACE_MENU_ENTERED | INTERFACE_MENU_WAKEUP;

	for (uint8_t i = 0; i < INTERFACE_BUTTON_COUNT; i++) interfaceButtonStateReset(&s_buttons[i]);
	for (uint8_t i = 0; i < INTERFACE_MENU_TIMERS_COUNT; i++) s_menuTimers[i] = 0;
	s_disableCounter = 0;
	s_interfaceEnabled = 0;

	upInit();
	selectInit();
	downInit();
	lcdPowerInit();

	interfaceEnable();
}

void interfaceTick()
{
	if (s_menusStackTop != 0 || selectIsPressed() || upIsPressed() || downIsPressed()) s_disableCounter = INTERFACE_DISABLE_NOACTION_TICKS;

	if (s_disableCounter > 0 && s_interfaceEnabled)
	{
		s_disableCounter--;

		interfaceButtonStateTick(&s_buttons[INTERFACE_BUTTON_UP], upIsPressed());
		interfaceButtonStateTick(&s_buttons[INTERFACE_BUTTON_SELECT], selectIsPressed());
		interfaceButtonStateTick(&s_buttons[INTERFACE_BUTTON_DOWN], downIsPressed());

		for (uint8_t i = 0; i < INTERFACE_MENU_TIMERS_COUNT; i++) s_menuTimers[i]++;
	}
}
void interfaceProcess()
{
	uint16_t tmpDisableCounter;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		tmpDisableCounter = s_disableCounter;
	}

	if (tmpDisableCounter > 0 && !s_interfaceEnabled) interfaceEnable();
	else if (tmpDisableCounter == 0 && s_interfaceEnabled && s_menusStackTop == 0) interfaceDisable();

	uint8_t flags = s_menuFlags;
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		interfaceMenuTimerResetAll();
	}
	if (flags & INTERFACE_MENU_ENTERED)
	{
		interfaceButtonsLock();
		interfaceHold();
	}
	s_menuFlags = 0;

	interfaceDisplayClear();
	if (s_interfaceEnabled) s_menusStack[s_menusStackTop](flags);

	interfaceDisplaySwapBuffers();
}

void interfaceEnable()
{
	for (uint8_t y = 0; y < INTERFACE_DISPLAY_HEIGHT; y++)
	{
		for (uint8_t x = 0; x < INTERFACE_DISPLAY_WIDTH; x++)
		{
			for (uint8_t i = 0; i < 2; i++)
			{
				s_displayBuffer[x][y][i] = ' ';
			}
		}
	}

	for (uint8_t i = 0; i < INTERFACE_CGRAM_SIZE; i++) s_cgramStatus[i] = 0;

	lcdPowerOn();
	hd44780Init(1);
	s_interfaceEnabled = 1;
	s_menusStackTop = 0;
	s_menuFlags |= INTERFACE_MENU_ENTERED | INTERFACE_MENU_WAKEUP;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		s_disableCounter = INTERFACE_DISABLE_NOACTION_TICKS;
	}
}
void interfaceDisable()
{
	hd44780Command(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ONOFF_OFF);
	lcdPowerOff();
	s_interfaceEnabled = 0;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		s_disableCounter = 0;
	}
}
uint8_t interfaceHold()
{
	if (!s_interfaceEnabled) return 0;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		s_disableCounter = INTERFACE_DISABLE_NOACTION_TICKS;
	}
	return 1;
}
uint8_t interfaceIsEnabled()
{
	return s_interfaceEnabled;
}

uint8_t interfaceHandleButton(uint8_t button, uint8_t type)
{
	if (s_buttons[button].locked) return 0;

	uint8_t result = 0;

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		switch (type)
		{
			case INTERFACE_BUTTON_HANDLE_STATE:
				switch (button)
				{
					case INTERFACE_BUTTON_UP: result = upIsPressed(); break;
					case INTERFACE_BUTTON_SELECT: result = selectIsPressed(); break;
					case INTERFACE_BUTTON_DOWN: result = downIsPressed(); break;
				}
				break;

			case INTERFACE_BUTTON_HANDLE_PRESSED:
				if (s_buttons[button].pressed)
				{
					s_buttons[button].pressed = 0;
					result = 1;
				}
				break;

			case INTERFACE_BUTTON_HANDLE_PULSE:
				if (s_buttons[button].pulseCount)
				{
					s_buttons[button].pulseCount--;
					result = 1;
				}
				break;
		}
	}

	return result;
}
uint8_t interfaceHandleAnyButton(uint8_t type)
{
	uint8_t result = 0;

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		switch (type)
		{
		case INTERFACE_BUTTON_HANDLE_STATE:
			result = upIsPressed() || selectIsPressed() || downIsPressed();
			break;

		case INTERFACE_BUTTON_HANDLE_PRESSED:
			for (uint8_t i = 0; i < INTERFACE_BUTTON_COUNT; i++)
			{
				if (s_buttons[i].pressed)
				{
					result = 1;
					s_buttons[i].pressed = 0;
				}
			}
			break;

		case INTERFACE_BUTTON_HANDLE_PULSE:
			for (uint8_t i = 0; i < INTERFACE_BUTTON_COUNT; i++)
			{
				if (s_buttons[i].pulseCount)
				{
					result = 1;
					s_buttons[i].pulseCount--;
				}
			}
			break;
		}
	}

	return result;
}

void interfaceMenuOpen(menuFunction_f function)
{
	s_menuFlags |= INTERFACE_MENU_INITIALIZE | INTERFACE_MENU_ENTERED;
	s_menusStackTop++;
	s_menusStack[s_menusStackTop] = function;
}
void interfaceMenuPrevious()
{
	if (s_menusStackTop == 0) return;
	s_menuFlags |= INTERFACE_MENU_ENTERED | INTERFACE_MENU_RETURNED;
	s_menusStackTop--;
}

uint16_t interfaceMenuTimerGet(uint8_t timer)
{
	uint16_t result;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		result = s_menuTimers[timer];
	}
	return result;
}
void interfaceMenuTimerSet(uint8_t timer, uint16_t ticks)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		s_menuTimers[timer] = ticks;
	}
}
void interfaceMenuTimerReset(uint8_t timer)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		s_menuTimers[timer] = 0;
	}
}
void interfaceMenuTimerResetAll()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		for (uint8_t i = 0; i < INTERFACE_MENU_TIMERS_COUNT; i++) s_menuTimers[i] = 0;
	}
}

void interfaceDisplayWrite(uint8_t x, uint8_t y, const char* text)
{
	//hd44780GoTo(x, y);
	while(*text)
	{
		interfaceDisplayPutChar(x, y, *text);
		text++;
		x++;
	}
}
void interfaceDisplayWriteChar(uint8_t x, uint8_t y, char value)
{
	//hd44780GoTo(x, y);
	interfaceDisplayPutChar(x, y, value);
}
void interfaceDisplayWriteChars(uint8_t x, uint8_t y, char value, uint8_t count)
{
	//hd44780GoTo(x, y);
	while (count--) interfaceDisplayPutChar(x++, y, value);
}
void interfaceDisplayClear()
{
	for (uint8_t y = 0; y < INTERFACE_DISPLAY_HEIGHT; y++)
	{
		for (uint8_t x = 0; x < INTERFACE_DISPLAY_WIDTH; x++)
		{
			s_displayBuffer[x][y][INTERFACE_DISPLAY_BACK_BUFFER_ID] = ' ';
		}
	}
	//hd44780Clear();
}
void interfaceDisplaySwapBuffers()
{
	for (uint8_t y = 0; y < INTERFACE_DISPLAY_HEIGHT; y++)
	{
		hd44780GoTo(0, y);
		uint8_t expectedX = 0;
		for (uint8_t x = 0; x < INTERFACE_DISPLAY_WIDTH; x++)
		{
			if (s_displayBuffer[x][y][INTERFACE_DISPLAY_BUFFER_ID] != s_displayBuffer[x][y][INTERFACE_DISPLAY_BACK_BUFFER_ID])
			{
				if (x != expectedX) hd44780GoTo(x, y);
				expectedX = x + 1;
				hd44780Data(s_displayBuffer[x][y][INTERFACE_DISPLAY_BACK_BUFFER_ID]);
			}
			s_displayBuffer[x][y][INTERFACE_DISPLAY_BUFFER_ID] = s_displayBuffer[x][y][INTERFACE_DISPLAY_BACK_BUFFER_ID];
		}
	}
}
void interfaceDisplayInvalidateCustomChar(uint8_t id)
{
	for (uint8_t i = 0; i < INTERFACE_CGRAM_SIZE; i++)
	{
		if (s_cgramStatus[i] == id)
		{
			s_cgramStatus[i] = 0;
			s_currentCgramAddress = i;
		}
	}
}

void interfaceButtonsLock()
{
	for (uint8_t i = 0; i < INTERFACE_BUTTON_COUNT; i++) s_buttons[i].locked = 1;
}

static void interfaceDisplayDataBuffered(uint8_t x, uint8_t y, char data)
{
	if (x >= INTERFACE_DISPLAY_WIDTH || y >= INTERFACE_DISPLAY_HEIGHT) return;

	s_displayBuffer[x][y][INTERFACE_DISPLAY_BACK_BUFFER_ID] = data;
}
static void interfaceDisplayPutChar(uint8_t x, uint8_t y, char value)
{
	// if value > 128 then this char maybe custom
	// if char is detected as custom, loads char into cgram (if not loaded) and display it
	// otherwise char is displayed normally
	// there is limit 8 custom chars per screen
	if (value > 128)
	{
		uint8_t id = customCharsAnsiToId(value);
		if (id != 0)
		{
			uint8_t address = 0xFF;
			for (uint8_t i = 0; i < INTERFACE_CGRAM_SIZE; i++)
			{
				if (s_cgramStatus[i] == id) address = i;
			}
			if (address == 0xFF)
			{
				address = s_currentCgramAddress++;
				if (s_currentCgramAddress >= INTERFACE_CGRAM_SIZE) s_currentCgramAddress = 0;
				s_cgramStatus[address] = id;
				hd44780DefineChar(address, customCharsGetIdBuffer(id));
				hd44780GoTo(x, y);
			}
			interfaceDisplayDataBuffered(x, y, address);
		}
		else interfaceDisplayDataBuffered(x, y, value);
	}
	else interfaceDisplayDataBuffered(x, y, value);
}

static void interfaceButtonStateReset(volatile interfaceButtonState_t* state)
{
	state->pulseCount = 0;
	state->pressed = 0;
	state->debounceTicks = 0;
	state->nextPressTicks = 0;
	state->locked = 0;
}
static void interfaceButtonStateTick(volatile interfaceButtonState_t* state, uint8_t isPressed)
{
	if (isPressed)
	{
		if (!state->locked)
		{
			if (state->debounceTicks == 0)
			{
				state->pulseCount++;
				state->pressed = 1;
				state->debounceTicks = INTERFACE_DEBOUNCE_TICKS;
				state->nextPressTicks = INTERFACE_COUNT_PULSES_DELAY_TICKS;
			}
			else if (--state->nextPressTicks == 0)
			{
				state->pulseCount++;
				state->nextPressTicks = INTERFACE_NEXT_PULSE_DELAY_TICKS;
			}
		}
	}
	else
	{
		if (state->debounceTicks == INTERFACE_DEBOUNCE_TICKS)
		{
			state->pulseCount = 0;
			state->pressed = 0;
			state->locked = 1;
			state->debounceTicks--;
		}
		else if (state->debounceTicks > 0) state->debounceTicks--;

		if (state->debounceTicks == 0) state->locked = 0;
	}
}

