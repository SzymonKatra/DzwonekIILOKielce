#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <stdint.h>
#include <avr/io.h>
#include "common.h"

#define CONTROLLER_BELL_DDR DDRD
#define CONTROLLER_BELL_PORT PORTD
#define CONTROLLER_BELL_BIT PD3

#define CONTROLLER_RUNALARM_DDR DDRD
#define CONTROLLER_RUNALARM_PORT PORTD
#define CONTROLLER_RUNALARM_PIN PIND
#define CONTROLLER_RUNALARM_BIT PD0

#define CONTROLLER_ALARM_LONGPRESS (3 * TICKS_PER_SECOND)

void controllerInit();

void controllerTick();
void controllerProcess();

void controllerSettingsApply(const settings_t* settings);
const settings_t* controllerSettingsGetPointer();

uint8_t controllerProfileGetCurrent();
void controllerProfileUpdate(uint8_t profile, const profile_t* data);

uint8_t controllerMonthGetCurrent();
void controllerMonthUpdate(uint8_t month, const month_t* data);

uint8_t controllerGetNextBell();
uint16_t controllerGetBellRemainingTicks();

void controllerMarkProfileReload();
void controllerMarkMonthReload();

#endif /* CONTROLLER_H_ */
