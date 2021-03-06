#include "menuSdCard.h"

#include <stdio.h>
#include <avr/eeprom.h>
#include "../interface.h"
#include "../strings.h"
#include "../device/sdcard/pff.h"
#include "menu.h"

#define MENU_SDCARD_STATE_INFO 1
#define MENU_SDCARD_STATE_INITIALIZATION 2
#define MENU_SDCARD_STATE_PROMPT 3
#define MENU_SDCARD_STATE_LOAD 4
#define MENU_SDCARD_STATE_SAVE 5
#define MENU_SDCARD_STATE_VERIFY 6
#define MENU_SDCARD_STATE_SUCCESS 199
#define MENU_SDCARD_STATE_NO_CARD 200
#define MENU_SDCARD_STATE_READ_ERROR 201
#define MENU_SDCARD_STATE_NO_FILESYSTEM 202
#define MENU_SDCARD_STATE_NO_SETTINGS_FILE 203
#define MENU_SDCARD_STATE_NOT_ENOUGH_DATA 204
#define MENU_SDCARD_STATE_VERIFICATION_ERROR 205

#define MENU_SDCARD_BUFFER_SIZE 16
#define MENU_SDCARD_REQUIRED_BYTES 1024

static FATFS s_fileSystem;
static uint8_t s_buffer[MENU_SDCARD_BUFFER_SIZE];
static uint8_t s_bufferSecond[MENU_SDCARD_BUFFER_SIZE];
static uint16_t s_bytesTransferred;
static uint8_t s_state;

static uint8_t s_mode;

static void menuSdCardStateInfo();
static void menuSdCardStateInitializeCard();
static void menuSdCardStateLoad();
static void menuSdCardStateSave();
static void menuSdCardStateVerify();
static void menuSdCardStateSuccess();
static void menuSdCardStateNoCard();
static void menuSdCardStateReadError();
static void menuSdCardStateNoFilesystem();
static void menuSdCardStateNoSettingsFile();
static void menuSdCardStateNotEnoughData();
static void menuSdCardStateVerificationError();

static uint16_t menuSdCardReadBuffer();
static void menuSdCardWriteProgress();

void menuSdCard(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_state = MENU_SDCARD_STATE_INFO;
		s_bytesTransferred = 0;
		interfaceMenuTimerReset(INTERFACE_MENU_LAST_TIMER);
	}

	switch (s_state)
	{
	case MENU_SDCARD_STATE_INFO: menuSdCardStateInfo(); break;
	case MENU_SDCARD_STATE_INITIALIZATION: menuSdCardStateInitializeCard(); break;
	case MENU_SDCARD_STATE_LOAD: menuSdCardStateLoad(); break;
	case MENU_SDCARD_STATE_SAVE: menuSdCardStateSave(); break;
	case MENU_SDCARD_STATE_VERIFY: menuSdCardStateVerify(); break;
	case MENU_SDCARD_STATE_SUCCESS: menuSdCardStateSuccess(); break;

	case MENU_SDCARD_STATE_NO_CARD: menuSdCardStateNoCard(); break;
	case MENU_SDCARD_STATE_READ_ERROR: menuSdCardStateReadError(); break;
	case MENU_SDCARD_STATE_NO_FILESYSTEM: menuSdCardStateNoFilesystem(); break;
	case MENU_SDCARD_STATE_NO_SETTINGS_FILE: menuSdCardStateNoSettingsFile(); break;
	case MENU_SDCARD_STATE_NOT_ENOUGH_DATA: menuSdCardStateNotEnoughData(); break;
	case MENU_SDCARD_STATE_VERIFICATION_ERROR: menuSdCardStateVerificationError(); break;
	}
}
void menuSdCardSet(uint8_t mode)
{
	s_mode = mode;
}

