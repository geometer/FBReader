/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __PARAGRAPH_H__
#define __PARAGRAPH_H__

#include <abstract/ZLStringUtil.h>

#include "TextKind.h"

class BookDescription;
class Image;
class BookModel;

class ParagraphEntry {

public:
	enum Kind {
		TEXT_ENTRY,
		IMAGE_ENTRY,
		CONTROL_ENTRY,
	};

public:
	virtual ~ParagraphEntry() {}
	virtual Kind entryKind() const = 0;
};

class ControlEntry : public ParagraphEntry {

public:
	ControlEntry(TextKind kind, bool isStart) : myKind(kind), myStart(isStart) {}
	virtual ~ControlEntry() {}
	TextKind kind() const { return myKind; }
	Kind entryKind() const { return CONTROL_ENTRY; }
	bool isStart() const { return myStart; }
	virtual bool isHyperlink() const { return false; }

private:
	TextKind myKind;
	bool myStart;
};

class HyperlinkControlEntry : public ControlEntry {

public:
	HyperlinkControlEntry(TextKind kind, const std::string &label) : ControlEntry(kind, true), myLabel(label) {}
	bool isHyperlink() const { return true; }
	const std::string &label() const { return myLabel; }

private:
	std::string myLabel;
};

class TextEntry : public ParagraphEntry {

public:
	const std::string &text() const { return myText; }
	void addText(const std::string &text);
	void addText(const std::vector<std::string> &text) { ZLStringUtil::append(myText, text); }
	Kind entryKind() const { return TEXT_ENTRY; }
 
private:
	std::string myText;
};

class ImageEntry : public ParagraphEntry {

public:
	ImageEntry(const std::string &id, const BookModel &model) : myId(id), myModel(model) {}
	Kind entryKind() const { return IMAGE_ENTRY; }
	const std::string &id() const { return myId; }
	const Image *image() const;

private:
	std::string myId;
	const BookModel &myModel;
};

class Paragraph {

public:
	enum Kind {
		TEXT_PARAGRAPH,
		TREE_PARAGRAPH,
		EMPTY_LINE_PARAGRAPH,
		BEFORE_SKIP_PARAGRAPH,
		AFTER_SKIP_PARAGRAPH,
		EOS_PARAGRAPH,
	};

public:
	Paragraph(Kind kind) : myKind(kind) {}
	virtual ~Paragraph();
	Kind kind() const { return myKind; }

	void addControl(TextKind textKind, bool isStart) { myEntries.push_back(new ControlEntry(textKind, isStart)); }
	void addHyperlinkControl(TextKind textKind, const std::string &label) { myEntries.push_back(new HyperlinkControlEntry(textKind, label)); }
	void addText(const std::string &text);
	void addText(const std::vector<std::string> &text);
	void addImage(const std::string &id, const BookModel &model) { myEntries.push_back(new ImageEntry(id, model)); }

	const std::vector<ParagraphEntry*> &entries() const { return myEntries; }

private:
	Kind myKind;
	std::vector<ParagraphEntry*> myEntries;
};

class ParagraphWithReference : public Paragraph {

public:
	ParagraphWithReference() : Paragraph(TEXT_PARAGRAPH), myReference(-1) {}
	void setReference(int reference) { myReference = reference; }
	int reference() const { return myReference; }

private:
	int myReference;
};

class TreeParagraph : public Paragraph {

public:
	TreeParagraph(TreeParagraph *parent = 0) : Paragraph(TREE_PARAGRAPH), myIsOpen(false), myParent(parent) {
		if (parent != 0) {
			parent->addChild(this);
			myDepth = parent->myDepth + 1;
		} else {
			myDepth = 0;
		}
	}
	bool isOpen() const { return myIsOpen; }
	void open(bool o) { myIsOpen = o; }
	void openTree();
	int depth() const { return myDepth; }
	TreeParagraph *parent() { return myParent; }
	const std::vector<TreeParagraph*> children() const { return myChildren; }

private:
	void addChild(TreeParagraph *child) { myChildren.push_back(child); }

private:
	bool myIsOpen;
	int myDepth;
	TreeParagraph *myParent;
	std::vector<TreeParagraph*> myChildren;
};

#endif /* __PARAGRAPH_H__ */
