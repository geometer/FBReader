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

#ifndef __TEXTELEMENT_H__
#define __TEXTELEMENT_H__

#include "../model/TextKind.h"
#include "../model/Paragraph.h"

class TextElement {

protected:
	TextElement() {}

public:
	virtual ~TextElement() {}

	enum Kind {
		WORD_ELEMENT,
		IMAGE_ELEMENT,
		INDENT_ELEMENT,
		HSPACE_ELEMENT,
		BEFORE_PARAGRAPH_ELEMENT,
		AFTER_PARAGRAPH_ELEMENT,
		EMPTY_LINE_ELEMENT,
		CONTROL_ELEMENT,
		TREE_ELEMENT,
	};

	virtual Kind kind() const = 0;

private:
	// assignment and copy constructor are disabled
	TextElement(const TextElement&);
	TextElement &operator = (const TextElement&);
};

class Image;

class ImageElement : public TextElement {

public:
	ImageElement(const Image &image) : myImage(image) {}
	Kind kind() const { return IMAGE_ELEMENT; };
	const Image &image() const { return myImage; }

private:
	const Image &myImage;
};

class SpecialTextElement : public TextElement {

public:
	SpecialTextElement(Kind kind) : myKind(kind) {}
	Kind kind() const { return myKind; };

private:
	Kind myKind;
};

class ControlElement : public TextElement {

public:
	ControlElement(const ControlEntry &entry) : myEntry(entry) {}
	Kind kind() const { return CONTROL_ELEMENT; };
	const ControlEntry &entry() const { return myEntry; }
	TextKind textKind() const { return myEntry.kind(); }
	bool isStart() const { return myEntry.isStart(); }

private:
	const ControlEntry &myEntry;
};

class TreeElement : public TextElement {

public:
	enum TreeElementKind {
		TREE_ELEMENT_OPEN_NODE,
		TREE_ELEMENT_CLOSED_NODE,
		TREE_ELEMENT_LEAF,
		TREE_ELEMENT_TOP_RIGHT_LINE,
		TREE_ELEMENT_TOP_BOTTOM_RIGHT_LINE,
		TREE_ELEMENT_SKIP,
	};

	TreeElement(TreeElementKind treeElementKind) : myTreeElementKind(treeElementKind) {}
	Kind kind() const { return TREE_ELEMENT; };
	TreeElementKind treeElementKind() const { return myTreeElementKind; }

private:
	TreeElementKind myTreeElementKind;
};

#endif /* __TEXTELEMENT_H__ */
