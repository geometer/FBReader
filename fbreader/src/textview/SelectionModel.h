/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __SELECTIONMODEL_H__
#define __SELECTIONMODEL_H__

#include <map>

#include "TextArea.h"

class SelectionModel {

public:
	SelectionModel(TextElementMap &elementMap);

	void activate(int x, int y);
	bool extendTo(int x, int y);
	void deactivate();
	void clear();

	bool isEmpty() const;

public:
	struct BoundElement {
		bool Exists;
		int ParagraphNumber;
		int TextElementNumber;
		size_t CharNumber;

		bool operator != (const BoundElement &element) const;
	};

private:
	struct Bound {
		BoundElement Before;
		BoundElement After;

		bool operator < (const Bound &bound) const;
	};

public:
	std::pair<BoundElement,BoundElement> range() const;

private:
	void setBound(Bound &bound, int x, int y);

private:
	const TextElementMap &myElementMap;
	bool myIsActive;
	bool myIsEmpty;

	Bound myFirstBound;
	Bound mySecondBound;
};

#endif /* __SELECTIONMODEL_H__ */
