#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include "clock.h"
#include "common.h"
#include "controller.h"
#include "interface.h"
#include "internalStorage.h"
#include "strings.h"
#include "device/dcf77.h"
#include "device/ds3231.h"
#include "device/hd44780.h"
#include "device/i2c.h"
#include "menu/menuMain.h"

// maximum speed of main loop (maximum calls to <module>Process() per second)
#define MAIN_TICKS (TICKS_PER_SECOND / 25)

static volatile uint8_t s_mainTicks;

int main()
{
	MCUSR = 0; // otherwise mcu will enter into watchdog reset loop
	wdt_disable();

	// user must know about initialization
	// this code bypasses all modules
	lcdPowerInit(); // defined in interface.h
	lcdPowerOn(); // same as above
	hd44780Init(1);

	if(!internalStorageIsChecksumValid())
	{
		stringsLoad(stringsBuffer, stringsSettingFactorySettingsTop);
		hd44780GoTo(0, 0);
		hd44780Text(stringsBuffer);

		stringsLoad(stringsBuffer, stringsSettingFactorySettingsBottom);
		hd44780GoTo(0, 1);
		hd44780Text(stringsBuffer);

		internalStorageFactorySettings();
	}

	stringsLoad(stringsBuffer, stringsInitialization);
	hd44780GoTo(0, 1);
	hd44780Text(stringsBuffer);

	// === INITIALIZATION PROCEDURES ===

	// devices
	i2cInit();
	dcf77Init();
	// hd44780 doesn't need to init here because interface module owns it initialization

	// modules
	clockInit();
	controllerInit();
	interfaceInit(&menuMain); // owns hd44780

	TCCR0B |= (1 << CS02); // clk / 256
	TIMSK0 |= (1 << TOIE0); // interrupt on overflow

	s_mainTicks = 0;

	sei(); // enable global interrupts

	wdt_enable(WDTO_8S); // enable watchdog to prevent hang-up
	// if the system hang-up and reset by watchdog, there must be a bug in firmware

	// === END OF INITIALIZATION ===

	while (1)
	{
		wdt_reset();

		if (s_mainTicks >= MAIN_TICKS)
		{
			s_mainTicks = 0;

			clockProcess();
			controllerProcess();
			interfaceProcess();
		}
	}

	return 0;
}

ISR(TIMER0_OVF_vect)
{
	// one tick is every 20 ms
	// TICKS_PER_SECOND defined in common.h

	s_mainTicks++;

	clockTick();
	controllerTick();
	interfaceTick();
}
