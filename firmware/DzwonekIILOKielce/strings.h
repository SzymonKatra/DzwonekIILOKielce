#ifndef STRINGS_H_
#define STRINGS_H_

#include <avr/pgmspace.h>

#define STRINGS_BUFFER_SIZE 64

extern const char stringsInitialization[] PROGMEM;
extern const char stringsSettingFactorySettingsTop[] PROGMEM;
extern const char stringsSettingFactorySettingsBottom[] PROGMEM;

extern const char stringsSplashText[] PROGMEM;

extern const char stringsYes[] PROGMEM;
extern const char stringsNo[] PROGMEM;
extern const char stringsReturn[] PROGMEM;
extern const char stringsSave[] PROGMEM;
extern const char stringsCancel[] PROGMEM;
extern const char stringsOk[] PROGMEM;
extern const char stringsWait[] PROGMEM;
extern const char stringsEnable[] PROGMEM;
extern const char stringsDisable[] PROGMEM;

extern const char stringsJanuary[] PROGMEM;
extern const char stringsFebuary[] PROGMEM;
extern const char stringsMarch[] PROGMEM;
extern const char stringsApril[] PROGMEM;
extern const char stringsMay[] PROGMEM;
extern const char stringsJune[] PROGMEM;
extern const char stringsJuly[] PROGMEM;
extern const char stringsAugust[] PROGMEM;
extern const char stringsSeptember[] PROGMEM;
extern const char stringsOctober[] PROGMEM;
extern const char stringsNovember[] PROGMEM;
extern const char stringsDecember[] PROGMEM;

extern const char stringsSetTime[] PROGMEM;
extern const char stringsSetDate[] PROGMEM;
extern const char stringsAutoTimeTransition[] PROGMEM;
extern const char stringsDcf77Synchronization[] PROGMEM;
extern const char stringsBellLength[] PROGMEM;
extern const char stringsChangeProfile[] PROGMEM;
extern const char stringsBellEnableHours[] PROGMEM;
extern const char stringsCalendar[] PROGMEM;
extern const char stringsManageSettings[] PROGMEM;

extern const char stringsHourFormat[] PROGMEM;
extern const char stringsMinuteFormat[] PROGMEM;

extern const char stringsYearFormat[] PROGMEM;
extern const char stringsMonthFormat[] PROGMEM;
extern const char stringsDayFormat[] PROGMEM;

extern const char stringsAt[] PROGMEM;
extern const char stringsFor[] PROGMEM;
extern const char stringsMinutesShortened[] PROGMEM;
extern const char stringsLastSynchronization[] PROGMEM;

extern const char stringsNever[] PROGMEM;

extern const char stringsToday[] PROGMEM;
extern const char stringsTommorow[] PROGMEM;

extern const char stringsForLesson[] PROGMEM;
extern const char stringsForBreak[] PROGMEM;

extern const char stringsProfileNormal[] PROGMEM;
extern const char stringsProfileShortened[] PROGMEM;
extern const char stringsProfileOther[] PROGMEM;
extern const char stringsProfileNone[] PROGMEM;

extern const char stringsAddBell[] PROGMEM;
extern const char stringsClear[] PROGMEM;
extern const char stringsDoYouWantToClearBells[] PROGMEM;

extern const char stringsNewCalendar[] PROGMEM;
extern const char stringsDoYouWantNewCalendar[] PROGMEM;

extern const char stringsSettingsFileName[] PROGMEM;
extern const char stringsInitializingSdCardTop[];
extern const char stringsInitializingSdCardBottom[] PROGMEM;
extern const char stringsNoSdCardTop[] PROGMEM;
extern const char stringsNoSdCardBottom[] PROGMEM;
extern const char stringsErrorWhileReadingSdCardTop[] PROGMEM;
extern const char stringsErrorWhileReadingSdCardBottom[] PROGMEM;
extern const char stringsNoFilesystemTop[] PROGMEM;
extern const char stringsNoFilesystemBottom[] PROGMEM;
extern const char stringsNoFile[] PROGMEM;
extern const char stringsNotEnoughDataTop[] PROGMEM;
extern const char stringsNotEnoughDataBottom[] PROGMEM;
extern const char stringsVerificationErrorTop[] PROGMEM;
extern const char stringsVerificationErrorBottom[] PROGMEM;
extern const char stringsSdCardLoadSuccessTop[] PROGMEM;
extern const char stringsSdCardLoadSuccessBottom[] PROGMEM;
extern const char stringsReadingSdCard[] PROGMEM;
extern const char stringsVerifyingSdCard[] PROGMEM;
extern const char stringsWritingSdCard[] PROGMEM;

extern const char stringsLoadSdCardSettings[] PROGMEM;
extern const char stringsSaveSdCardSettings[] PROGMEM;
extern const char stringsFactorySettings[] PROGMEM;

extern const char stringsDoYouWantLoadFromSdCard[] PROGMEM;
extern const char stringsDoYouWantSaveToSdCard[] PROGMEM;
extern const char stringsDoYouWantFactorySettings[] PROGMEM;

extern char stringsBuffer[];

void stringsLoad(char* buffer, const char* progmemAddress);

#endif /* STRINGS_H_ */
