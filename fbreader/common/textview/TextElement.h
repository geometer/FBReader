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

#ifndef __TEXTELEMENT_H__
#define __TEXTELEMENT_H__

#include <abstract/ZLImageManager.h>

#include "../model/TextKind.h"
#include "../model/Paragraph.h"

class TextElement {

protected:
	TextElement() VIEW_SECTION;

public:
	virtual ~TextElement() VIEW_SECTION;

	enum Kind {
		WORD_ELEMENT,
		IMAGE_ELEMENT,
		CONTROL_ELEMENT,
		FORCED_CONTROL_ELEMENT,
		TREE_ELEMENT,
		INDENT_ELEMENT,
		HSPACE_ELEMENT,
		BEFORE_PARAGRAPH_ELEMENT,
		AFTER_PARAGRAPH_ELEMENT,
		EMPTY_LINE_ELEMENT,
	};

	virtual Kind kind() const VIEW_SECTION = 0;

private:
	// assignment and copy constructor are disabled
	TextElement(const TextElement&) VIEW_SECTION;
	TextElement &operator = (const TextElement&) VIEW_SECTION;
};

class ImageElement : public TextElement {

public:
	ImageElement(const shared_ptr<ZLImageData> image) VIEW_SECTION;
	~ImageElement() VIEW_SECTION;
	Kind kind() const VIEW_SECTION;
	const ZLImageData &image() const VIEW_SECTION;

private:
	const shared_ptr<ZLImageData> myImage;
};

class SpecialTextElement : public TextElement {

public:
	SpecialTextElement(Kind kind) VIEW_SECTION;
	~SpecialTextElement() VIEW_SECTION;
	Kind kind() const VIEW_SECTION;

private:
	Kind myKind;
};

class ForcedControlElement : public TextElement {

public:
	ForcedControlElement(const ForcedControlEntry &entry) VIEW_SECTION;
	~ForcedControlElement() VIEW_SECTION;
	Kind kind() const VIEW_SECTION;
	const ForcedControlEntry &entry() const VIEW_SECTION;

private:
	const ForcedControlEntry &myEntry;
};

class ControlElement : public TextElement {

public:
	ControlElement(const ControlEntry &entry) VIEW_SECTION;
	~ControlElement() VIEW_SECTION;
	Kind kind() const VIEW_SECTION;
	const ControlEntry &entry() const VIEW_SECTION;
	TextKind textKind() const VIEW_SECTION;
	bool isStart() const VIEW_SECTION;

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

	TreeElement(TreeElementKind treeElementKind) VIEW_SECTION;
	~TreeElement() VIEW_SECTION;
	Kind kind() const VIEW_SECTION;
	TreeElementKind treeElementKind() const VIEW_SECTION;

private:
	TreeElementKind myTreeElementKind;
};

inline TextElement::TextElement() {}
inline TextElement::~TextElement() {}

inline ImageElement::ImageElement(const shared_ptr<ZLImageData> image) : myImage(image) {}
inline ImageElement::~ImageElement() {}
inline TextElement::Kind ImageElement::kind() const { return IMAGE_ELEMENT; };
inline const ZLImageData &ImageElement::image() const { return *myImage; }

inline SpecialTextElement::SpecialTextElement(Kind kind) : myKind(kind) {}
inline SpecialTextElement::~SpecialTextElement() {}
inline TextElement::Kind SpecialTextElement::kind() const { return myKind; };

inline ForcedControlElement::ForcedControlElement(const ForcedControlEntry &entry) : myEntry(entry) {}
inline ForcedControlElement::~ForcedControlElement() {}
inline TextElement::Kind ForcedControlElement::kind() const { return FORCED_CONTROL_ELEMENT; };
inline const ForcedControlEntry &ForcedControlElement::entry() const { return myEntry; }

inline ControlElement::ControlElement(const ControlEntry &entry) : myEntry(entry) {}
inline ControlElement::~ControlElement() {}
inline TextElement::Kind ControlElement::kind() const { return CONTROL_ELEMENT; };
inline const ControlEntry &ControlElement::entry() const { return myEntry; }
inline TextKind ControlElement::textKind() const { return myEntry.kind(); }
inline bool ControlElement::isStart() const { return myEntry.isStart(); }

inline TreeElement::TreeElement(TreeElementKind treeElementKind) : myTreeElementKind(treeElementKind) {}
inline TreeElement::~TreeElement() {}
inline TextElement::Kind TreeElement::kind() const { return TREE_ELEMENT; };
inline TreeElement::TreeElementKind TreeElement::treeElementKind() const { return myTreeElementKind; }

#endif /* __TEXTELEMENT_H__ */
