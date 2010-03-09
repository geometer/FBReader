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

#include "LibraryNodes.h"
#include "BooksUtil.h"

#include "../library/Library.h"
#include "../library/Tag.h"
#include "../library/Lists.h"
#include "../library/Comparators.h"

class TagNode::EditOrCloneAction : public ZLRunnableWithKey {

public:
	EditOrCloneAction(TagNode &node);

private:
	void run();

protected:
	virtual void onAccept(const std::string &name, bool includeSubTags) = 0;
	virtual std::string resourceKeyName() const = 0;

protected:
	TagNode &myTagNode;
};

class TagNode::EditAction : public EditOrCloneAction {

public:
	EditAction(TagNode &node);

private:
	ZLResourceKey key() const;
	void onAccept(const std::string &name, bool includeSubTags);
	std::string resourceKeyName() const;
};

class TagNode::CloneAction : public EditOrCloneAction {

public:
	CloneAction(TagNode &node);

private:
	ZLResourceKey key() const;
	void onAccept(const std::string &name, bool includeSubTags);
	std::string resourceKeyName() const;
};

class TagNode::RemoveAction : public ZLRunnableWithKey {

public:
	RemoveAction(shared_ptr<Tag> tag);

private:
	void run();
	ZLResourceKey key() const;

private:
	const shared_ptr<Tag> myTag;
};

class TagNode::NameEntry : public ZLComboOptionEntry {

public:
	NameEntry(const std::vector<std::string> &values, const std::string &initialValue);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	const std::vector<std::string> &myValues;
	std::string myValue;
};

class TagNode::IncludeSubtagsEntry : public ZLBooleanOptionEntry {

public:
	IncludeSubtagsEntry();

	bool initialState() const;
	void onAccept(bool state);

private:
	bool myValue;
};

const ZLTypeId TagNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLTypeId &TagNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &TagNode::resource() const {
	return ZLResource::resource("libraryView")["tagNode"];
}

size_t TagNode::positionToInsert(ZLBlockTreeNode *parent, shared_ptr<Tag> tag) {
	const ZLBlockTreeNode::List &children = parent->children();
	ZLBlockTreeNode::List::const_reverse_iterator it = children.rbegin();
	for (; it != children.rend(); ++it) {
		if (!(*it)->isInstanceOf(TagNode::TYPE_ID) ||
				TagComparator()(((TagNode*)*it)->tag(), tag)) {
			break;
		}
	}
	return children.rend() - it;
}

TagNode::TagNode(ZLBlockTreeView::RootNode *parent, shared_ptr<Tag> tag) : FBReaderNode(parent, positionToInsert(parent, tag)), myTag(tag) {
}

void TagNode::init() {
	registerExpandTreeAction();
	if (!myTag.isNull()) {
		registerAction(new EditAction(*this));
		registerAction(new CloneAction(*this));
		registerAction(new RemoveAction(myTag));
	}
}

TagNode::TagNode(TagNode *parent, shared_ptr<Tag> tag) : FBReaderNode(parent, positionToInsert(parent, tag)), myTag(tag) {
}

shared_ptr<Tag> TagNode::tag() const {
	return myTag;
}

std::string TagNode::title() const {
	if (myTag.isNull()) {
		return resource()["noTags"].value();
	}
	return myTag->name();
}

TagNode::EditOrCloneAction::EditOrCloneAction(TagNode &node) : myTagNode(node) {
}

void TagNode::EditOrCloneAction::run() {
	shared_ptr<ZLDialog> dialog = ZLDialogManager::Instance().createDialog(ZLResourceKey(resourceKeyName()));

	TagList tags;
	BooksUtil::collectTagsFromLibrary(tags);
	std::vector<std::string> names;
	for (TagList::const_iterator it = tags.begin(); it != tags.end(); ++it) {
		if (!it->isNull()) {
			names.push_back((*it)->fullName());
		}
	}
	NameEntry *tagNameEntry = new NameEntry(names, myTagNode.myTag->fullName());
	dialog->addOption(ZLResourceKey("name"), tagNameEntry);

	const ZLBlockTreeNode::List &children = myTagNode.children();
	IncludeSubtagsEntry *includeSubtagsEntry = new IncludeSubtagsEntry();
	if (!children.empty() &&
			children.back()->isInstanceOf(TagNode::TYPE_ID)) {
		if (!children.front()->isInstanceOf(BookNode::TYPE_ID)) {
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

TagNode::EditAction::EditAction(TagNode &node) : EditOrCloneAction(node) {
}

void TagNode::EditAction::onAccept(const std::string &name, bool includeSubTags) {
	Library::Instance().renameTag(myTagNode.myTag, Tag::getTagByFullName(name), includeSubTags);
}

ZLResourceKey TagNode::EditAction::key() const {
	return ZLResourceKey("edit");
}

std::string TagNode::EditAction::resourceKeyName() const {
	return "editTagDialog";
}

TagNode::CloneAction::CloneAction(TagNode &node) : EditOrCloneAction(node) {
}

void TagNode::CloneAction::onAccept(const std::string &name, bool includeSubTags) {
	Library::Instance().cloneTag(myTagNode.myTag, Tag::getTagByFullName(name), includeSubTags);
}

ZLResourceKey TagNode::CloneAction::key() const {
	return ZLResourceKey("clone");
}

std::string TagNode::CloneAction::resourceKeyName() const {
	return "cloneTagDialog";
}

TagNode::RemoveAction::RemoveAction(shared_ptr<Tag> tag) : myTag(tag) {
}

void TagNode::RemoveAction::run() {
	BooksUtil::removeTag(myTag);
}

ZLResourceKey TagNode::RemoveAction::key() const {
	return ZLResourceKey("delete");
}

shared_ptr<ZLImage> TagNode::extractCoverImage() const {
	return defaultCoverImage("booktree-tag.png");
}

TagNode::NameEntry::NameEntry(const std::vector<std::string> &values, const std::string &initialValue) : ZLComboOptionEntry(true), myValues(values), myValue(initialValue) {
}

const std::string &TagNode::NameEntry::initialValue() const {
	return myValue;
}

const std::vector<std::string> &TagNode::NameEntry::values() const {
	return myValues;
}

void TagNode::NameEntry::onAccept(const std::string &value) {
	myValue = value;
}

TagNode::IncludeSubtagsEntry::IncludeSubtagsEntry() : myValue(true) {
}

bool TagNode::IncludeSubtagsEntry::initialState() const {
	return myValue;
}

void TagNode::IncludeSubtagsEntry::onAccept(bool state) {
	myValue = state;
}
