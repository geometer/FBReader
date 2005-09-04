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
#include "RowMemoryAllocator.h"

const shared_ptr<ParagraphEntry> Paragraph::Iterator::entry() const {
	switch (**myIterator) {
		case ParagraphEntry::TEXT_ENTRY:
			return new TextEntry(*myIterator + 1);
		case ParagraphEntry::HYPERLINK_CONTROL_ENTRY:
			return new HyperlinkControlEntry(*myIterator + 1);
		default:
			return *(shared_ptr<ParagraphEntry>*)(*myIterator + 1);
	}
}

void Paragraph::addText(const std::string &text, RowMemoryAllocator &allocator) {
	size_t len = text.length();
	char *address = (char*)allocator.allocate(len + sizeof(size_t) + 1);
	*address = ParagraphEntry::TEXT_ENTRY;
	memcpy(address + 1, &len, sizeof(size_t));
	memcpy(address + sizeof(size_t) + 1, text.data(), len);
	myEntryAddress.push_back(address);
}

void Paragraph::addText(const std::vector<std::string> &text, RowMemoryAllocator &allocator) {
	size_t len = 0;
	for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); it++) {
		len += it->length();
	}
	char *address = (char*)allocator.allocate(len + sizeof(size_t) + 1);
	*address = ParagraphEntry::TEXT_ENTRY;
	memcpy(address + 1, &len, sizeof(size_t));
	size_t offset = sizeof(size_t) + 1;
	for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); it++) {
		memcpy(address + offset, it->data(), it->length());
		offset += it->length();
	}
	myEntryAddress.push_back(address);
}

static shared_ptr<ParagraphEntry> NULL_PTR = 0;

void Paragraph::addControl(TextKind textKind, bool isStart, RowMemoryAllocator &allocator) {
	char *address = (char*)allocator.allocate(sizeof(shared_ptr<ParagraphEntry>) + 1);
	*address = ParagraphEntry::CONTROL_ENTRY;
	memcpy(address + 1, &NULL_PTR, sizeof(shared_ptr<ParagraphEntry>));
	*(shared_ptr<ParagraphEntry>*)(address + 1) = ControlEntryPool::Pool.controlEntry(textKind, isStart);
	myEntryAddress.push_back(address);
}

void Paragraph::addControl(ForcedControlEntry *entry, RowMemoryAllocator &allocator) {
	char *address = (char*)allocator.allocate(sizeof(shared_ptr<ParagraphEntry>) + 1);
	*address = ParagraphEntry::FORCED_CONTROL_ENTRY;
	memcpy(address + 1, &NULL_PTR, sizeof(shared_ptr<ParagraphEntry>));
	*(shared_ptr<ParagraphEntry>*)(address + 1) = entry;
	myEntryAddress.push_back(address);
}

void Paragraph::addHyperlinkControl(TextKind textKind, const std::string &label, RowMemoryAllocator &allocator) {
	char *address = (char*)allocator.allocate(label.length() + 3);
	*address = ParagraphEntry::HYPERLINK_CONTROL_ENTRY;
	*(address + 1) = textKind;
	memcpy(address + 2, label.data(), label.length());
	*(address + label.length() + 3) = '\0';
	myEntryAddress.push_back(address);
}

void Paragraph::addImage(const std::string &id, const ImageMap &imageMap, RowMemoryAllocator &allocator) {
	char *address = (char*)allocator.allocate(sizeof(shared_ptr<ParagraphEntry>) + 1);
	*address = ParagraphEntry::IMAGE_ENTRY;
	memcpy(address + 1, &NULL_PTR, sizeof(shared_ptr<ParagraphEntry>));
	*(shared_ptr<ParagraphEntry>*)(address + 1) = new ImageEntry(id, imageMap);
	myEntryAddress.push_back(address);
}

ControlEntryPool ControlEntryPool::Pool;

shared_ptr<ParagraphEntry> ControlEntryPool::controlEntry(TextKind kind, bool isStart) {
	std::map<TextKind, shared_ptr<ParagraphEntry> > &entries = isStart ? myStartEntries : myEndEntries;
	std::map<TextKind, shared_ptr<ParagraphEntry> >::iterator it = entries.find(kind);
	if (it != entries.end()) {
		return it->second;
	}
	shared_ptr<ParagraphEntry> entry = new ControlEntry(kind, isStart);
	entries[kind] = entry;
	return entry;
}
	
size_t Paragraph::textLength() const {
	size_t len = 0;
	for (Iterator it = *this; !it.isEnd(); it.next()) {
		if (it.entryKind() == ParagraphEntry::TEXT_ENTRY) {
			len += ((TextEntry&)it.entry()).dataLength();
		}
	}
	return len;
}

const ZLImage *ImageEntry::image() const {
	ImageMap::const_iterator i = myMap.find(myId);
	return (i != myMap.end()) ? (*i).second : 0;
}

TreeParagraph::TreeParagraph(TreeParagraph *parent) : myIsOpen(false), myParent(parent) {
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
