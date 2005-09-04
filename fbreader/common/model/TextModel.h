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

#ifndef __TEXTMODEL_H__
#define __TEXTMODEL_H__

#include <vector>
#include <string>

#include "TextMark.h"
#include "RowMemoryAllocator.h"

class Paragraph;
class TreeParagraph;

class TextModel {

public:
	enum Kind {
		PLAIN_TEXT_MODEL,
		TREE_MODEL,
	};

	TextModel() MODEL_SECTION;
	virtual ~TextModel() MODEL_SECTION;
	virtual Kind kind() const MODEL_SECTION = 0;

	const std::vector<Paragraph*> &paragraphs() const MODEL_SECTION;
	const std::vector<TextMark> &marks() const MODEL_SECTION;

	virtual void search(const std::string &text, size_t startIndex, size_t endIndex, bool ignoreCase) const MODEL_SECTION;
	virtual void selectParagraph(const unsigned int paragraphNumber) const MODEL_SECTION;

	TextMark firstMark() const MODEL_SECTION;
	TextMark lastMark() const MODEL_SECTION;
	TextMark nextMark(TextMark position) const MODEL_SECTION;
	TextMark previousMark(TextMark position) const MODEL_SECTION;

	RowMemoryAllocator &allocator() const MODEL_SECTION;

protected:
	void addParagraphInternal(Paragraph *paragraph) MODEL_SECTION;
	
private:
	std::vector<Paragraph*> myParagraphs;
	mutable std::vector<TextMark> myMarks;
	mutable RowMemoryAllocator myAllocator;
};

class PlainTextModel : public TextModel {

public:
	Kind kind() const MODEL_SECTION;
	void addParagraph(Paragraph *paragraph) MODEL_SECTION;
};

class TreeModel : public TextModel {

public:
	TreeModel() MODEL_SECTION;
	~TreeModel() MODEL_SECTION;
	Kind kind() const MODEL_SECTION;
	TreeParagraph *createParagraph(TreeParagraph *parent = 0) MODEL_SECTION;
	
	void search(const std::string &text, size_t startIndex, size_t endIndex, bool ignoreCase) const MODEL_SECTION;
	void selectParagraph(const unsigned int paragraphNumber) const MODEL_SECTION;

private:
	TreeParagraph *myRoot;
};

inline const std::vector<Paragraph*> &TextModel::paragraphs() const { return myParagraphs; }
inline const std::vector<TextMark> &TextModel::marks() const { return myMarks; }
inline void TextModel::addParagraphInternal(Paragraph *paragraph) { myParagraphs.push_back(paragraph); }
inline RowMemoryAllocator &TextModel::allocator() const { return myAllocator; }

inline TextModel::Kind PlainTextModel::kind() const { return PLAIN_TEXT_MODEL; }
inline void PlainTextModel::addParagraph(Paragraph *paragraph) { addParagraphInternal(paragraph); }

inline TextModel::Kind TreeModel::kind() const { return TREE_MODEL; }

#endif /* __TEXTMODEL_H__ */
