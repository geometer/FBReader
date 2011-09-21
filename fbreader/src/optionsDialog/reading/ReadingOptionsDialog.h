/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __READINGOPTIONSDIALOG_H__
#define __READINGOPTIONSDIALOG_H__

#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include "../AbstractOptionsDialog.h"

class ReadingOptionsDialog : public AbstractOptionsDialog {

private:
	void createIndicatorTab();
	void createKeyBindingsTab();

public:
	ReadingOptionsDialog();
};

class RotationTypeEntry : public ZLComboOptionEntry {

public:
	RotationTypeEntry(const ZLResource &resource, ZLIntegerOption &angleOption);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	ZLIntegerOption &myAngleOption;
	std::vector<std::string> myValues;
};

class StateOptionEntry : public ZLToggleBooleanOptionEntry {

public:
	StateOptionEntry(ZLBooleanOption &option);
	void onStateChanged(bool state);

private:
	bool myState;

friend class SpecialFontSizeEntry;
};

class SpecialFontSizeEntry : public ZLSimpleSpinOptionEntry {

public:
	SpecialFontSizeEntry(ZLIntegerRangeOption &option, int step, StateOptionEntry &first, StateOptionEntry &second);
	void setVisible(bool state);

private:
	StateOptionEntry &myFirst;
	StateOptionEntry &mySecond;
};

class IndicatorTypeEntry : public ZLComboOptionEntry {

public:
	IndicatorTypeEntry(const ZLResource &resource, ZLIntegerRangeOption &typeOption);
	void addDependentEntry(ZLOptionEntry *entry);
	const std::string &initialValue() const;

private:
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);
	void onValueSelected(int index);

private:
	ZLIntegerRangeOption &myOption;
	std::vector<std::string> myValues;
	std::vector<ZLOptionEntry*> myDependentEntries;
};

#endif /* __READINGOPTIONSDIALOG_H__ */
