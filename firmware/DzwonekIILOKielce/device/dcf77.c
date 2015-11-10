#include "dcf77.h"

#include <string.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define dcf77PowerEnable() DCF77_PDN_PORT &= ~(1 << DCF77_PDN_BIT)
#define dcf77PowerDisable() DCF77_PDN_PORT |= (1 << DCF77_PDN_BIT)
#define dcf77PowerIsEnabled() (!(DCF77_PDN_PORT & (1 << DCF77_PDN_BIT)))

#define dcf77InterruptEnable() EICRA |= (1 << ISC01); EIMSK |= (1 << INT0)
#define dcf77InterruptDisable() EIMSK &= ~(1 << INT0)

// ISC01 in EICRA is set in dcf77InterruptEnable()
// thereby we are sure that ISC01 is set when changing direction when interrupt is raised
#define dcf77InterruptFallingEdge() EICRA &= ~(1 << ISC00)
#define dcf77InterruptRisingEdge() EICRA |= (1 << ISC00)
#define dcf77InterruptEdgeToggle() EICRA ^= (1 << ISC00)

#define dcf77IsInterruptOnFallingEdge() (!(EICRA & (1 << ISC00)))
#define dcf77IsInterruptOnRisingEdge() (EICRA & (1 << ISC00))

static volatile uint8_t s_synchronized;
static volatile uint8_t s_failCount;
static volatile uint64_t s_data;
static volatile uint8_t s_position;
static volatile uint64_t s_lastData;
static volatile dcf77ReceivedCallback_f s_receivedCallback;
static volatile dcf77FailedCallback_f s_failedCallback;

void dcf77Init()
{
	s_receivedCallback = NULL;
	s_failedCallback = NULL;

	DCF77_PDN_DDR |= (1 << DCF77_PDN_BIT);
	dcf77PowerDisable();

	// signal input without pull-up
	// just to ensure, this is default state after reset
	DCF77_SIGNAL_PORT &= ~(1 << DCF77_SIGNAL_BIT);
	DCF77_SIGNAL_DDR &= ~(1 << DCF77_SIGNAL_BIT);

	TCCR1B |= (1 << CS12); // clk / 256
	TIMSK1 |= (1 << TOIE1); // interrupt on overflow (receiving failed)

	s_lastData = 0;
}

void dcf77Start()
{
	s_synchronized = 0;
	s_failCount = 0;
	s_data = 0;
	s_position = 0;
	TCNT1 = 0;

	dcf77InterruptRisingEdge();
	dcf77InterruptEnable();

	dcf77PowerEnable();
}
void dcf77Stop()
{
	dcf77PowerDisable();

	dcf77InterruptDisable();
}
uint8_t dcf77IsEnabled()
{
	return dcf77PowerIsEnabled();
}

uint8_t dcf77IsSynchronized()
{
	return s_synchronized;
}
uint8_t dcf77GetReceivedCount()
{
	return s_position;
}
uint8_t dcf77GetSignalState()
{
	return DCF77_SIGNAL_PIN & (1 << DCF77_SIGNAL_BIT);
}
uint64_t dcf77GetLastRawData()
{
	uint64_t tmp;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		tmp = s_lastData;
	}
	return tmp;
}
uint8_t dcf77GetFailCount()
{
	return s_failCount;
}
void dcf77ResetFailCount()
{
	s_failCount = 0;
}

void dcf77RegisterReceivedCallback(dcf77ReceivedCallback_f callback)
{
	s_receivedCallback = callback;
}
void dcf77RegisterFailedCallback(dcf77FailedCallback_f callback)
{
	s_failedCallback = callback;
}

