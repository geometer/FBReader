/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLPaintContext.h>

#include <optionEntries/ZLToggleBooleanOptionEntry.h>
#include <optionEntries/ZLColorOptionBuilder.h>

#include "OptionsDialog.h"
#include "FormatOptionsPage.h"
#include "ScrollingOptionsPage.h"
#include "StyleOptionsPage.h"
#include "KeyBindingsPage.h"
#include "ConfigPage.h"

#include "../fbreader/FBReader.h"
#include "../fbreader/CollectionView.h"
#include "../fbreader/RecentBooksView.h"

#include "../collection/BookCollection.h"
#include "../external/ProgramCollection.h"
#include "../textview/TextView.h"
#include "../textview/TextStyle.h"
#include "../textview/TextStyleOptions.h"
#include "../formats/FormatPlugin.h"

class DictionaryEntry : public ZLComboOptionEntry {

public:
	DictionaryEntry(const ProgramCollection &collection, const std::string &name);

private:
	const std::string &name() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	const ProgramCollection &myCollection;
	const std::string myName;
};

class EnableDictionaryEntry : public ZLSimpleBooleanOptionEntry {

public:
	EnableDictionaryEntry(const std::string &name, ZLBooleanOption &option, ZLOptionEntry *comboEntry);

private:
	void onStateChanged(bool state);

private:
	ZLOptionEntry *myComboEntry;
};

DictionaryEntry::DictionaryEntry(const ProgramCollection &collection, const std::string &name) : myCollection(collection), myName(name) {
}

const std::string &DictionaryEntry::initialValue() const {
	return myCollection.CurrentNameOption.value();
}

const std::string &DictionaryEntry::name() const {
	return myName;
}

const std::vector<std::string> &DictionaryEntry::values() const {
	return myCollection.names();
}

void DictionaryEntry::onAccept(const std::string &value) {
	myCollection.CurrentNameOption.setValue(value);
}

EnableDictionaryEntry::EnableDictionaryEntry(const std::string &name, ZLBooleanOption &option, ZLOptionEntry *comboEntry) : ZLSimpleBooleanOptionEntry(name, option), myComboEntry(comboEntry) {
	onStateChanged(initialState());
}

void EnableDictionaryEntry::onStateChanged(bool state) {
	if (myComboEntry != 0) {
		myComboEntry->setVisible(state);
	}
}

void OptionsDialog::createCollectionTab(shared_ptr<ProgramCollection> collection, const std::string &name, const std::string &checkBoxPrefix, const std::string &checkBoxSuffix, const std::string &comboBoxName) {
	if (!collection.isNull()) {
		const std::vector<std::string> &dictionaryNames = collection->names();
		if (!dictionaryNames.empty()) {
			ZLDialogContent &dictionaryTab = myDialog->createTab(name);
			ZLOptionEntry *dictionaryChoiceEntry =
				(dictionaryNames.size() > 1) ? new DictionaryEntry(*collection, comboBoxName) : 0;
			const std::string optionName = checkBoxPrefix +
				((dictionaryNames.size() == 1) ? dictionaryNames[0] : checkBoxSuffix);
			dictionaryTab.addOption(new EnableDictionaryEntry(optionName, collection->EnableCollectionOption, dictionaryChoiceEntry));
			if (dictionaryChoiceEntry != 0) {
				dictionaryTab.addOption(dictionaryChoiceEntry);
			}
		}
	}
}
