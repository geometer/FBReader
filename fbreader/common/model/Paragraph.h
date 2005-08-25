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

#include <abstract/ZLString.h>

#include "TextKind.h"
#include "AlignmentType.h"

class BookDescription;
class ZLImage;
typedef std::map<std::string,ZLImage*> ImageMap;

class ParagraphEntry {

public:
	enum Kind {
		TEXT_ENTRY,
		IMAGE_ENTRY,
		CONTROL_ENTRY,
		FORCED_CONTROL_ENTRY,
	};

protected:
	ParagraphEntry() MODEL_SECTION;

public:
	virtual ~ParagraphEntry() MODEL_SECTION;
	virtual Kind entryKind() const MODEL_SECTION = 0;
};

class ForcedControlEntry : public ParagraphEntry {

public:
	ForcedControlEntry() MODEL_SECTION;
	~ForcedControlEntry() MODEL_SECTION;
	Kind entryKind() const MODEL_SECTION;

	bool leftIndentSupported() const MODEL_SECTION;
	int leftIndent() const MODEL_SECTION;
	void setLeftIndent(int leftIndent) MODEL_SECTION;

	bool rightIndentSupported() const MODEL_SECTION;
	int rightIndent() const MODEL_SECTION;
	void setRightIndent(int rightIndent) MODEL_SECTION;

	bool alignmentTypeSupported() const MODEL_SECTION;
	AlignmentType alignmentType() const MODEL_SECTION;
	void setAlignmentType(AlignmentType alignmentType) MODEL_SECTION;

private:
	enum {
		SUPPORT_LEFT_INDENT = 1,
		SUPPORT_RIGHT_INDENT = 2,
		SUPPORT_ALIGNMENT_TYPE = 4,
	};

	int myMask;

	int myLeftIndent;
	int myRightIndent;
	AlignmentType myAlignmentType;
};

class ControlEntry : public ParagraphEntry {

protected:
	ControlEntry(TextKind kind, bool isStart) MODEL_SECTION;

public:
	virtual ~ControlEntry() MODEL_SECTION;
	TextKind kind() const MODEL_SECTION;
	Kind entryKind() const MODEL_SECTION;
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
	ControlEntry *controlEntry(TextKind kind, bool isStart) MODEL_SECTION;

private:
	std::map<TextKind, ControlEntry*> myStartEntries;
	std::map<TextKind, ControlEntry*> myEndEntries;
};

class HyperlinkControlEntry : public ControlEntry {

public:
	HyperlinkControlEntry(TextKind kind, const std::string &label) MODEL_SECTION;
	~HyperlinkControlEntry() MODEL_SECTION;
	bool isHyperlink() const MODEL_SECTION;
	const std::string &label() const MODEL_SECTION;

private:
	std::string myLabel;
};

class TextEntry : public ParagraphEntry {

public:
	TextEntry() MODEL_SECTION;
	TextEntry(ZLString &text) MODEL_SECTION;
	~TextEntry() MODEL_SECTION;
	const ZLString &text() const MODEL_SECTION;
	void addText(const std::string &text) MODEL_SECTION;
	void addText(const ZLString &text) MODEL_SECTION;
	void addText(const ZLStringBuffer &text) MODEL_SECTION;
	Kind entryKind() const MODEL_SECTION;
 
private:
	ZLString myText;
};

class ImageEntry : public ParagraphEntry {

public:
	ImageEntry(const std::string &id, const ImageMap &imageMap) MODEL_SECTION;
	~ImageEntry() MODEL_SECTION;
	Kind entryKind() const MODEL_SECTION;
	const std::string &id() const MODEL_SECTION;
	const ZLImage *image() const MODEL_SECTION;

private:
	std::string myId;
	const ImageMap &myMap;
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
	Paragraph(Kind kind) MODEL_SECTION;
	virtual ~Paragraph() MODEL_SECTION;
	Kind kind() const MODEL_SECTION;

	void addControl(TextKind textKind, bool isStart) MODEL_SECTION;
	void addControl(ForcedControlEntry *entry) MODEL_SECTION;
	void addHyperlinkControl(TextKind textKind, const std::string &label) MODEL_SECTION;
	void addNonConstText(ZLString &text) MODEL_SECTION;
	void addText(const std::string &text) MODEL_SECTION;
	void addText(const ZLStringBuffer &text) MODEL_SECTION;
	void addImage(const std::string &id, const ImageMap &imageMap) MODEL_SECTION;

	const std::vector<ParagraphEntry*> &entries() const MODEL_SECTION;

	size_t textLength() const MODEL_SECTION;

private:
	Kind myKind;
	std::vector<ParagraphEntry*> myEntries;
};

class ParagraphWithReference : public Paragraph {

public:
	ParagraphWithReference() MODEL_SECTION;
	~ParagraphWithReference() MODEL_SECTION;
	void setReference(long reference) MODEL_SECTION;
	long reference() const MODEL_SECTION;

private:
	long myReference;
};

