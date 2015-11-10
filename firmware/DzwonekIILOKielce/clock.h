#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>
#include "common.h"

#define CLOCK_TIME_TRANSITION_NO_ACTION 0
#define CLOCK_TIME_TRANSITION_CET_TO_CEST 1 // + 1 hour
#define CLOCK_TIME_TRANSITION_CEST_TO_CET 2 // - 1 hour

#define CLOCK_UPDATE_TICKS 10
#define CLOCK_DCF77_FAILS_TO_RESET 10
#define CLOCK_DCF77_RESET_DELAY TICKS_PER_SECOND

void clockInit();

void clockTick();
void clockProcess();

const dateTime_t* clockGetCurrentDateTimePointer();

void clockSetDateTime(const dateTime_t* dateTime);

uint8_t clockGetTimeTransitionState();
const dateTime_t* clockGetLastSynchronizationPointer(); // year will be 0 if synchronization never occurred

#endif /* CLOCK_H_ */
