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

#ifndef __PARAGRAPH_H__
#define __PARAGRAPH_H__

#include <map>
#include <vector>
#include <string>

#include <abstract/shared_ptr.h>

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
	};

protected:
	ParagraphEntry() MODEL_SECTION;

public:
	virtual ~ParagraphEntry() MODEL_SECTION;

private: // disable copying
	ParagraphEntry(const ParagraphEntry &entry);
	const ParagraphEntry &operator = (const ParagraphEntry &entry);
};

class ForcedControlEntry : public ParagraphEntry {

public:
	ForcedControlEntry() MODEL_SECTION;
	ForcedControlEntry(char *address) MODEL_SECTION;
	~ForcedControlEntry() MODEL_SECTION;

	bool leftIndentSupported() const MODEL_SECTION;
	short leftIndent() const MODEL_SECTION;
	void setLeftIndent(short leftIndent) MODEL_SECTION;

	bool rightIndentSupported() const MODEL_SECTION;
	short rightIndent() const MODEL_SECTION;
	void setRightIndent(short rightIndent) MODEL_SECTION;

	bool alignmentTypeSupported() const MODEL_SECTION;
	AlignmentType alignmentType() const MODEL_SECTION;
	void setAlignmentType(AlignmentType alignmentType) MODEL_SECTION;

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
	ControlEntry(TextKind kind, bool isStart) MODEL_SECTION;

public:
	virtual ~ControlEntry() MODEL_SECTION;
	TextKind kind() const MODEL_SECTION;
	bool isStart() const MODEL_SECTION;
	virtual bool isHyperlink() const MODEL_SECTION;

private:
	TextKind myKind;
	bool myStart;

friend class ControlEntryPool;
};

class ControlEntryPool {

public:
	static ControlEntryPool Pool;
	
public:
	ControlEntryPool() MODEL_SECTION;
	~ControlEntryPool() MODEL_SECTION;
	shared_ptr<ParagraphEntry> controlEntry(TextKind kind, bool isStart) MODEL_SECTION;

private:
	std::map<TextKind, shared_ptr<ParagraphEntry> > myStartEntries;
	std::map<TextKind, shared_ptr<ParagraphEntry> > myEndEntries;
};

class HyperlinkControlEntry : public ControlEntry {

public:
	HyperlinkControlEntry(const char *address) MODEL_SECTION;
	~HyperlinkControlEntry() MODEL_SECTION;
	const std::string &label() const MODEL_SECTION;
	bool isHyperlink() const MODEL_SECTION;

private:
	std::string myLabel;
};

class TextEntry : public ParagraphEntry {

public:
	TextEntry(const char *address) MODEL_SECTION;
	~TextEntry() MODEL_SECTION;

	size_t dataLength() const MODEL_SECTION;
	const char *data() const MODEL_SECTION;
 
private:
	const char *myAddress;
};

class ImageEntry : public ParagraphEntry {

public:
	ImageEntry(const std::string &id, const ImageMap *imageMap) MODEL_SECTION;
	~ImageEntry() MODEL_SECTION;
	const std::string &id() const MODEL_SECTION;
	const ZLImage *image() const MODEL_SECTION;

private:
	std::string myId;
	const ImageMap *myMap;
};

class Paragraph {

public:
	class Iterator {

	public:
		Iterator(const Paragraph &paragraph) MODEL_SECTION;
		~Iterator() MODEL_SECTION;

		bool isEnd() const MODEL_SECTION;
		void next() MODEL_SECTION;
		const shared_ptr<ParagraphEntry> entry() const MODEL_SECTION;
		ParagraphEntry::Kind entryKind() const MODEL_SECTION;

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
	Paragraph() MODEL_SECTION;

public:
	virtual ~Paragraph() MODEL_SECTION;
	virtual Kind kind() const MODEL_SECTION;

	size_t entryNumber() const MODEL_SECTION;

	size_t textLength() const MODEL_SECTION;

private:
	void addEntry(char *address) MODEL_SECTION;

private:
	char *myFirstEntryAddress;
	size_t myEntryNumber;

friend class Iterator;
friend class TextModel;
friend class PlainTextModel;
};

class SpecialParagraph : public Paragraph {

private:
	SpecialParagraph(Kind kind) MODEL_SECTION;

public:
	~SpecialParagraph() MODEL_SECTION;
	Kind kind() const MODEL_SECTION;

private:
	Kind myKind;

friend class PlainTextModel;
};

class ParagraphWithReference : public Paragraph {

private:
	ParagraphWithReference(long reference) MODEL_SECTION;

public:
	~ParagraphWithReference() MODEL_SECTION;
	long reference() const MODEL_SECTION;

private:
	const long myReference;

friend class PlainTextModel;
};

class TreeParagraph : public Paragraph {

public:
	TreeParagraph(TreeParagraph *parent = 0) MODEL_SECTION;
	~TreeParagraph() MODEL_SECTION;
	Kind kind() const MODEL_SECTION;

	bool isOpen() const MODEL_SECTION;
	void open(bool o) MODEL_SECTION;
	void openTree() MODEL_SECTION;
	int depth() const MODEL_SECTION;
	TreeParagraph *parent() MODEL_SECTION;
	const std::vector<TreeParagraph*> &children() const MODEL_SECTION;

private:
	void addChild(TreeParagraph *child) MODEL_SECTION;

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
inline void Paragraph::addEntry(char *address) { if (myEntryNumber == 0) myFirstEntryAddress = address; myEntryNumber++; }

inline Paragraph::Iterator::Iterator(const Paragraph &paragraph) : myPointer(paragraph.myFirstEntryAddress), myIndex(0), myEndIndex(paragraph.entryNumber()) {}
inline Paragraph::Iterator::~Iterator() {}
inline bool Paragraph::Iterator::isEnd() const { return myIndex == myEndIndex; }
inline ParagraphEntry::Kind Paragraph::Iterator::entryKind() const { return (ParagraphEntry::Kind)*myPointer; }

inline SpecialParagraph::SpecialParagraph(Kind kind) : myKind(kind) {}
inline SpecialParagraph::~SpecialParagraph() {}
inline Paragraph::Kind SpecialParagraph::kind() const { return myKind; }

inline ParagraphWithReference::ParagraphWithReference(long reference) : myReference(reference) {}
inline ParagraphWithReference::~ParagraphWithReference() {}
inline long ParagraphWithReference::reference() const { return myReference; }

inline TreeParagraph::~TreeParagraph() {}
inline Paragraph::Kind TreeParagraph::kind() const { return TREE_PARAGRAPH; }
inline bool TreeParagraph::isOpen() const { return myIsOpen; }
inline void TreeParagraph::open(bool o) { myIsOpen = o; }
inline int TreeParagraph::depth() const { return myDepth; }
inline TreeParagraph *TreeParagraph::parent() { return myParent; }
inline const std::vector<TreeParagraph*> &TreeParagraph::children() const { return myChildren; }
inline void TreeParagraph::addChild(TreeParagraph *child) { myChildren.push_back(child); }

#endif /* __PARAGRAPH_H__ */