static void menuSdCardStateInfo()
{
	stringsLoad(stringsBuffer, stringsInitializingSdCardTop);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	stringsLoad(stringsBuffer, stringsInitializingSdCardBottom);
	interfaceDisplayWrite(0, 1, stringsBuffer);

	interfaceDisplayWriteChar(15, 1, CUSTOMCHARS_HOURGLASS_ANSI);

	s_state = MENU_SDCARD_STATE_INITIALIZATION;
}
static void menuSdCardStateInitializeCard()
{
	FRESULT result;

	result = pf_mount(&s_fileSystem);

#if MENU_SDCARD_INITIALIZE_TWO_CHANCES == 1
	if (result == FR_NOT_READY) result = pf_mount(&s_fileSystem);
#endif

	if (result == FR_NOT_READY)
		s_state = MENU_SDCARD_STATE_NO_CARD;
	else if (result == FR_DISK_ERR)
		s_state = MENU_SDCARD_STATE_READ_ERROR;
	else if (result == FR_NO_FILESYSTEM)
		s_state = MENU_SDCARD_STATE_NO_FILESYSTEM;

	if (result != FR_OK) return;

	stringsLoad(stringsBuffer, stringsSettingsFileName);
	result = pf_open(stringsBuffer);

	if (result == FR_NO_FILE)
		s_state = MENU_SDCARD_STATE_NO_SETTINGS_FILE;
	else if (result == FR_DISK_ERR)
		s_state = MENU_SDCARD_STATE_READ_ERROR;

	if (result != FR_OK) return;

	s_state = (s_mode == MENU_SDCARD_MODE_LOAD ? MENU_SDCARD_STATE_LOAD : MENU_SDCARD_STATE_SAVE);
}
static void menuSdCardStateLoad()
{
	uint16_t len = menuSdCardReadBuffer();
	if (len == 0)
	{
		s_state = MENU_SDCARD_STATE_NOT_ENOUGH_DATA;
		return;
	}
	uint16_t validLen = len;
	if (s_bytesTransferred + len > MENU_SDCARD_REQUIRED_BYTES) validLen = MENU_SDCARD_REQUIRED_BYTES - s_bytesTransferred;
	eeprom_update_block(s_buffer, (void*)s_bytesTransferred, validLen);
	s_bytesTransferred += len;

	stringsLoad(stringsBuffer, stringsReadingSdCard);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	menuSdCardWriteProgress();

	if (s_bytesTransferred >= MENU_SDCARD_REQUIRED_BYTES)
	{
		s_state = MENU_SDCARD_STATE_VERIFY;
		pf_lseek(0);
		s_bytesTransferred = 0;
	}
}
static void menuSdCardStateSave()
{
	uint16_t len = MENU_SDCARD_REQUIRED_BYTES - s_bytesTransferred;
	if (MENU_SDCARD_BUFFER_SIZE < len) len = MENU_SDCARD_BUFFER_SIZE;
	eeprom_read_block(s_buffer, (void*)s_bytesTransferred, len);
	UINT writtenLen;
	pf_write(s_buffer, len, &writtenLen);
	s_bytesTransferred += len;

	if (len != (uint16_t)writtenLen)
	{
		s_state = MENU_SDCARD_STATE_NOT_ENOUGH_DATA;
		pf_write(0, 0, &writtenLen);
		return;
	}

	stringsLoad(stringsBuffer, stringsWritingSdCard);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	menuSdCardWriteProgress();

	if (s_bytesTransferred >= MENU_SDCARD_REQUIRED_BYTES)
	{
		pf_write(0, 0, &writtenLen);
		s_state = MENU_SDCARD_STATE_VERIFY;
		pf_lseek(0);
		s_bytesTransferred = 0;
	}
}
static void menuSdCardStateVerify()
{
	uint16_t len = menuSdCardReadBuffer();
	if (len == 0)
	{
		s_state = MENU_SDCARD_STATE_NOT_ENOUGH_DATA;
		return;
	}
	uint16_t validLen = len;
	if (s_bytesTransferred + len > MENU_SDCARD_REQUIRED_BYTES) validLen = MENU_SDCARD_REQUIRED_BYTES - s_bytesTransferred;
	eeprom_read_block(s_bufferSecond, (void*)s_bytesTransferred, validLen);
	s_bytesTransferred += len;

	for (uint16_t i = 0; i < validLen; i++)
	{
		if (s_buffer[i] != s_bufferSecond[i])
		{
			s_state = MENU_SDCARD_STATE_VERIFICATION_ERROR;
			return;
		}
	}

	stringsLoad(stringsBuffer, stringsVerifyingSdCard);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	menuSdCardWriteProgress();

	if (s_bytesTransferred >= MENU_SDCARD_REQUIRED_BYTES) s_state = MENU_SDCARD_STATE_SUCCESS;
}
static void menuSdCardStateSuccess()
{
	stringsLoad(stringsBuffer, stringsSdCardLoadSuccessTop);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	stringsLoad(stringsBuffer, stringsSdCardLoadSuccessBottom);
	interfaceDisplayWrite(0, 1, stringsBuffer);

	if (interfaceHandleAnyButtonPressed())
	{
		// load needs restart to reload all data from eeprom
		if (s_mode == MENU_SDCARD_MODE_LOAD) resetSystem(); else interfaceMenuPrevious();
	}
}
static void menuSdCardStateNoCard()
{
	stringsLoad(stringsBuffer, stringsNoSdCardTop);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	stringsLoad(stringsBuffer, stringsNoSdCardBottom);
	interfaceDisplayWrite(0, 1, stringsBuffer);

	if (interfaceHandleAnyButtonPressed()) interfaceMenuPrevious();
}
static void menuSdCardStateReadError()
{
	stringsLoad(stringsBuffer, stringsErrorWhileReadingSdCardTop);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	stringsLoad(stringsBuffer, stringsErrorWhileReadingSdCardBottom);
	interfaceDisplayWrite(0, 1, stringsBuffer);

	if (interfaceHandleAnyButtonPressed()) interfaceMenuPrevious();
}
static void menuSdCardStateNoFilesystem()
{
	stringsLoad(stringsBuffer, stringsNoFilesystemTop);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	stringsLoad(stringsBuffer, stringsNoFilesystemBottom);
	interfaceDisplayWrite(0, 1, stringsBuffer);

	if (interfaceHandleAnyButtonPressed()) interfaceMenuPrevious();
}
static void menuSdCardStateNoSettingsFile()
{
	stringsLoad(stringsBuffer, stringsNoFile);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	stringsLoad(stringsBuffer, stringsSettingsFileName);
	interfaceDisplayWrite(0, 1, stringsBuffer);

	if (interfaceHandleAnyButtonPressed()) interfaceMenuPrevious();
}
static void menuSdCardStateNotEnoughData()
{
	stringsLoad(stringsBuffer, stringsNotEnoughDataTop);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	stringsLoad(stringsBuffer, stringsNotEnoughDataBottom);
	interfaceDisplayWrite(0, 1, stringsBuffer);

	if (interfaceHandleAnyButtonPressed()) interfaceMenuPrevious();
}
static void menuSdCardStateVerificationError()
{
	stringsLoad(stringsBuffer, stringsVerificationErrorTop);
	interfaceDisplayWrite(0, 0, stringsBuffer);
	stringsLoad(stringsBuffer, stringsVerificationErrorBottom);
	interfaceDisplayWrite(0, 1, stringsBuffer);

	if (interfaceHandleAnyButtonPressed()) interfaceMenuPrevious();
}

static uint16_t menuSdCardReadBuffer()
{
	UINT len;
	FRESULT result = pf_read(&s_buffer, MENU_SDCARD_BUFFER_SIZE, &len);

	if (result == FR_DISK_ERR) s_state = MENU_SDCARD_STATE_READ_ERROR;

	return (uint8_t)len;
}
static void menuSdCardWriteProgress()
{
	uint8_t percentage = ((uint32_t)s_bytesTransferred * (uint32_t)100) / (uint32_t)MENU_SDCARD_REQUIRED_BYTES;
	sprintf(menuBuffer, MENU_SDCARD_PERCENTAGE_FORMAT, percentage);
	interfaceDisplayWrite(11, 0, menuBuffer);

	interfaceDisplayWriteChar(0, 1, MENU_SDCARD_PROGRESS_BORDER_CHAR);
	interfaceDisplayWriteChar(15, 1, MENU_SDCARD_PROGRESS_BORDER_CHAR);
	uint8_t count = percentage / (100 / 14);
	for (uint8_t i = 0; i < count; i++) interfaceDisplayWriteChar(i + 1, 1, MENU_SDCARD_PROGRESS_CHAR);
}
