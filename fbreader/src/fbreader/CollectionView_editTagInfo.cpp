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
#include <ZLApplication.h>
#include <ZLStringUtil.h>

#include "CollectionView.h"
#include "CollectionModel.h"

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
	int myIndex;
};

EditOrCloneEntry::EditOrCloneEntry(const ZLResource &resource, int initialValue) : myResource(resource), myIndex(initialValue) {
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
	return myIndex;
}

void EditOrCloneEntry::onAccept(int index) {
	myIndex = index;
}

class TagNameEntry : public ZLComboOptionEntry {

public:
	TagNameEntry(const std::vector<std::string> &values, const std::string &initialValue);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	const std::vector<std::string> &myValues;
	std::string myValue;
};

TagNameEntry::TagNameEntry(const std::vector<std::string> &values, const std::string &initialValue) : ZLComboOptionEntry(true), myValues(values), myValue(initialValue) {
}

const std::string &TagNameEntry::initialValue() const {
	return myValue;
}

const std::vector<std::string> &TagNameEntry::values() const {
	return myValues;
}

void TagNameEntry::onAccept(const std::string &value) {
	myValue = value;
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
	const bool tagIsSpecial = (tag == SpecialTagAllBooks) || (tag == SpecialTagNoTagsBooks);

	shared_ptr<ZLDialog> dialog = ZLDialogManager::instance().createDialog(ZLResourceKey("editTagInfoDialog"));

	ZLResourceKey editOrCloneKey("editOrClone");
	EditOrCloneEntry *editOrCloneEntry;
	if (tag == SpecialTagAllBooks) {
		editOrCloneEntry = new EditOrCloneEntry(dialog->resource(editOrCloneKey), 1);
		editOrCloneEntry->setActive(false);
	} else if (tag == SpecialTagNoTagsBooks) {
		editOrCloneEntry = new EditOrCloneEntry(dialog->resource(editOrCloneKey), 0);
		editOrCloneEntry->setActive(false);
	} else {
		editOrCloneEntry = new EditOrCloneEntry(dialog->resource(editOrCloneKey), 0);
	}
	dialog->addOption(editOrCloneKey, editOrCloneEntry);

	std::vector<std::string> names;
	if (tagIsSpecial) {
		names.push_back("");
	}

	std::set<std::string> tagSet;
	const Books &books = myCollection.books();
	for (Books::const_iterator it = books.begin(); it != books.end(); ++it) {
		const std::vector<std::string> &bookTags = (*it)->tags();
		tagSet.insert(bookTags.begin(), bookTags.end());
	}
	std::set<std::string> fullTagSet = tagSet;
	for (std::set<std::string>::const_iterator it = tagSet.begin(); it != tagSet.end(); ++it) {
		for (int index = 0;;) {
			index = it->find('/', index);
			if (index == -1) {
				break;
			}
			fullTagSet.insert(it->substr(0, index++));
		}
	}
	names.insert(names.end(), fullTagSet.begin(), fullTagSet.end());
	TagNameEntry *tagNameEntry = new TagNameEntry(names, tagIsSpecial ? "" : tag);
	dialog->addOption(ZLResourceKey("name"), tagNameEntry);

	IncludeSubtagsEntry *includeSubtagsEntry;
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

	while (dialog->run()) {
		dialog->acceptValues();
		std::string newTag = tagNameEntry->initialValue();
		ZLStringUtil::stripWhiteSpaces(newTag);
		if (newTag.empty()) {
			ZLDialogManager::instance().errorBox(ZLResourceKey("tagMustBeNonEmpty"));
			continue;
		}
		if (newTag.find(',') != (size_t)-1) {
			ZLDialogManager::instance().errorBox(ZLResourceKey("tagMustNotContainComma"));
			continue;
		}
		const bool includeSubtags = includeSubtagsEntry->initialState();
		collectionModel().removeAllMarks();
		if (tag == SpecialTagAllBooks) {
			myCollection.addTagToAllBooks(newTag);
		} else if (tag == SpecialTagNoTagsBooks) {
			myCollection.addTagToBooksWithNoTags(newTag);
		} else if (editOrCloneEntry->initialCheckedIndex() == 0) {
			myCollection.renameTag(tag, newTag, includeSubtags);
		} else {
			myCollection.cloneTag(tag, newTag, includeSubtags);
		}
		updateModel();
		application().refreshWindow();
		break;
	}
}
