#ifndef DCF77_H_
#define DCF77_H_

#include <stdint.h>
#include <avr/io.h>
#include "../common.h"

#define DCF77_PDN_DDR DDRC
#define DCF77_PDN_PORT PORTC
#define DCF77_PDN_BIT PC0

// must match INT0
#define DCF77_SIGNAL_DDR DDRD
#define DCF77_SIGNAL_PORT PORTD
#define DCF77_SIGNAL_PIN PIND
#define DCF77_SIGNAL_BIT PD2

#define DCF77_TIMER_SECOND 12800

#define DCF77_NODATA_MIN 8960 // 700 ms
#define DCF77_NODATA_MAX 12800 // 1000 ms

#define DCF77_SYNCHRONIZATION_WAIT_MIN 19200 // 1500 ms
#define DCF77_SYNCHRONIZATION_WAIT_MAX 28160 // 2200 ms

#define DCF77_ZERO_MIN 768 // 60 ms
#define DCF77_ZERO_MAX 1792 // 140 ms

#define DCF77_ONE_MIN 2048 // 160 ms
#define DCF77_ONE_MAX 3072 // 240 ms

typedef struct
{
	uint8_t transmitterType; // 0 - main, 1 - backup
	uint8_t timeChangeAnnouncement;
	uint8_t summerTimeNow;
	uint8_t leapSecondAnnouncement;
	dateTime_t currentDateTime;
}dcf77Data_t;

// (uint64_t rawData)
typedef void(*dcf77ReceivedCallback_f)(uint64_t rawData);

typedef void(*dcf77FailedCallback_f)();

void dcf77Init();

void dcf77Start();
void dcf77Stop();

uint8_t dcf77IsEnabled();

uint8_t dcf77IsSynchronized();
uint8_t dcf77GetReceivedCount();
uint8_t dcf77GetSignalState();
uint64_t dcf77GetLastRawData();
uint8_t dcf77GetFailCount();
void dcf77ResetFailCount();

void dcf77RegisterReceivedCallback(dcf77ReceivedCallback_f callback); // registered callback will be fired in interrupt, not allowed to be any time-expensive operations
void dcf77RegisterFailedCallback(dcf77FailedCallback_f callback); // registered callback will be fired in interrupt, not allowed to be any time-expensive operations

uint8_t dcf77Decode(uint64_t data, dcf77Data_t* result);

#endif /* DCF77_H_ */
