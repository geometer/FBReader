/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "Paragraph.h"

ControlEntryPool ControlEntryPool::Pool;

ControlEntryPool::~ControlEntryPool() {
	std::map<TextKind, ControlEntry*>::iterator it;
	for (it = myStartEntries.begin(); it != myStartEntries.end(); it++) {
		delete it->second;
	}
	for (it = myEndEntries.begin(); it != myEndEntries.end(); it++) {
		delete it->second;
	}
}

ControlEntry *ControlEntryPool::controlEntry(TextKind kind, bool isStart) {
	std::map<TextKind, ControlEntry*> &entries = isStart ? myStartEntries : myEndEntries;
	std::map<TextKind, ControlEntry*>::iterator it = entries.find(kind);
	if (it != entries.end()) {
		return it->second;
	}
	ControlEntry *entry = new ControlEntry(kind, isStart);
	entries.insert(std::pair<TextKind, ControlEntry*>(kind, entry));
	return entry;
}
	
Paragraph::~Paragraph() {
	for (std::vector<ParagraphEntry*>::const_iterator it = myEntries.begin(); it != myEntries.end(); it++) {
		if (((*it)->entryKind() != ParagraphEntry::CONTROL_ENTRY) || ((ControlEntry*)*it)->isHyperlink()) {
			delete *it;
		}
	}
}

size_t Paragraph::textLength() const {
	size_t len = 0;
	for (std::vector<ParagraphEntry*>::const_iterator it = myEntries.begin(); it != myEntries.end(); it++) {
		if ((*it)->entryKind() == ParagraphEntry::TEXT_ENTRY) {
			len += ((TextEntry*)(*it))->text().length();
		}
	}
	return len;
}

void Paragraph::addNonConstText(ZLString &text) {
	if (myEntries.empty() || (myEntries.back()->entryKind() != ParagraphEntry::TEXT_ENTRY)) {
		myEntries.push_back(new TextEntry(text));
	} else {
		((TextEntry*)myEntries.back())->addText(text);
	}
}

void Paragraph::addText(const std::string &text) {
	if (myEntries.empty() || (myEntries.back()->entryKind() != ParagraphEntry::TEXT_ENTRY)) {
		myEntries.push_back(new TextEntry());
	}
	((TextEntry*)myEntries.back())->addText(text);
}

void Paragraph::addText(const ZLStringBuffer &text) {
	if (myEntries.empty() || (myEntries.back()->entryKind() != ParagraphEntry::TEXT_ENTRY)) {
		myEntries.push_back(new TextEntry());
	}
	((TextEntry*)myEntries.back())->addText(text);
}

const ZLImage *ImageEntry::image() const {
	ImageMap::const_iterator i = myMap.find(myId);
	return (i != myMap.end()) ? (*i).second : 0;
}

TreeParagraph::TreeParagraph(TreeParagraph *parent) : Paragraph(TREE_PARAGRAPH), myIsOpen(false), myParent(parent) {
	if (parent != 0) {
		parent->addChild(this);
		myDepth = parent->myDepth + 1;
	} else {
		myDepth = 0;
	}
}

void TreeParagraph::openTree() {
	for (TreeParagraph *p = parent(); p != 0; p = p->parent()) {
		p->open(true);
	}
}
