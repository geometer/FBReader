/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLTEXTSELECTIONMODEL_H__
#define __ZLTEXTSELECTIONMODEL_H__

#include <set>
#include <string>

#include <shared_ptr.h>

#include <ZLDialogManager.h>

#include "ZLTextParagraphCursor.h"

class ZLTextArea;
class ZLImageData;
class ZLTextElementRectangle;

class ZLTextSelectionModel {

public:
	struct BoundElement {
		bool Exists;
		int ParagraphIndex;
		int ElementIndex;
		size_t CharIndex;

		bool operator == (const BoundElement &element) const;
		bool operator != (const BoundElement &element) const;
	};
	typedef std::pair<BoundElement,BoundElement> Range;

public:
	ZLTextSelectionModel(ZLTextArea &area);
	~ZLTextSelectionModel();

	void activate(int x, int y);
	enum ExtensionResult {
		BOUND_NOT_CHANGED,
		BOUND_CHANGED,
		BOUND_OVER_BEFORE,
		BOUND_OVER_AFTER
	};
	ExtensionResult extendTo(int x, int y);
	void invalidate();
	void update();
	void deactivate();
	void clear();

	bool selectWord(int x, int y);
	void extendWordSelectionToParagraph();

	const std::string &text() const;
	shared_ptr<ZLImageData> image() const;
	const std::vector<Range> &ranges() const;

	void copySelectionToClipboard(ZLDialogManager::ClipboardType type) const;

	bool isEmpty() const;

private:
	struct Bound {
		BoundElement Before;
		BoundElement After;

		bool operator < (const Bound &bound) const;
	};

private:
	int charIndex(const ZLTextElementRectangle &rectangle, int x);

	Range internalRange() const;
	void setBound(Bound &bound, int x, int y);

	void clearData() const;
	void createData() const;

private:
	ZLTextArea &myArea;
	bool myIsActive;
	bool myIsEmpty;

	Bound myFirstBound;
	Bound mySecondBound;

	int myStoredX;
	int myStoredY;

	mutable std::set<ZLTextParagraphCursorPtr> myCursors;
	mutable std::string myText;
	mutable shared_ptr<ZLImageData> myImage;
	mutable bool myTextIsUpToDate;

	mutable std::vector<Range> myRanges;
	mutable bool myRangeVectorIsUpToDate;

	bool myIsValid;
};

#endif /* __ZLTEXTSELECTIONMODEL_H__ */
