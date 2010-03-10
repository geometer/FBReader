/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLResource.h>
#include <ZLDialogManager.h>
#include <ZLDialog.h>
#include <ZLOptionEntry.h>

#include "LibraryTagActions.h"

#include "BooksUtil.h"

#include "../library/Library.h"
#include "../library/Tag.h"
#include "../library/Lists.h"
//#include "../library/Comparators.h"

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

class TagIncludeSubtagsEntry : public ZLBooleanOptionEntry {

public:
	TagIncludeSubtagsEntry();

	bool initialState() const;
	void onAccept(bool state);

private:
	bool myValue;
};

TagEditOrCloneAction::TagEditOrCloneAction(shared_ptr<Tag> tag) : myTag(tag) {
}

void TagEditOrCloneAction::run() {
	shared_ptr<ZLDialog> dialog = ZLDialogManager::Instance().createDialog(ZLResourceKey(resourceKeyName()));

	TagList tags;
	BooksUtil::collectTagsFromLibrary(tags);
	std::vector<std::string> names;
	for (TagList::const_iterator it = tags.begin(); it != tags.end(); ++it) {
		if (!it->isNull()) {
			names.push_back((*it)->fullName());
		}
	}
	TagNameEntry *tagNameEntry = new TagNameEntry(names, myTag->fullName());
	dialog->addOption(ZLResourceKey("name"), tagNameEntry);

	TagIncludeSubtagsEntry *includeSubtagsEntry = new TagIncludeSubtagsEntry();
	const Library &library = Library::Instance();
	if (library.hasSubtags(myTag)) {
		if (!library.hasBooks(myTag)) {
			includeSubtagsEntry->setActive(false);
		}
		dialog->addOption(ZLResourceKey("includeSubtags"), includeSubtagsEntry);
	}

	dialog->addButton(ZLDialogManager::OK_BUTTON, true);
	dialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);

	if (dialog->run()) {
		dialog->acceptValues();
		onAccept(tagNameEntry->initialValue(), includeSubtagsEntry->initialState());
	}
}

TagEditAction::TagEditAction(shared_ptr<Tag> tag) : TagEditOrCloneAction(tag) {
}

void TagEditAction::onAccept(const std::string &name, bool includeSubTags) {
	Library::Instance().renameTag(myTag, Tag::getTagByFullName(name), includeSubTags);
}

ZLResourceKey TagEditAction::key() const {
	return ZLResourceKey("edit");
}

std::string TagEditAction::resourceKeyName() const {
	return "editTagDialog";
}

TagCloneAction::TagCloneAction(shared_ptr<Tag> tag) : TagEditOrCloneAction(tag) {
}

void TagCloneAction::onAccept(const std::string &name, bool includeSubTags) {
	Library::Instance().cloneTag(myTag, Tag::getTagByFullName(name), includeSubTags);
}

ZLResourceKey TagCloneAction::key() const {
	return ZLResourceKey("clone");
}

std::string TagCloneAction::resourceKeyName() const {
	return "cloneTagDialog";
}

TagRemoveAction::TagRemoveAction(shared_ptr<Tag> tag) : myTag(tag) {
}

void TagRemoveAction::run() {
	BooksUtil::removeTag(myTag);
}

ZLResourceKey TagRemoveAction::key() const {
	return ZLResourceKey("delete");
}

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

TagIncludeSubtagsEntry::TagIncludeSubtagsEntry() : myValue(true) {
}

bool TagIncludeSubtagsEntry::initialState() const {
	return myValue;
}

void TagIncludeSubtagsEntry::onAccept(bool state) {
	myValue = state;
}
