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

#ifndef __PARAGRAPH_H__
#define __PARAGRAPH_H__

#include <map>
#include <vector>
#include <string>

#include <shared_ptr.h>

#include "TextKind.h"
#include "AlignmentType.h"

class BookDescription;
class ZLImage;
typedef std::map<std::string,ZLImage*> ImageMap;
class RowMemoryAllocator;

class ParagraphEntry {

public:
	enum Kind {
		TEXT_ENTRY = 1,
		IMAGE_ENTRY = 2,
		CONTROL_ENTRY = 3,
		HYPERLINK_CONTROL_ENTRY = 4,
		FORCED_CONTROL_ENTRY = 5,
		FIXED_HSPACE_ENTRY = 6,
	};

protected:
	ParagraphEntry();

public:
	virtual ~ParagraphEntry();

private: // disable copying
	ParagraphEntry(const ParagraphEntry &entry);
	const ParagraphEntry &operator = (const ParagraphEntry &entry);
};

class ForcedControlEntry : public ParagraphEntry {

public:
	ForcedControlEntry();
	ForcedControlEntry(char *address);
	~ForcedControlEntry();

	bool leftIndentSupported() const;
	short leftIndent() const;
	void setLeftIndent(short leftIndent);

	bool rightIndentSupported() const;
	short rightIndent() const;
	void setRightIndent(short rightIndent);

	bool alignmentTypeSupported() const;
	AlignmentType alignmentType() const;
	void setAlignmentType(AlignmentType alignmentType);

	enum {
		SUPPORT_LEFT_INDENT = 1,
		SUPPORT_RIGHT_INDENT = 2,
		SUPPORT_ALIGNMENT_TYPE = 4,
	};

private:
	int myMask;

	short myLeftIndent;
	short myRightIndent;
	AlignmentType myAlignmentType;

friend class TextModel;
};

class ControlEntry : public ParagraphEntry {

protected:
	ControlEntry(TextKind kind, bool isStart);

public:
	virtual ~ControlEntry();
	TextKind kind() const;
	bool isStart() const;
	virtual bool isHyperlink() const;

private:
	TextKind myKind;
	bool myStart;

friend class ControlEntryPool;
};

class FixedHSpaceEntry : public ParagraphEntry {

public:
	FixedHSpaceEntry(unsigned char length);
	unsigned char length() const;

private:
	const unsigned char myLength;
};

class ControlEntryPool {

public:
	static ControlEntryPool Pool;
	
public:
	ControlEntryPool();
	~ControlEntryPool();
	shared_ptr<ParagraphEntry> controlEntry(TextKind kind, bool isStart);

private:
	std::map<TextKind, shared_ptr<ParagraphEntry> > myStartEntries;
	std::map<TextKind, shared_ptr<ParagraphEntry> > myEndEntries;
};

class HyperlinkControlEntry : public ControlEntry {

public:
	HyperlinkControlEntry(const char *address);
	~HyperlinkControlEntry();
	const std::string &label() const;
	bool isHyperlink() const;

private:
	std::string myLabel;
};

class TextEntry : public ParagraphEntry {

public:
	TextEntry(const char *address);
	~TextEntry();

	size_t dataLength() const;
	const char *data() const;
 
private:
	const char *myAddress;
};

class ImageEntry : public ParagraphEntry {

public:
	ImageEntry(const std::string &id, const ImageMap *imageMap);
	~ImageEntry();
	const std::string &id() const;
	const ZLImage *image() const;

private:
	std::string myId;
	const ImageMap *myMap;
};

class Paragraph {

public:
	class Iterator {

	public:
		Iterator(const Paragraph &paragraph);
		~Iterator();

		bool isEnd() const;
		void next();
		const shared_ptr<ParagraphEntry> entry() const;
		ParagraphEntry::Kind entryKind() const;

	private:
		char *myPointer;
		size_t myIndex;
		size_t myEndIndex;
		mutable shared_ptr<ParagraphEntry> myEntry;
	};

	enum Kind {
		TEXT_PARAGRAPH,
		TREE_PARAGRAPH,
		EMPTY_LINE_PARAGRAPH,
		BEFORE_SKIP_PARAGRAPH,
		AFTER_SKIP_PARAGRAPH,
		END_OF_SECTION_PARAGRAPH,
		END_OF_TEXT_PARAGRAPH,
	};

protected:
	Paragraph();

public:
	virtual ~Paragraph();
	virtual Kind kind() const;

	size_t entryNumber() const;

	size_t textLength() const;

private:
	void addEntry(char *address);

private:
	char *myFirstEntryAddress;
	size_t myEntryNumber;

friend class Iterator;
friend class TextModel;
friend class PlainTextModel;
};

class SpecialParagraph : public Paragraph {

private:
	SpecialParagraph(Kind kind);

public:
	~SpecialParagraph();
	Kind kind() const;

private:
	Kind myKind;

friend class PlainTextModel;
};

class TreeParagraph : public Paragraph {

public:
	TreeParagraph(TreeParagraph *parent = 0);
	~TreeParagraph();
	Kind kind() const;