class TreeParagraph : public Paragraph {

public:
	TreeParagraph(TreeParagraph *parent = 0) MODEL_SECTION;
	~TreeParagraph() MODEL_SECTION;
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
inline ParagraphEntry::Kind ForcedControlEntry::entryKind() const { return FORCED_CONTROL_ENTRY; }
inline bool ForcedControlEntry::leftIndentSupported() const { return myMask & SUPPORT_LEFT_INDENT; }
inline int ForcedControlEntry::leftIndent() const { return myLeftIndent; }
inline void ForcedControlEntry::setLeftIndent(int leftIndent) { myLeftIndent = leftIndent; myMask |= SUPPORT_LEFT_INDENT; }
inline bool ForcedControlEntry::rightIndentSupported() const { return myMask & SUPPORT_RIGHT_INDENT; }
inline int ForcedControlEntry::rightIndent() const { return myRightIndent; }
inline void ForcedControlEntry::setRightIndent(int rightIndent) { myRightIndent = rightIndent; myMask |= SUPPORT_RIGHT_INDENT; }
inline bool ForcedControlEntry::alignmentTypeSupported() const { return myMask & SUPPORT_ALIGNMENT_TYPE; }
inline AlignmentType ForcedControlEntry::alignmentType() const { return myAlignmentType; }
inline void ForcedControlEntry::setAlignmentType(AlignmentType alignmentType) { myAlignmentType = alignmentType; myMask |= SUPPORT_ALIGNMENT_TYPE; }

inline ControlEntry::ControlEntry(TextKind kind, bool isStart) : myKind(kind), myStart(isStart) {}
inline ControlEntry::~ControlEntry() {}
inline TextKind ControlEntry::kind() const { return myKind; }
inline ParagraphEntry::Kind ControlEntry::entryKind() const { return CONTROL_ENTRY; }
inline bool ControlEntry::isStart() const { return myStart; }
inline bool ControlEntry::isHyperlink() const { return false; }
inline ControlEntryPool::ControlEntryPool() {}

inline HyperlinkControlEntry::HyperlinkControlEntry(TextKind kind, const std::string &label) : ControlEntry(kind, true), myLabel(label) {}
inline HyperlinkControlEntry::~HyperlinkControlEntry() {}
inline bool HyperlinkControlEntry::isHyperlink() const { return true; }
inline const std::string &HyperlinkControlEntry::label() const { return myLabel; }

inline TextEntry::TextEntry() {}
inline TextEntry::TextEntry(ZLString &text) { myText.swap(text); }
inline TextEntry::~TextEntry() {}
inline const ZLString &TextEntry::text() const { return myText; }
inline void TextEntry::addText(const std::string &text) { myText += text; }
inline void TextEntry::addText(const ZLString &text) { myText += text; }
inline void TextEntry::addText(const ZLStringBuffer &text) { myText += text; }
inline ParagraphEntry::Kind TextEntry::entryKind() const { return TEXT_ENTRY; }

inline ImageEntry::ImageEntry(const std::string &id, const ImageMap &imageMap) : myId(id), myMap(imageMap) {}
inline ImageEntry::~ImageEntry() {}
inline ParagraphEntry::Kind ImageEntry::entryKind() const { return IMAGE_ENTRY; }
inline const std::string &ImageEntry::id() const { return myId; }

inline Paragraph::Paragraph(Kind kind) : myKind(kind) {}
inline Paragraph::Kind Paragraph::kind() const { return myKind; }
inline void Paragraph::addControl(TextKind textKind, bool isStart) { myEntries.push_back(ControlEntryPool::Pool.controlEntry(textKind, isStart)); }
inline void Paragraph::addControl(ForcedControlEntry *entry) { myEntries.push_back(entry); }
inline void Paragraph::addHyperlinkControl(TextKind textKind, const std::string &label) { myEntries.push_back(new HyperlinkControlEntry(textKind, label)); }
inline void Paragraph::addImage(const std::string &id, const ImageMap &imageMap) { myEntries.push_back(new ImageEntry(id, imageMap)); }
inline const std::vector<ParagraphEntry*> &Paragraph::entries() const { return myEntries; }

inline ParagraphWithReference::ParagraphWithReference() : Paragraph(TEXT_PARAGRAPH), myReference(-1) {}
inline ParagraphWithReference::~ParagraphWithReference() {}
inline void ParagraphWithReference::setReference(long reference) { myReference = reference; }
inline long ParagraphWithReference::reference() const { return myReference; }

inline TreeParagraph::~TreeParagraph() {}
inline bool TreeParagraph::isOpen() const { return myIsOpen; }
inline void TreeParagraph::open(bool o) { myIsOpen = o; }
inline int TreeParagraph::depth() const { return myDepth; }
inline TreeParagraph *TreeParagraph::parent() { return myParent; }
inline const std::vector<TreeParagraph*> &TreeParagraph::children() const { return myChildren; }
inline void TreeParagraph::addChild(TreeParagraph *child) { myChildren.push_back(child); }

#endif /* __PARAGRAPH_H__ */
