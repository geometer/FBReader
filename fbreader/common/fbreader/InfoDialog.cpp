/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <algorithm>

#include <abstract/ZLEncodingConverter.h>
#include <abstract/ZLDialogManager.h>
#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLOptionEntry.h>
#include <abstract/ZLFile.h>

#include "InfoDialog.h"

#include "../hyphenation/TeXHyphenator.h"

class StringInfoEntry : public ZLStringOptionEntry {

public:
	StringInfoEntry(const std::string &name, const std::string &value);
	~StringInfoEntry();
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	std::string myName;
	std::string myValue;
};

class EncodingEntry : public ZLComboOptionEntry {

public:
	EncodingEntry(const std::string &name, ZLStringOption &encodingOption);
	~EncodingEntry();

	const std::string &name() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	std::string myName;
	ZLStringOption &myEncodingOption;
};

class LanguageEntry : public ZLComboOptionEntry {

public:
	LanguageEntry(const std::string &name, ZLStringOption &encodingOption);
	~LanguageEntry();

	const std::string &name() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	std::string myName;
	ZLStringOption &myLanguageOption;
};

static std::vector<std::string> AUTO_ENCODING;

StringInfoEntry::StringInfoEntry(const std::string &name, const std::string &value) : myName(name), myValue(value) {
	setActive(false);
}

StringInfoEntry::~StringInfoEntry() {
}

const std::string &StringInfoEntry::name() const {
	return myName;
}

const std::string &StringInfoEntry::initialValue() const {
	return myValue;
}

void StringInfoEntry::onAccept(const std::string&) {
}

static const std::string AUTO = "auto";

EncodingEntry::EncodingEntry(const std::string &name, ZLStringOption &encodingOption) : myName(name), myEncodingOption(encodingOption) {
	setActive(initialValue() != AUTO);
}

EncodingEntry::~EncodingEntry() {
}

const std::vector<std::string> &EncodingEntry::values() const {
	if (initialValue() == AUTO) {
		if (AUTO_ENCODING.empty()) {
			AUTO_ENCODING.push_back(AUTO);
		}
		return AUTO_ENCODING;
	}
	return ZLEncodingConverter::knownEncodings();
}

const std::string &EncodingEntry::name() const {
	return myName;
}

const std::string &EncodingEntry::initialValue() const {
	return myEncodingOption.value();
}

void EncodingEntry::onAccept(const std::string &value) {
	myEncodingOption.setValue(value);
}

LanguageEntry::LanguageEntry(const std::string &name, ZLStringOption &encodingOption) : myName(name), myLanguageOption(encodingOption) {
}

LanguageEntry::~LanguageEntry() {
}

const std::string &LanguageEntry::initialValue() const {
	const std::vector<std::string> &codes = TeXHyphenator::languageCodes();
	const std::vector<std::string> &names = TeXHyphenator::languageNames();
	const size_t index = std::find(codes.begin(), codes.end(), myLanguageOption.value()) - codes.begin();
	return (index < names.size()) ? names[index] : names.back();
}

const std::vector<std::string> &LanguageEntry::values() const {
	return TeXHyphenator::languageNames();
}

const std::string &LanguageEntry::name() const {
	return myName;
}

void LanguageEntry::onAccept(const std::string &value) {
	const std::vector<std::string> &codes = TeXHyphenator::languageCodes();
	const std::vector<std::string> &names = TeXHyphenator::languageNames();
	const size_t index = std::find(names.begin(), names.end(), value) - names.begin();
	myLanguageOption.setValue((index < codes.size()) ? codes[index] : codes.back());
}

InfoDialog::InfoDialog(const std::string &fileName) : myBookInfo(fileName) {
	myDialog = ZLDialogManager::instance().createOptionsDialog("InfoDialog", "FBReader - Book Info");

	ZLDialogContent &commonTab = myDialog->createTab("Common");
	commonTab.addOption(new StringInfoEntry("File", fileName));
	commonTab.addOption(new ZLSimpleStringOptionEntry("Title", myBookInfo.TitleOption));
	commonTab.addOption(new ZLSimpleStringOptionEntry("Author (display name)", myBookInfo.AuthorDisplayNameOption));
	commonTab.addOption(new ZLSimpleStringOptionEntry("Author (sort name)", myBookInfo.AuthorSortKeyOption));
	commonTab.addOption(new EncodingEntry("Encoding", myBookInfo.EncodingOption));
	commonTab.addOption(new LanguageEntry("Language", myBookInfo.LanguageOption));

	ZLDialogContent &sequenceTab = myDialog->createTab("Sequence");
	sequenceTab.addOption(new ZLSimpleStringOptionEntry("Sequence Name", myBookInfo.SequenceNameOption));
	sequenceTab.addOption(new ZLSimpleSpinOptionEntry("Number In Sequence", myBookInfo.NumberInSequenceOption, 1));

	FormatPlugin *plugin = PluginCollection::instance().plugin(ZLFile(fileName), false);
	if (plugin != 0) {
		myFormatInfoPage = plugin->createInfoPage(*myDialog, fileName);
	}
}