	bool isOpen() const;
	void open(bool o);
	void openTree();
	int depth() const;
	TreeParagraph *parent();
	const TreeParagraph *parent() const;
	const std::vector<TreeParagraph*> &children() const;
	int fullSize() const;

	void removeFromParent();

private:
	void addChild(TreeParagraph *child);

private:
	bool myIsOpen;
	int myDepth;
	TreeParagraph *myParent;
	std::vector<TreeParagraph*> myChildren;
};

inline ParagraphEntry::ParagraphEntry() {}
inline ParagraphEntry::~ParagraphEntry() {}

inline ForcedControlEntry::ForcedControlEntry() : myMask(0) {}
inline ForcedControlEntry::~ForcedControlEntry() {}
inline bool ForcedControlEntry::leftIndentSupported() const { return myMask & SUPPORT_LEFT_INDENT; }
inline short ForcedControlEntry::leftIndent() const { return myLeftIndent; }
inline void ForcedControlEntry::setLeftIndent(short leftIndent) { myLeftIndent = leftIndent; myMask |= SUPPORT_LEFT_INDENT; }
inline bool ForcedControlEntry::rightIndentSupported() const { return myMask & SUPPORT_RIGHT_INDENT; }
inline short ForcedControlEntry::rightIndent() const { return myRightIndent; }
inline void ForcedControlEntry::setRightIndent(short rightIndent) { myRightIndent = rightIndent; myMask |= SUPPORT_RIGHT_INDENT; }
inline bool ForcedControlEntry::alignmentTypeSupported() const { return myMask & SUPPORT_ALIGNMENT_TYPE; }
inline AlignmentType ForcedControlEntry::alignmentType() const { return myAlignmentType; }
inline void ForcedControlEntry::setAlignmentType(AlignmentType alignmentType) { myAlignmentType = alignmentType; myMask |= SUPPORT_ALIGNMENT_TYPE; }

inline ControlEntry::ControlEntry(TextKind kind, bool isStart) : myKind(kind), myStart(isStart) {}
inline ControlEntry::~ControlEntry() {}
inline TextKind ControlEntry::kind() const { return myKind; }
inline bool ControlEntry::isStart() const { return myStart; }
inline bool ControlEntry::isHyperlink() const { return false; }

inline FixedHSpaceEntry::FixedHSpaceEntry(unsigned char length) : myLength(length) {}
inline unsigned char FixedHSpaceEntry::length() const { return myLength; }

inline ControlEntryPool::ControlEntryPool() {}
inline ControlEntryPool::~ControlEntryPool() {}

inline HyperlinkControlEntry::HyperlinkControlEntry(const char *address) : ControlEntry((TextKind)*address, true), myLabel(address + 1) {}
inline HyperlinkControlEntry::~HyperlinkControlEntry() {}
inline const std::string &HyperlinkControlEntry::label() const { return myLabel; }
inline bool HyperlinkControlEntry::isHyperlink() const { return true; }

inline TextEntry::TextEntry(const char *address) : myAddress(address) {}
inline TextEntry::~TextEntry() {}
inline const char *TextEntry::data() const { return myAddress + sizeof(size_t); }

inline ImageEntry::ImageEntry(const std::string &id, const ImageMap *imageMap) : myId(id), myMap(imageMap) {}
inline ImageEntry::~ImageEntry() {}
inline const std::string &ImageEntry::id() const { return myId; }

inline Paragraph::Paragraph() : myEntryNumber(0) {}
inline Paragraph::~Paragraph() {}
inline Paragraph::Kind Paragraph::kind() const { return TEXT_PARAGRAPH; }
inline size_t Paragraph::entryNumber() const { return myEntryNumber; }
inline void Paragraph::addEntry(char *address) { if (myEntryNumber == 0) myFirstEntryAddress = address; ++myEntryNumber; }

inline Paragraph::Iterator::Iterator(const Paragraph &paragraph) : myPointer(paragraph.myFirstEntryAddress), myIndex(0), myEndIndex(paragraph.entryNumber()) {}
inline Paragraph::Iterator::~Iterator() {}
inline bool Paragraph::Iterator::isEnd() const { return myIndex == myEndIndex; }
inline ParagraphEntry::Kind Paragraph::Iterator::entryKind() const { return (ParagraphEntry::Kind)*myPointer; }

inline SpecialParagraph::SpecialParagraph(Kind kind) : myKind(kind) {}
inline SpecialParagraph::~SpecialParagraph() {}
inline Paragraph::Kind SpecialParagraph::kind() const { return myKind; }

inline TreeParagraph::~TreeParagraph() {}
inline Paragraph::Kind TreeParagraph::kind() const { return TREE_PARAGRAPH; }
inline bool TreeParagraph::isOpen() const { return myIsOpen; }
inline void TreeParagraph::open(bool o) { myIsOpen = o; }
inline int TreeParagraph::depth() const { return myDepth; }
inline TreeParagraph *TreeParagraph::parent() { return myParent; }
inline const TreeParagraph *TreeParagraph::parent() const { return myParent; }
inline const std::vector<TreeParagraph*> &TreeParagraph::children() const { return myChildren; }
inline void TreeParagraph::addChild(TreeParagraph *child) { myChildren.push_back(child); }

#endif /* __PARAGRAPH_H__ */