uint8_t dcf77Decode(uint64_t data, dcf77Data_t* result)
{
	// significance of bits
	// source: https://en.wikipedia.org/wiki/DCF77 and https://pl.wikipedia.org/wiki/DCF77

	// 0 : start of transmission, always 0
	// 1 - 14 : civil waring bits
	// 15 : 0 - main transmitter, 1 - backup transmitter
	// 16 : time change announcment (hour before change)
	// 17 : summer time
	// 18 : winter time
	// 19 : leap second announcment (hour before leap second)
	// 20 : start of time information, always 1
	// 21 - 24 : unity of minutes encoded in BCD (21 - LSB, 24 - MSB)
	// 25 - 27 : tens of minutes encoded in BCD (25 - LSB, 27 - MSB)
	// 28 : parity bit for 21 - 27 bits
	// 29 - 32 : unity of hours encoded in BCD (29 - LSB, 32 - MSB)
	// 33 - 34 : tens of hours encoded in BCD (33 - LSB, 34 - MSB)
	// 35 : parity bit for 29 - 34 bits
	// 36 - 39 : unity of day encoded in BCD (36 - LSB, 39 - MSB)
	// 40 - 41 : tens of day encoded in BCD (40 - LSB, 41 - MSB)
	// 42 - 44 : day of week encoded in BCD, 1 - monday, 7 - sunday (42 - LSB, 44 - MSB)
	// 45 - 48 : unity of month in BCD (45 - LSB, 48 - MSB)
	// 49 : tens of month in BCD
	// 50 - 53 : unity of year (within century) in BCD (50 - LSB, 53 - MSB)
	// 54 - 57 : tens of year (within century) in BCD (54 - LSB, 57 - MSB)
	// 58 : parity bit for 36 - 57 bits

	uint8_t bit;

	bit = (data & ((uint64_t)1 << 0)) >> 0;
	if (bit) return 0;

	result->transmitterType = (data & ((uint64_t)1 << 15)) >> 15;
	result->timeChangeAnnouncement = (data & ((uint64_t)1 << 16)) >> 16;
	result->summerTimeNow = (data & ((uint64_t)1 << 17)) >> 17;
	// doesn't need to check 18 (winter time), we can rely only on 17 (summer time) bit
	result->leapSecondAnnouncement = (data & ((uint64_t)1 << 19)) >> 19;

	bit = (data & ((uint64_t)1 << 20)) >> 20;
	if (!bit) return 0;

	uint8_t bcd;

	bcd = (data & ((uint64_t)0x7F << 21)) >> 21;
	result->currentDateTime.minutes = bcdToUint8(bcd);
	if (result->currentDateTime.minutes > 59) return 0;
	bit = (data & ((uint64_t)1 << 28)) >> 28;
	if (computeParity(bcd) != bit) return 0;

	bcd = (data & ((uint64_t)0x3F << 29)) >> 29;
	result->currentDateTime.hours = bcdToUint8(bcd);
	if (result->currentDateTime.hours > 59) return 0;
	bit = (data & ((uint64_t)1 << 35)) >> 35;
	if (computeParity(bcd) != bit) return 0;

	uint8_t parityDate = 0;

	bcd = (data & ((uint64_t)0x3F << 36)) >> 36;
	result->currentDateTime.day = bcdToUint8(bcd);
	parityDate ^= computeParity(bcd);
	// day correctness will be checked when we parse month and year

	bcd = (data & ((uint64_t)0x07 << 42)) >> 42;
	result->currentDateTime.dayOfWeek = bcdToUint8(bcd);
	parityDate ^= computeParity(bcd);
	if (result->currentDateTime.dayOfWeek < 1 || result->currentDateTime.dayOfWeek > 7) return 0;

	bcd = (data & ((uint64_t)0x1F << 45)) >> 45;
	result->currentDateTime.month = bcdToUint8(bcd);
	parityDate ^= computeParity(bcd);
	if (result->currentDateTime.month < 1 || result->currentDateTime.month > 12) return 0;

	bcd = (data & ((uint64_t)0xFF << 50)) >> 50;
	result->currentDateTime.year = bcdToUint8(bcd) + 100; // assume that now is XXI century
	parityDate ^= computeParity(bcd);
	if (result->currentDateTime.year > 199) return 0;

	// month and year parsed so we can check correctness of day
	if (result->currentDateTime.day < 1 || result->currentDateTime.day > monthDays(result->currentDateTime.month, (uint16_t)result->currentDateTime.year + (uint16_t)1900)) return 0;

	bit = (data & ((uint64_t)1 << 58)) >> 58;
	if (parityDate != bit) return 0;

	result->currentDateTime.seconds = 0;

	return 1;
}

ISR(INT0_vect)
{
	uint16_t counterTmp = TCNT1;

	if (dcf77IsInterruptOnRisingEdge())
	{
		// counterTmp - time of low level from last falling edge

		if (s_synchronized)
		{
			if (counterTmp < DCF77_NODATA_MIN || counterTmp > DCF77_NODATA_MAX)
			{
				// invalid no data time
				s_synchronized = 0;
				s_failCount++;
				if (s_failedCallback != NULL) s_failedCallback();
			}
		}
		else // not synchronized
		{
			if (counterTmp >= DCF77_SYNCHRONIZATION_WAIT_MIN && counterTmp <= DCF77_SYNCHRONIZATION_WAIT_MAX)
			{
				s_synchronized = 1;
				s_position = 0;
				s_data = 0;
			}
		}
	}
	else // on falling edge
	{
		// counterTmp - time of high level from last rising edge

		if (counterTmp >= DCF77_ZERO_MIN && counterTmp <= DCF77_ZERO_MAX)
		{
			// zero arrived
			// don't need to change s_data because default is 0
		}
		else if (counterTmp >= DCF77_ONE_MIN && counterTmp <= DCF77_ONE_MAX)
		{
			// one arrived
			s_data |= ((uint64_t)1 << s_position);
		}
		else
		{
			// invalid pulse
			uint8_t tmpSynchronized = s_synchronized;
			s_synchronized = 0;
			s_failCount++;
			if (tmpSynchronized && s_failedCallback != NULL) s_failedCallback();
		}

		if (s_synchronized) // ensure that current pulse was valid
		{
			s_position++;

			if (s_position >= 59)
			{
				// end of packet frame
				// receiving successful
				s_synchronized = 0;
				s_lastData = s_data;

				if (s_receivedCallback != NULL) s_receivedCallback(s_lastData);
			}
		}
	}

	TCNT1 = 0;
	dcf77InterruptEdgeToggle();
}

ISR(TIMER1_OVF_vect)
{
	// no level change for more than (65536 / DCF77_TIMER_SECOND) seconds (5,12 s)
	// it is recognized as fail (the total absence of signal or noise)

	s_failCount++;
	s_synchronized = 0;
}
