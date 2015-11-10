#include "menuEditMonth.h"

#include <stdio.h>
#include <string.h>
#include "../common.h"
#include "../controller.h"
#include "../interface.h"
#include "../internalStorage.h"
#include "../strings.h"
#include "menu.h"

// s_currentDay - m_maxDay will return valid option
#define MENU_EDITMONTH_SAVE 1
#define MENU_EDITMONTH_CANCEL 2
#define MENU_EDITMONTH_COUNT 2

#define menuEditMonthNeedBlink() (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS)
#define menuEditMonthReturningMenuIsVisible() (s_currentDay > s_maxDay)
#define menuEditMonthWriteSave() { stringsLoad(stringsBuffer, stringsSave); interfaceDisplayWrite(MENU_EDITMONTH_SAVE_X, MENU_EDITMONTH_SAVE_Y, stringsBuffer); }
#define menuEditMonthWriteCancel() { stringsLoad(stringsBuffer, stringsCancel); interfaceDisplayWrite(MENU_EDITMONTH_CANCEL_X, MENU_EDITMONTH_CANCEL_Y, stringsBuffer); }
#define menuEditMonthComputeNextProfile(profile) ((profile) == 0xFF ? 0 : ((profile) + 1 >= PROFILE_COUNT ? 0xFF : (profile) + 1))

static uint8_t s_minDay;
static uint8_t s_maxDay;
static uint8_t s_editingMonth;
static uint16_t s_schoolYearForThisMonth;

static uint8_t s_currentDay;
static month_t s_month;

static uint8_t s_returningMenuPreviousVisible;

static uint8_t s_multiselectMode;
static uint8_t s_multiselectCenter;

static uint8_t menuEditMonthGetCurrentX();

