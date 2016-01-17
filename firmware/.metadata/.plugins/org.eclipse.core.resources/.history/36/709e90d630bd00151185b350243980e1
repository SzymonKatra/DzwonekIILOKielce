#include "internalStorage.h"

#include <string.h>
#include <avr/eeprom.h>

uint8_t internalStorageIsChecksumValid()
{
	metadata_t metadata;
	eeprom_read_block(&metadata, (void*)INTERNALSTORAGE_METADATA_ADDRESS, sizeof(metadata_t));
	return metadata.checksum == (uint16_t)INTERNALSTORAGE_VALID_CHECKSUM;
}
void internalStorageFactorySettings()
{
	metadata_t metadata;
	eeprom_read_block(&metadata, (void*)INTERNALSTORAGE_METADATA_ADDRESS, sizeof(metadata_t));

	settings_t settings;
	settings.bellLengthForLesson = 10;
	settings.bellLengthForBreak = 5;
	settings.schoolYear = 0;
	settings.autoTimeTransition = 1;
	settings.dcf77SynchronizationTime = 1320; // 22:00
	settings.dcf77SynchronizationMaxLength = 420; // 7 hours
	internalStorageSettingsWrite(&settings);

	profile_t profile;
	profile.count = 0;
	profile.bellTypes = 0xAAAAAAAAAAAAAAAA; // alternately 0 and 1 bits
	for (uint8_t i = 0; i < PROFILE_COUNT; i++) internalStorageProfileWrite(i, &profile);

	month_t month;
	memset(&month, 0, sizeof(month_t));
	for (uint8_t i = 0; i < 12; i++) internalStorageMonthWrite(i, &month);

	metadata.checksum = (uint16_t)INTERNALSTORAGE_VALID_CHECKSUM;
	eeprom_update_block(&metadata, (void*)INTERNALSTORAGE_METADATA_ADDRESS, sizeof(metadata_t));
}

void internalStorageSettingsWrite(const settings_t* data)
{
	eeprom_update_block(data, (void*)INTERNALSTORAGE_SETTINGS_ADDRESS, sizeof(settings_t));
}
void internalStorageSettingsRead(settings_t* result)
{
	eeprom_read_block(result, (void*)INTERNALSTORAGE_SETTINGS_ADDRESS, sizeof(settings_t));
}

void internalStorageProfileWrite(uint8_t profile, const profile_t* data)
{
	eeprom_update_block(data, (void*)INTERNALSTORAGE_COMPUTE_PROFILE_ADDRESS(profile), sizeof(profile_t));
}
void internalStorageProfileRead(uint8_t profile, profile_t* result)
{
	eeprom_read_block(result, (void*)INTERNALSTORAGE_COMPUTE_PROFILE_ADDRESS(profile), sizeof(profile_t));
}

void internalStorageMonthWrite(uint8_t month, const month_t* data)
{
	eeprom_update_block(data, (void*)INTERNALSTORAGE_COMPUTE_MONTH_ADDRESS(month), sizeof(month_t));
}
void internalStorageMonthRead(uint8_t month, month_t* result)
{
	eeprom_read_block(result, (void*)INTERNALSTORAGE_COMPUTE_MONTH_ADDRESS(month), sizeof(month_t));
}

void internalStorageInvalidateChecksum()
{
	metadata_t metadata;
	eeprom_read_block(&metadata, (void*)INTERNALSTORAGE_METADATA_ADDRESS, sizeof(metadata_t));

	metadata.checksum = 0xFFFF; // invalid, in the next reset all data will be restored to factory state
	eeprom_update_block(&metadata, (void*)INTERNALSTORAGE_METADATA_ADDRESS, sizeof(metadata_t));
}
