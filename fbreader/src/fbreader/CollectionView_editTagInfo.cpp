/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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
#include <ZLDialog.h>
#include <ZLOptionEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>

#include "CollectionView.h"

#include "../options/FBOptions.h"
#include "../collection/BookCollection.h"

const std::string CollectionView::SpecialTagAllBooks = ",AllBooks,";
const std::string CollectionView::SpecialTagNoTagsBooks = ",NoTags,";

class EditOrCloneEntry : public ZLChoiceOptionEntry {

public:
	EditOrCloneEntry(const ZLResource &resource, int initialValue);
	const std::string &text(int index) const;
	int choiceNumber() const;
	int initialCheckedIndex() const;
	void onAccept(int index);

private:
	const ZLResource &myResource;
	const int myInitialValue;
};

EditOrCloneEntry::EditOrCloneEntry(const ZLResource &resource, int initialValue) : myResource(resource), myInitialValue(initialValue) {
}

const std::string &EditOrCloneEntry::text(int index) const {
	std::string keyName;
	switch (index) {
		case 0:
			keyName = "edit";
			break;
		case 1:
			keyName = "clone";
			break;
	}
	return myResource[keyName].value();
}

int EditOrCloneEntry::choiceNumber() const {
	return 2;
}

int EditOrCloneEntry::initialCheckedIndex() const {
	return myInitialValue;
}

void EditOrCloneEntry::onAccept(int index) {
}

class TagNameEntry : public ZLComboOptionEntry {

public:
	TagNameEntry(const std::vector<std::string> &values, const std::string &initialValue);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	const std::vector<std::string> &myValues;
	const std::string &myInitialValue;
};

TagNameEntry::TagNameEntry(const std::vector<std::string> &values, const std::string &initialValue) : ZLComboOptionEntry(true), myValues(values), myInitialValue(initialValue) {
}

const std::string &TagNameEntry::initialValue() const {
	return myInitialValue;
}

const std::vector<std::string> &TagNameEntry::values() const {
	return myValues;
}

void TagNameEntry::onAccept(const std::string &value) {
}

class IncludeSubtagsEntry : public ZLBooleanOptionEntry {

public:
	IncludeSubtagsEntry(bool initialValue);

	bool initialState() const;
	void onAccept(bool state);

private:
	bool myValue;
};

IncludeSubtagsEntry::IncludeSubtagsEntry(bool initialValue) : myValue(initialValue) {
}

bool IncludeSubtagsEntry::initialState() const {
	return myValue;
}

void IncludeSubtagsEntry::onAccept(bool state) {
	myValue = state;
}

void CollectionView::editTagInfo(const std::string &tag) {
	if (tag.empty()) {
		return;
	}
	const bool tagIsSpecial = tag[0] == ',';

	shared_ptr<ZLDialog> dialog = ZLDialogManager::instance().createDialog(ZLResourceKey("editTagInfoDialog"));

	ZLResourceKey editOrCloneKey("editOrClone");
	ZLOptionEntry *editOrCloneEntry;
	if (tagIsSpecial) {
		editOrCloneEntry = new EditOrCloneEntry(dialog->resource(editOrCloneKey), 1);
		editOrCloneEntry->setActive(false);
	} else {
		editOrCloneEntry = new EditOrCloneEntry(dialog->resource(editOrCloneKey), 0);
	}
	dialog->addOption(editOrCloneKey, editOrCloneEntry);

	std::vector<std::string> names;
	if (tagIsSpecial) {
		names.push_back("");
	}
	names.push_back("A");
	names.push_back("B");
	names.push_back("C");
	names.push_back(tag);
	ZLOptionEntry *tagNameEntry = new TagNameEntry(names, tagIsSpecial ? "" : tag);
	dialog->addOption(ZLResourceKey("name"), tagNameEntry);

	ZLOptionEntry *includeSubtagsEntry;
	if (!tagIsSpecial && myCollection.hasSubtags(tag)) {
		includeSubtagsEntry = new IncludeSubtagsEntry(true);
		if (!myCollection.hasBooks(tag)) {
			includeSubtagsEntry->setActive(false);
		}
		dialog->addOption(ZLResourceKey("includeSubtags"), includeSubtagsEntry);
	} else {
		includeSubtagsEntry = new IncludeSubtagsEntry(false);
	}

	dialog->addButton(ZLDialogManager::OK_BUTTON, true);
	dialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);

	if (dialog->run()) {
	}
}