void menuEditMonth(uint8_t flags)
{
	if (flags & INTERFACE_MENU_INITIALIZE)
	{
		s_schoolYearForThisMonth = (uint16_t)controllerSettingsGetPointer()->schoolYear + 1900;
		if (s_editingMonth <= 7) s_schoolYearForThisMonth++;

		s_minDay = 1;
		s_maxDay = monthDays(s_editingMonth + 1, s_schoolYearForThisMonth);
		internalStorageMonthRead(s_editingMonth, &s_month);

		s_currentDay = s_minDay;

		s_multiselectMode = 0;
		s_multiselectCenter = 0;

		interfaceMenuTimerReset(0);
	}

	if (interfaceMenuTimerGet(0) >= MENU_HIGHLIGHT_TOGGLE_TICKS * 2) interfaceMenuTimerReset(0);

	if (menuEditMonthReturningMenuIsVisible())
	{
		menuEditMonthWriteSave();
		menuEditMonthWriteCancel();

		switch(s_currentDay - s_maxDay)
		{
		case MENU_EDITMONTH_SAVE:
			if (menuEditMonthNeedBlink())
				interfaceDisplayWriteChars(MENU_EDITMONTH_SAVE_X, MENU_EDITMONTH_SAVE_Y, ' ', 6);
			break;
		case MENU_EDITMONTH_CANCEL:
			if (menuEditMonthNeedBlink())
				interfaceDisplayWriteChars(MENU_EDITMONTH_CANCEL_X, MENU_EDITMONTH_CANCEL_Y, ' ', 6);
			break;
		}
	}
	else
	{
		uint8_t dayDisplayStart = (s_currentDay - (MENU_EDITMONTH_DISPLAY_DAYS_COUNT / 2) > 1 ? s_currentDay - (MENU_EDITMONTH_DISPLAY_DAYS_COUNT / 2) : 1);
		if (dayDisplayStart > s_maxDay - (MENU_EDITMONTH_DISPLAY_DAYS_COUNT - 1)) dayDisplayStart = s_maxDay - (MENU_EDITMONTH_DISPLAY_DAYS_COUNT - 1);

		for (uint8_t i = 0; i < MENU_EDITMONTH_DISPLAY_DAYS_COUNT; i++)
		{
			uint8_t x = MENU_EDITMONTH_TITLE_X + i * MENU_EDITMONTH_TITLE_OFFSET;
			uint8_t day = dayDisplayStart + i;
			sprintf(menuBuffer, MENU_DAY_FORMAT, day);
			interfaceDisplayWrite(x, MENU_EDITMONTH_TITLE_Y, menuBuffer);

			uint8_t dayOfWeek = computeDayOfWeek(day, s_editingMonth + 1, s_schoolYearForThisMonth);
			if (dayOfWeek == MONDAY)
			{
				interfaceDisplayWriteChar(x - 1, MENU_EDITMONTH_TITLE_Y, MENU_EDITMONTH_WEEK_SEPARATOR_CHAR);
				interfaceDisplayWriteChar(x - 1, MENU_EDITMONTH_PROFILE_Y, MENU_EDITMONTH_WEEK_SEPARATOR_CHAR);
			}
			else if (dayOfWeek == SUNDAY && i == MENU_EDITMONTH_DISPLAY_DAYS_COUNT - 1)
			{
				interfaceDisplayWriteChar(x + 2, MENU_EDITMONTH_TITLE_Y, MENU_EDITMONTH_WEEK_SEPARATOR_CHAR);
				interfaceDisplayWriteChar(x + 2, MENU_EDITMONTH_PROFILE_Y, MENU_EDITMONTH_WEEK_SEPARATOR_CHAR);
			}
		}

		for (uint8_t i = 0; i < MENU_EDITMONTH_DISPLAY_DAYS_COUNT; i++)
		{
			uint8_t x = MENU_EDITMONTH_PROFILE_X + i * MENU_EDITMONTH_PROFILE_OFFSET;
			uint8_t day = dayDisplayStart + i;

			//if (day == s_currentDay && !s_multiselectMode) continue;

			interfaceDisplayWriteChar(x, MENU_EDITMONTH_PROFILE_Y, menuGetProfileChar(s_month.days[day]));
		}

		if (s_multiselectMode)
		{
			if (menuEditMonthNeedBlink())
			{
				uint8_t start = s_multiselectCenter;
				uint8_t end = s_currentDay;
				int8_t len = (int8_t)start - (int8_t)end;
				if (len < 0) len = -len;

				int8_t minPos, maxPos;
				if (end > start)
				{
					maxPos = menuEditMonthGetCurrentX();
					minPos = maxPos - len * MENU_EDITMONTH_PROFILE_OFFSET;
					maxPos++; // + 1 because profile takes two spaces
					if (minPos < 0) minPos = 0;
				}
				else if (start > end)
				{
					minPos = menuEditMonthGetCurrentX();
					maxPos = minPos + len * MENU_EDITMONTH_PROFILE_OFFSET;
					if (maxPos > INTERFACE_DISPLAY_WIDTH) maxPos = INTERFACE_DISPLAY_WIDTH;
				}
				else // start == end
				{
					minPos = maxPos = menuEditMonthGetCurrentX();
				}

				for (uint8_t i = minPos; i <= maxPos; i++) interfaceDisplayWriteChar(i, MENU_EDITMONTH_PROFILE_Y, 0xFF);
			}
		}
		else
		{
			if (menuEditMonthNeedBlink())
				interfaceDisplayWriteChar(menuEditMonthGetCurrentX(), MENU_EDITMONTH_PROFILE_Y, ' ');
		}
	}

	s_returningMenuPreviousVisible = menuEditMonthReturningMenuIsVisible();

	uint8_t up = interfaceHandleButtonPressed(INTERFACE_BUTTON_UP);
	uint8_t down = interfaceHandleButtonPressed(INTERFACE_BUTTON_DOWN);
	if (up && down)
	{
		s_multiselectMode ^= 1; // toggle
		s_multiselectCenter = s_currentDay;
		interfaceButtonsLock();
	}
	else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_SELECT))
	{
		if (menuEditMonthReturningMenuIsVisible())
		{
			switch(s_currentDay - s_maxDay)
			{
			case MENU_EDITMONTH_SAVE:
				controllerMonthUpdate(s_editingMonth, &s_month);
				interfaceMenuPrevious();
				break;
			case MENU_EDITMONTH_CANCEL: interfaceMenuPrevious(); break;
			}
		}
		else
		{
			if (s_multiselectMode)
			{
				uint8_t rangeStart = s_multiselectCenter;
				uint8_t rangeEnd = s_currentDay;

				if (rangeStart > rangeEnd)
				{
					uint8_t tmp = rangeStart;
					rangeStart = rangeEnd;
					rangeEnd = tmp;
				}

				uint8_t rangeProfileCount[PROFILE_COUNT + 1]; // plus 1 because of no profile
				memset(&rangeProfileCount, 0, PROFILE_COUNT + 1);
				uint8_t bestProfile = 0;
				uint8_t oneProfileChoosen = 1;
				for (uint8_t i = rangeStart; i <= rangeEnd; i++)
				{
					uint8_t profile = s_month.days[i];

					uint8_t tabIndex = (profile == 0xFF ? PROFILE_COUNT : profile); // PROFILE_COUNT is the last index

					if (profile == 0xFF) rangeProfileCount[tabIndex]++;
					else rangeProfileCount[tabIndex]++;

					if (rangeProfileCount[tabIndex] > rangeProfileCount[bestProfile == 0xFF ? PROFILE_COUNT : bestProfile])
					{
						bestProfile = profile;
					}

					if (i > rangeStart && s_month.days[i - 1] != profile) oneProfileChoosen = 0;
				}

				uint8_t profileForAll = (oneProfileChoosen ? menuEditMonthComputeNextProfile(s_month.days[rangeStart]) : bestProfile);
				for (uint8_t i = rangeStart; i <= rangeEnd; i++) s_month.days[i] = profileForAll;
			}
			else
			{
				s_month.days[s_currentDay] = menuEditMonthComputeNextProfile(s_month.days[s_currentDay]);
			}
		}
	}
	else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_UP) || up)
	{
		s_currentDay++;
		if (s_multiselectMode && s_currentDay > s_maxDay) s_currentDay = s_maxDay;
		else if (s_currentDay > s_maxDay + MENU_EDITMONTH_COUNT) s_currentDay = s_minDay;
		if (menuEditMonthReturningMenuIsVisible())
		{
			menuEditMonthWriteSave();
			menuEditMonthWriteCancel();
		}
	}
	else if (interfaceHandleButtonPulse(INTERFACE_BUTTON_DOWN) || down)
	{
		s_currentDay--;
		if (s_multiselectMode && s_currentDay < s_minDay) s_currentDay = s_minDay;
		if (s_currentDay <= 0) s_currentDay = s_maxDay + MENU_EDITMONTH_COUNT;
		if (menuEditMonthReturningMenuIsVisible())
		{
			menuEditMonthWriteSave();
			menuEditMonthWriteCancel();
		}
	}
}

void menuEditMonthSet(uint8_t month)
{
	s_editingMonth = month;
}

static uint8_t menuEditMonthGetCurrentX()
{
	if (s_currentDay - s_minDay < MENU_EDITMONTH_DISPLAY_DAYS_COUNT / 2) return MENU_EDITMONTH_PROFILE_X + ((s_currentDay - s_minDay) * MENU_EDITMONTH_PROFILE_OFFSET);
	else if (s_maxDay - s_currentDay < MENU_EDITMONTH_DISPLAY_DAYS_COUNT / 2) return MENU_EDITMONTH_PROFILE_X + (MENU_EDITMONTH_DISPLAY_DAYS_COUNT - 1 - (s_maxDay - s_currentDay)) * MENU_EDITMONTH_PROFILE_OFFSET;

	return MENU_EDITMONTH_PROFILE_X + ((MENU_EDITMONTH_DISPLAY_DAYS_COUNT / 2) * MENU_EDITMONTH_PROFILE_OFFSET);
}
