/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLTEXTELEMENT_H__
#define __ZLTEXTELEMENT_H__

#include <ZLImageManager.h>

#include <ZLTextKind.h>
#include <ZLTextParagraph.h>

class ZLTextElement {

protected:
	ZLTextElement();

public:
	virtual ~ZLTextElement();

	enum Kind {
		WORD_ELEMENT,
		IMAGE_ELEMENT,
		CONTROL_ELEMENT,
		FORCED_CONTROL_ELEMENT,
		INDENT_ELEMENT,
		HSPACE_ELEMENT,
		NB_HSPACE_ELEMENT,
		FIXED_HSPACE_ELEMENT,
		BEFORE_PARAGRAPH_ELEMENT,
		AFTER_PARAGRAPH_ELEMENT,
		EMPTY_LINE_ELEMENT,
	};

	virtual Kind kind() const = 0;

private:
	// assignment and copy constructor are disabled
	ZLTextElement(const ZLTextElement&);
	ZLTextElement &operator = (const ZLTextElement&);
};

class ZLTextImageElement : public ZLTextElement {

public:
	ZLTextImageElement(const std::string &id, const shared_ptr<ZLImageData> image);
	~ZLTextImageElement();
	Kind kind() const;
	const ZLImageData &image() const;
	const std::string &id() const;

private:
	const std::string myId;
	const shared_ptr<ZLImageData> myImage;
};

class ZLTextSpecialElement : public ZLTextElement {

public:
	ZLTextSpecialElement(Kind kind);
	~ZLTextSpecialElement();
	Kind kind() const;

private:
	Kind myKind;
};

class ZLTextForcedControlElement : public ZLTextElement {

public:
	ZLTextForcedControlElement(shared_ptr<ZLTextParagraphEntry> entry);
	~ZLTextForcedControlElement();
	Kind kind() const;
	const ZLTextForcedControlEntry &entry() const;

private:
	const shared_ptr<ZLTextParagraphEntry> myEntry;
};

class ZLTextFixedHSpaceElement : public ZLTextElement {

public:
	ZLTextFixedHSpaceElement(unsigned char length);
	Kind kind() const;
	unsigned char length() const;

private:
	const unsigned char myLength;
};

class ZLTextControlElement : public ZLTextElement {

private:
	ZLTextControlElement(shared_ptr<ZLTextParagraphEntry> entry);
	~ZLTextControlElement();

public:
	Kind kind() const;
	const ZLTextControlEntry &entry() const;
	ZLTextKind textKind() const;
	bool isStart() const;

private:
	const shared_ptr<ZLTextParagraphEntry> myEntry;

friend class ZLTextElementPool;
};

inline ZLTextElement::ZLTextElement() {}
inline ZLTextElement::~ZLTextElement() {}

inline ZLTextImageElement::ZLTextImageElement(const std::string &id, const shared_ptr<ZLImageData> image) : myId(id), myImage(image) {}
inline ZLTextImageElement::~ZLTextImageElement() {}
inline ZLTextElement::Kind ZLTextImageElement::kind() const { return IMAGE_ELEMENT; };
inline const ZLImageData &ZLTextImageElement::image() const { return *myImage; }
inline const std::string &ZLTextImageElement::id() const { return myId; }

inline ZLTextSpecialElement::ZLTextSpecialElement(Kind kind) : myKind(kind) {}
inline ZLTextSpecialElement::~ZLTextSpecialElement() {}
inline ZLTextElement::Kind ZLTextSpecialElement::kind() const { return myKind; };

inline ZLTextForcedControlElement::ZLTextForcedControlElement(const shared_ptr<ZLTextParagraphEntry> entry) : myEntry(entry) {}
inline ZLTextForcedControlElement::~ZLTextForcedControlElement() {}
inline ZLTextElement::Kind ZLTextForcedControlElement::kind() const { return FORCED_CONTROL_ELEMENT; };
inline const ZLTextForcedControlEntry &ZLTextForcedControlElement::entry() const { return (const ZLTextForcedControlEntry&)*myEntry; }

inline ZLTextControlElement::ZLTextControlElement(const shared_ptr<ZLTextParagraphEntry> entry) : myEntry(entry) {}
inline ZLTextControlElement::~ZLTextControlElement() {}
inline ZLTextElement::Kind ZLTextControlElement::kind() const { return CONTROL_ELEMENT; };
inline const ZLTextControlEntry &ZLTextControlElement::entry() const { return (const ZLTextControlEntry&)*myEntry; }
inline ZLTextKind ZLTextControlElement::textKind() const { return entry().kind(); }
inline bool ZLTextControlElement::isStart() const { return entry().isStart(); }

inline ZLTextFixedHSpaceElement::ZLTextFixedHSpaceElement(unsigned char length) : myLength(length) {}
inline ZLTextElement::Kind ZLTextFixedHSpaceElement::kind() const { return FIXED_HSPACE_ELEMENT; }
inline unsigned char ZLTextFixedHSpaceElement::length() const { return myLength; }

#endif /* __ZLTEXTELEMENT_H__ */
