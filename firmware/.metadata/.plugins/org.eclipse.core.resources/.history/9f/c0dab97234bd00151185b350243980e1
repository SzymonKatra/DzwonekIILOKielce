#ifndef INTERNALSTORAGE_H_
#define INTERNALSTORAGE_H_

#include <stdint.h>
#include "common.h"

// if you dont change CONTROLLER_PROFILE_BELLS_COUNT = 40 and CONTROLLER_PROFILE_COUNT = 4 all data will take up exactly 1000 bytes

#define INTERNALSTORAGE_METADATA_PRESERVE_SPACE 0x8
#define INTERNALSTORAGE_SETTINGS_PRESERVE_SPACE 0x40
#define INTERNALSTORAGE_PROFILE_PRESERVE_SPACE (2 * PROFILE_BELLS_COUNT + 0x20)
#define INTERNALSTORAGE_MONTH_PRESERVE_SPACE 0x28

#define INTERNALSTORAGE_METADATA_ADDRESS 0x0
#define INTERNALSTORAGE_SETTINGS_ADDRESS (INTERNALSTORAGE_METADATA_ADDRESS + INTERNALSTORAGE_METADATA_PRESERVE_SPACE)
#define INTERNALSTORAGE_PROFILES_ADDRESS (INTERNALSTORAGE_SETTINGS_ADDRESS + INTERNALSTORAGE_SETTINGS_PRESERVE_SPACE)
#define INTERNALSTORAGE_MONTHS_ADDRESS (INTERNALSTORAGE_PROFILES_ADDRESS + INTERNALSTORAGE_PROFILE_PRESERVE_SPACE * PROFILE_COUNT)

#define INTERNALSTORAGE_COMPUTE_PROFILE_ADDRESS(profile) (INTERNALSTORAGE_PROFILES_ADDRESS + profile * INTERNALSTORAGE_PROFILE_PRESERVE_SPACE)
#define INTERNALSTORAGE_COMPUTE_MONTH_ADDRESS(month) (INTERNALSTORAGE_MONTHS_ADDRESS + month * INTERNALSTORAGE_MONTH_PRESERVE_SPACE)

#define INTERNALSTORAGE_VALID_CHECKSUM 0x14D4 // random, magic value

uint8_t internalStorageIsChecksumValid();
void internalStorageFactorySettings();

void internalStorageSettingsWrite(const settings_t* data);
void internalStorageSettingsRead(settings_t* result);

void internalStorageProfileWrite(uint8_t profile, const profile_t* data);
void internalStorageProfileRead(uint8_t profile, profile_t* result);

void internalStorageMonthWrite(uint8_t month, const month_t* data);
void internalStorageMonthRead(uint8_t month, month_t* result);

void internalStorageInvalidateChecksum();

#endif /* INTERNALSTORAGE_H_ */
