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

#include "Paragraph.h"
#include "RowMemoryAllocator.h"

size_t TextEntry::dataLength() const {
	size_t len;
	memcpy(&len, myAddress, sizeof(size_t));
	return len;
}

ForcedControlEntry::ForcedControlEntry(char *address) {
	myMask = *address;
	memcpy(&myLeftIndent, address + 1, sizeof(short));
	memcpy(&myRightIndent, address + 1 + sizeof(short), sizeof(short));
	myAlignmentType = (AlignmentType)*(address + 1 + 2 * sizeof(short));
}

const shared_ptr<ParagraphEntry> Paragraph::Iterator::entry() const {
	if (myEntry.isNull()) {
		switch (*myPointer) {
			case ParagraphEntry::TEXT_ENTRY:
				myEntry = new TextEntry(myPointer + 1);
				break;
			case ParagraphEntry::CONTROL_ENTRY:
			{
				unsigned char token = *(myPointer + 1);
				myEntry = ControlEntryPool::Pool.controlEntry((TextKind)(token >> 1), (token & 1) == 1);
				break;
			}
			case ParagraphEntry::HYPERLINK_CONTROL_ENTRY:
				myEntry = new HyperlinkControlEntry(myPointer + 1);
				break;
			case ParagraphEntry::IMAGE_ENTRY:
			{
				ImageMap *imageMap = 0;
				memcpy(&imageMap, myPointer + 1, sizeof(const ImageMap*));
				myEntry = new ImageEntry(myPointer + sizeof(const ImageMap*) + 1, imageMap);
				break;
			}
			case ParagraphEntry::FORCED_CONTROL_ENTRY:
				myEntry = new ForcedControlEntry(myPointer + 1);
				break;
			case ParagraphEntry::FIXED_HSPACE_ENTRY:
				myEntry = new FixedHSpaceEntry((unsigned char)*(myPointer + 1));
				break;
		}
	}
	return myEntry;
}

void Paragraph::Iterator::next() {
	++myIndex;
	myEntry = 0;
	if (myIndex != myEndIndex) {
		switch (*myPointer) {
			case ParagraphEntry::TEXT_ENTRY:
			{
				size_t len;
				memcpy(&len, myPointer + 1, sizeof(size_t));
				myPointer += len + sizeof(size_t) + 1;
				break;
			}
			case ParagraphEntry::CONTROL_ENTRY:
				myPointer += 2;
				break;
			case ParagraphEntry::HYPERLINK_CONTROL_ENTRY:
				myPointer += 2;
				while (*myPointer != '\0') {
					++myPointer;
				}
				++myPointer;
				break;
			case ParagraphEntry::IMAGE_ENTRY:
				myPointer += sizeof(const ImageMap*) + 1;
				while (*myPointer != '\0') {
					++myPointer;
				}
				++myPointer;
				break;
			case ParagraphEntry::FORCED_CONTROL_ENTRY:
				myPointer += 2 * sizeof(short) + 3;
				break;
			case ParagraphEntry::FIXED_HSPACE_ENTRY:
				myPointer += 2;
				break;
		}
		if (*myPointer == 0) {
			memcpy(&myPointer, myPointer + 1, sizeof(char*));
		}
	}
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
			len += ((TextEntry&)*it.entry()).dataLength();
		}
	}
	return len;
}

const ZLImage *ImageEntry::image() const {
	ImageMap::const_iterator i = myMap->find(myId);
	return (i != myMap->end()) ? (*i).second : 0;
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

void TreeParagraph::removeFromParent() {
	if (myParent != 0) {
		myParent->myChildren.erase(std::find(myParent->myChildren.begin(), myParent->myChildren.end(), this));
	}
}

int TreeParagraph::fullSize() const {
	int size = 1;
	for (std::vector<TreeParagraph*>::const_iterator it = myChildren.begin(); it != myChildren.end(); ++it) {
		size += (*it)->fullSize();
	}
	return size;
}
