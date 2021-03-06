#include "strings.h"

const char stringsInitialization[] PROGMEM = "Inicjalizacja...";
const char stringsSettingFactorySettingsTop[] PROGMEM = "Przywracanie";
const char stringsSettingFactorySettingsBottom[] PROGMEM = "ust. fabrycznych";

const char stringsSplashText[] PROGMEM = "II LO Kielce";

const char stringsYes[] PROGMEM = "Tak";
const char stringsNo[] PROGMEM = "Nie";
const char stringsReturn[] PROGMEM = "Powr�t";
const char stringsSave[] PROGMEM = "Zapisz";
const char stringsCancel[] PROGMEM = "Anuluj";
const char stringsOk[] PROGMEM = "Ok";
const char stringsWait[] PROGMEM = "Czekaj";
const char stringsEnable[] PROGMEM = "W��cz";
const char stringsDisable[] PROGMEM = "Wy��cz";

const char stringsJanuary[] PROGMEM = "Stycze�";
const char stringsFebuary[] PROGMEM = "Luty";
const char stringsMarch[] PROGMEM = "Marzec";
const char stringsApril[] PROGMEM = "Kwiecie�";
const char stringsMay[] PROGMEM = "Maj";
const char stringsJune[] PROGMEM = "Czerwiec";
const char stringsJuly[] PROGMEM = "Lipiec";
const char stringsAugust[] PROGMEM = "Sierpie�";
const char stringsSeptember[] PROGMEM = "Wrzesie�";
const char stringsOctober[] PROGMEM = "Pa�dziernik";
const char stringsNovember[] PROGMEM = "Listopad";
const char stringsDecember[] PROGMEM = "Grudzie�";

const char stringsSetTime[] PROGMEM = "Ustaw godzin�";
const char stringsSetDate[] PROGMEM = "Ustaw dat�";
const char stringsAutoTimeTransition[] PROGMEM = "Automatyczna zmiana czasu";
const char stringsDcf77Synchronization[] PROGMEM = "Synchronizacja z wzorcem czasu DCF77";
const char stringsBellLength[] PROGMEM = "D�ugo�� dzwonka";
const char stringsBellEnableHours[] PROGMEM = "Profile (godziny dzwonk�w)";
const char stringsChangeProfile[] PROGMEM = "Zmie� profil";
const char stringsCalendar[] PROGMEM = "Kalendarz";
const char stringsManageSettings[] PROGMEM = "Zarz�dzaj ustawieniami";

const char stringsHourFormat[] PROGMEM = "gg";
const char stringsMinuteFormat[] PROGMEM = "mm";

const char stringsYearFormat[] PROGMEM = "rrrr";
const char stringsMonthFormat[] PROGMEM = "mm";
const char stringsDayFormat[] PROGMEM = "dd";

const char stringsForLesson[] PROGMEM = "Na lekcj�";
const char stringsForBreak[] PROGMEM = "Na przerw�";

const char stringsAt[] PROGMEM = "O godz";
const char stringsFor[] PROGMEM = "Przez";
const char stringsMinutesShortened[] PROGMEM = "min";
const char stringsLastSynchronization[] PROGMEM = "Ostatnia synchronizacja";

const char stringsNever[] PROGMEM = "nigdy";

const char stringsToday[] PROGMEM = "Dzisiaj";
const char stringsTommorow[] PROGMEM = "Jutro";

const char stringsProfileNormal[] PROGMEM = "Normalne";
const char stringsProfileShortened[] PROGMEM = "Skr�cone";
const char stringsProfileOther[] PROGMEM = "Inne";
const char stringsProfileNone[] PROGMEM = "Brak";

const char stringsAddBell[] PROGMEM = "Dodaj dzwonek";
const char stringsClear[] PROGMEM = "Wyczy��";
const char stringsDoYouWantToClearBells[] PROGMEM = "Czy na pewno chcesz usun�� dzwonki?";

const char stringsNewCalendar[] PROGMEM = "Nowy kalendarz";
const char stringsDoYouWantNewCalendar[] PROGMEM = "Czy chcesz rozpocz�� nowy rok szkolny?";

const char stringsSettingsFileName[] = "dzwonek.2lo";
const char stringsInitializingSdCardTop[] PROGMEM = "Inicjalizacja";
const char stringsInitializingSdCardBottom[] PROGMEM = "karty SD";
const char stringsNoSdCardTop[] PROGMEM = "W�� kart� SD i";
const char stringsNoSdCardBottom[] PROGMEM = "spr�buj ponownie";
const char stringsErrorWhileReadingSdCardTop[] PROGMEM = "B��d podczas";
const char stringsErrorWhileReadingSdCardBottom[] PROGMEM = "odczytu karty SD";
const char stringsNoFilesystemTop[] PROGMEM = "Sformatuj kart�";
const char stringsNoFilesystemBottom[] PROGMEM = "u�ywaj�c FAT32";
const char stringsNoFile[] PROGMEM = "Brak pliku";
const char stringsVerificationErrorTop[] PROGMEM = "Weryfikacja";
const char stringsVerificationErrorBottom[] PROGMEM = "nieudana";
const char stringsNotEnoughDataTop[] PROGMEM = "Plik zawiera za";
const char stringsNotEnoughDataBottom[] PROGMEM = "ma�o danych";
const char stringsSdCardLoadSuccessTop[] PROGMEM = "Wykonano";
const char stringsSdCardLoadSuccessBottom[] PROGMEM = "pomy�lnie";
const char stringsReadingSdCard[] PROGMEM = "Odczyt";
const char stringsVerifyingSdCard[] PROGMEM = "Weryfik.";
const char stringsWritingSdCard[] PROGMEM = "Zapis";

const char stringsLoadSdCardSettings[] PROGMEM = "Odczytaj z karty pami�ci";
const char stringsSaveSdCardSettings[] PROGMEM = "Zapisz na kart� pami�ci";
const char stringsFactorySettings[] PROGMEM = "Przywr�� fabryczne";

const char stringsDoYouWantLoadFromSdCard[] PROGMEM = "Czy na pewno chcesz odczyta� ustawienia z karty SD?";
const char stringsDoYouWantSaveToSdCard[] PROGMEM = "Czy na pewno chcesz zapisa� ustawienia na kart� SD?";
const char stringsDoYouWantFactorySettings[] PROGMEM = "Czy na pewno chcesz wyczy�ci� wszystkie ustawienia?";

char stringsBuffer[STRINGS_BUFFER_SIZE];

void stringsLoad(char* buffer, const char* progmemAddress)
{
	strcpy_P(buffer, progmemAddress);
}
