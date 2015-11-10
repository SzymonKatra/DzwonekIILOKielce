#ifndef DEVICE_SDCARD_HARDWARECONFIG_H_
#define DEVICE_SDCARD_HARDWARECONFIG_H_

#include <avr/io.h>

#define SDCARD_CS_DDR DDRD
#define SDCARD_CS_PORT PORTD
#define SDCARD_CS_BIT PD1

#define SDCARD_CK_DDR DDRB
#define SDCARD_CK_PORT PORTB
#define SDCARD_CK_BIT PB5

#define SDCARD_DI_DDR DDRB
#define SDCARD_DI_PORT PORTB
#define SDCARD_DI_BIT PB3

#define SDCARD_DO_DDR DDRB
#define SDCARD_DO_PORT PORTB
#define SDCARD_DO_PIN PINB
#define SDCARD_DO_BIT PB4

#endif
