#ifndef INTERFACE_H_
#define INTERFACE_H_

// SW1 - DOWN
// SW2 - SELECT
// SW3 - UP

#include <avr/io.h>
#include "common.h"

#define INTERFACE_DOWN_DDR DDRC
#define INTERFACE_DOWN_PORT PORTC
#define INTERFACE_DOWN_PIN PINC
#define INTERFACE_DOWN_BIT PC3

#define INTERFACE_SELECT_DDR DDRC
#define INTERFACE_SELECT_PORT PORTC
#define INTERFACE_SELECT_PIN PINC
#define INTERFACE_SELECT_BIT PC2

#define INTERFACE_UP_DDR DDRC
#define INTERFACE_UP_PORT PORTC
#define INTERFACE_UP_PIN PINC
#define INTERFACE_UP_BIT PC1

#define INTERFACE_LCD_POWER_DDR DDRD
#define INTERFACE_LCD_POWER_PORT PORTD
#define INTERFACE_LCD_POWER_BIT PD4

#define INTERFACE_DEBOUNCE_TICKS (TICKS_PER_SECOND / 10)
#define INTERFACE_COUNT_PULSES_DELAY_TICKS (TICKS_PER_SECOND / 2)
#define INTERFACE_NEXT_PULSE_DELAY_TICKS (TICKS_PER_SECOND / 7)

#define INTERFACE_DISABLE_NOACTION_TICKS (TICKS_PER_SECOND * 30)

#define INTERFACE_BUTTON_UP 0
#define INTERFACE_BUTTON_SELECT 1
#define INTERFACE_BUTTON_DOWN 2

#define INTERFACE_BUTTON_COUNT 3

#define INTERFACE_BUTTON_HANDLE_STATE 0
#define INTERFACE_BUTTON_HANDLE_PRESSED 1
#define INTERFACE_BUTTON_HANDLE_PULSE 2

#define INTERFACE_MENU_COUNT 10
#define INTERFACE_MENU_STACK_SIZE 10
#define INTERFACE_MENU_TIMERS_COUNT 5
#define INTERFACE_MENU_LAST_TIMER (INTERFACE_MENU_TIMERS_COUNT - 1)

#define INTERFACE_MENU_INITIALIZE (1 << 0)
#define INTERFACE_MENU_ENTERED (1 << 1)
#define INTERFACE_MENU_WAKEUP (1 << 2)
#define INTERFACE_MENU_RETURNED (1 << 3)

#define INTERFACE_DISPLAY_WIDTH 16
#define INTERFACE_DISPLAY_HEIGHT 2

#define lcdPowerInit() INTERFACE_LCD_POWER_DDR |= (1 << INTERFACE_LCD_POWER_BIT)
#define lcdPowerOff() INTERFACE_LCD_POWER_PORT |= (1 << INTERFACE_LCD_POWER_BIT)
#define lcdPowerOn() INTERFACE_LCD_POWER_PORT &= ~(1 << INTERFACE_LCD_POWER_BIT)

// (uint8_t flags)
typedef void(*menuFunction_f)(uint8_t);

#define interfaceHandleButtonState(button) interfaceHandleButton(button, INTERFACE_BUTTON_HANDLE_STATE)
#define interfaceHandleButtonPressed(button) interfaceHandleButton(button, INTERFACE_BUTTON_HANDLE_PRESSED)
#define interfaceHandleButtonPulse(button) interfaceHandleButton(button, INTERFACE_BUTTON_HANDLE_PULSE)

#define interfaceHandleAnyButtonState() interfaceHandleAnyButton(INTERFACE_BUTTON_HANDLE_STATE)
#define interfaceHandleAnyButtonPressed() interfaceHandleAnyButton(INTERFACE_BUTTON_HANDLE_PRESSED)
#define interfaceHandleAnyButtonPulse() interfaceHandleAnyButton(INTERFACE_BUTTON_HANDLE_PULSE)

void interfaceInit(menuFunction_f startupMenu);

void interfaceTick();
void interfaceProcess();

void interfaceEnable();
void interfaceDisable();
uint8_t interfaceHold();
uint8_t interfaceIsEnabled();

uint8_t interfaceHandleButton(uint8_t button, uint8_t type);
uint8_t interfaceHandleAnyButton(uint8_t type);

void interfaceButtonsLock();

void interfaceMenuOpen(menuFunction_f menu);
void interfaceMenuPrevious();

// menu can use following functions to measure time
uint16_t interfaceMenuTimerGet(uint8_t timer);
void interfaceMenuTimerSet(uint8_t timer, uint16_t ticks);
void interfaceMenuTimerReset(uint8_t timer);
void interfaceMenuTimerResetAll();

void interfaceDisplayWrite(uint8_t x, uint8_t y, const char* text);
void interfaceDisplayWriteChar(uint8_t x, uint8_t y, char value);
void interfaceDisplayWriteChars(uint8_t x, uint8_t y, char value, uint8_t count);
void interfaceDisplayClear();
void interfaceDisplaySwapBuffers();
void interfaceDisplayInvalidateCustomChar(uint8_t id);

#endif /* INTERFACE_H_ */
