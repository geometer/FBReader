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

#ifndef __TEXTMODEL_H__
#define __TEXTMODEL_H__

#include <vector>
#include <string>

#include "TextMark.h"

class Paragraph;
class TreeParagraph;

class TextModel {

public:
	enum Kind {
		PLAIN_TEXT_MODEL,
		TREE_MODEL,
	};

	TextModel();
	virtual ~TextModel();
	virtual Kind kind() const = 0;

	const std::vector<Paragraph*> &paragraphs() const { return myParagraphs; }
	const std::vector<TextMark> &marks() const { return myMarks; }

	virtual void search(const std::string &text, bool ignoreCase) const;

	TextMark firstMark() const;
	TextMark lastMark() const;
	TextMark nextMark(TextMark position) const;
	TextMark previousMark(TextMark position) const;

protected:
	void addParagraphInternal(Paragraph *paragraph) { myParagraphs.push_back(paragraph); }
	
private:
	std::vector<Paragraph*> myParagraphs;
	mutable std::vector<TextMark> myMarks;
};

class PlainTextModel : public TextModel {

public:
	Kind kind() const { return PLAIN_TEXT_MODEL; }
	void addParagraph(Paragraph *paragraph) { addParagraphInternal(paragraph); }
};

class TreeModel : public TextModel {

public:
	TreeModel();
	~TreeModel();
	Kind kind() const { return TREE_MODEL; }
	TreeParagraph *createParagraph(TreeParagraph *parent = 0);
	
	void search(const std::string &text, bool ignoreCase) const;

private:
	TreeParagraph *myRoot;
};

#endif /* __TEXTMODEL_H__ */
