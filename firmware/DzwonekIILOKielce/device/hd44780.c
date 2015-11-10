#include "hd44780.h"

#include <util/delay.h>

#define HD44780_4_BIT_MODE_NIBBLE 0x03
#define HD44780_4_BIT_MODE_INIT 0x02

static void hd44780SetNibble(uint8_t nibble);
static void hd44780Write(uint8_t data);

void hd44780Init(uint8_t powerApplied)
{
	HD44780_RS_DDR |= (1 << HD44780_RS_BIT);
	HD44780_E_DDR |= (1 << HD44780_E_BIT);
	HD44780_D4_DDR |= (1 << HD44780_D4_BIT);
	HD44780_D5_DDR |= (1 << HD44780_D5_BIT);
	HD44780_D6_DDR |= (1 << HD44780_D6_BIT);
	HD44780_D7_DDR |= (1 << HD44780_D7_BIT);

	if (powerApplied) _delay_ms(15); // wait 15 ms if power was applied

	HD44780_RS_PORT &= ~(1 << HD44780_RS_BIT);
	HD44780_E_PORT &= ~(1 << HD44780_E_BIT);

	// initialization
	HD44780_E_PORT |= (1 << HD44780_E_BIT);
	hd44780SetNibble(HD44780_4_BIT_MODE_NIBBLE);
	HD44780_E_PORT &= ~(1 << HD44780_E_BIT);
	_delay_ms(5);

	HD44780_E_PORT |= (1 << HD44780_E_BIT);
	hd44780SetNibble(HD44780_4_BIT_MODE_NIBBLE);
	HD44780_E_PORT &= ~(1 << HD44780_E_BIT);
	_delay_us(160);

	HD44780_E_PORT |= (1 << HD44780_E_BIT);
	hd44780SetNibble(HD44780_4_BIT_MODE_NIBBLE);
	HD44780_E_PORT &= ~(1 << HD44780_E_BIT);
	_delay_us(160);


	// operating characteristics
	HD44780_E_PORT |= (1 << HD44780_E_BIT);
	hd44780SetNibble(HD44780_4_BIT_MODE_INIT);
	HD44780_E_PORT &= ~(1 << HD44780_E_BIT);

	_delay_ms(1);

	hd44780Command(HD44780_FUNCTION_SET | HD44780_FUNCTION_SET_FONT5x7 | HD44780_FUNCTION_SET_2_LINE | HD44780_FUNCTION_SET_4_BIT);
	hd44780Command(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ONOFF_OFF);
	hd44780Command(HD44780_CLEAR);
	_delay_ms(2);
	hd44780Command(HD44780_ENTRY_MODE | HD44780_ENTRY_MODE_SHIFT_CURSOR | HD44780_ENTRY_MODE_INCREMENT);
	hd44780Command(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ONOFF_ON | HD44780_DISPLAY_ONOFF_CURSOR_OFF | HD44780_DISPLAY_ONOFF_CURSOR_NOBLINK);
}

void hd44780Home()
{
	hd44780Command(HD44780_HOME);
	_delay_ms(2);
}
void hd44780Clear()
{
	hd44780Command(HD44780_CLEAR);
	_delay_ms(2);
}
void hd44780GoTo(uint8_t x, uint8_t y)
{
	hd44780Command(HD44780_SET_DDRAM | (x + 0x40 * y));
}
void hd44780Text(const char* text)
{
	while(*text) hd44780Data(*text++);
}

void hd44780Command(uint8_t command)
{
	HD44780_RS_PORT &= ~(1 << HD44780_RS_BIT);
	hd44780Write(command);
}
void hd44780Data(uint8_t data)
{
	HD44780_RS_PORT |= (1 << HD44780_RS_BIT);
	hd44780Write(data);
}

void hd44780DefineChar(uint8_t id, const uint8_t* data)
{
	hd44780Command(HD44780_SET_CGRAM + 8 * id);
	for(uint8_t i = 0; i < 8; i++) hd44780Data(data[i]);
}

static void hd44780SetNibble(uint8_t nibble)
{
	if (nibble & (1 << 0)) HD44780_D4_PORT |= (1 << HD44780_D4_BIT); else HD44780_D4_PORT &= ~(1 << HD44780_D4_BIT);
	if (nibble & (1 << 1)) HD44780_D5_PORT |= (1 << HD44780_D5_BIT); else HD44780_D5_PORT &= ~(1 << HD44780_D5_BIT);
	if (nibble & (1 << 2)) HD44780_D6_PORT |= (1 << HD44780_D6_BIT); else HD44780_D6_PORT &= ~(1 << HD44780_D6_BIT);
	if (nibble & (1 << 3)) HD44780_D7_PORT |= (1 << HD44780_D7_BIT); else HD44780_D7_PORT &= ~(1 << HD44780_D7_BIT);
}
static void hd44780Write(uint8_t data)
{
	// delay 1 us to avoid weird problems
	// delay 50 us - typical execution time

	_delay_us(1);
	HD44780_E_PORT |= (1 << HD44780_E_BIT);
	hd44780SetNibble(data >> 4);
	_delay_us(1);
	HD44780_E_PORT &= ~(1 << HD44780_E_BIT);

	_delay_us(1);
	HD44780_E_PORT |= (1 << HD44780_E_BIT);
	hd44780SetNibble(data);
	_delay_us(1);
	HD44780_E_PORT &= ~(1 << HD44780_E_BIT);

	_delay_us(50);
}
