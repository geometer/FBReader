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

#include "SelectionModel.h"

SelectionModel::SelectionModel(TextElementMap &elementMap) : myElementMap(elementMap), myIsActive(false), myIsEmpty(true) {
}

void SelectionModel::setBound(Bound &bound, int x, int y) {
	bound.X = x;
	bound.Y = y;

	const TextElementArea &back = myElementMap.back();
	bound.ParagraphNumber = back.ParagraphNumber;
	bound.TextElementNumber = back.TextElementNumber + 1;
	bound.InsideElement = false;

	for (TextElementMap::const_iterator it = myElementMap.begin(); it != myElementMap.end(); ++it) {
		if ((it->YStart > y) || ((it->YEnd > y) && (it->XEnd > x))) {
			bound.InsideElement = TextElementArea::RangeChecker(x, y)(*it);
			bound.ParagraphNumber = it->ParagraphNumber;
			bound.TextElementNumber = it->TextElementNumber;
			break;
		}
	}
}

void SelectionModel::activate(int x, int y) {
	if (myElementMap.empty()) {
		return;
	}

	myIsActive = true;
	myIsEmpty = false;
	setBound(myFirstBound, x, y);
	mySecondBound = myFirstBound;
}

void SelectionModel::extendTo(int x, int y) {
	if (!myIsActive || myElementMap.empty()) {
		return;
	}

	setBound(mySecondBound, x, y);
}

void SelectionModel::deactivate() {
	myIsActive = false;
}

void SelectionModel::clear() {
	myIsEmpty = true;
}

std::pair<TextElementMap::const_iterator,TextElementMap::const_iterator> SelectionModel::range() const {
	std::pair<TextElementMap::const_iterator,TextElementMap::const_iterator> answer(myElementMap.end(), myElementMap.end());

	bool changeOrder = mySecondBound < myFirstBound;
	const Bound &leftBound = changeOrder ? mySecondBound : myFirstBound;
	const Bound &rightBound = changeOrder ? myFirstBound : mySecondBound;

	TextElementMap::const_iterator it = myElementMap.begin();
	if (rightBound < *it) {
		return answer;
	}

	if (leftBound < *it) {
		answer.first = it;
	} else {
		for (; it != myElementMap.end(); ++it) {
			if (leftBound <= *it) {
				answer.first = it;
				break;
			}
		}
	}
	for (; it != myElementMap.end(); ++it) {
		if (rightBound <= *it) {
			answer.second = rightBound.InsideElement ? (it + 1) : it;
			break;
		}
	}

	return answer;
}

bool SelectionModel::Bound::operator < (const Bound &bound) const {
	if (ParagraphNumber < bound.ParagraphNumber) {
		return true;
	}
	if (ParagraphNumber > bound.ParagraphNumber) {
		return false;
	}
	if (TextElementNumber < bound.TextElementNumber) {
		return true;
	}
	if (TextElementNumber > bound.TextElementNumber) {
		return false;
	}
	return !InsideElement && bound.InsideElement;
}

bool SelectionModel::Bound::operator <= (const TextElementArea &area) const {
	return
		(ParagraphNumber < area.ParagraphNumber) ||
		((ParagraphNumber == area.ParagraphNumber) &&
		 (TextElementNumber <= area.TextElementNumber));
}

bool SelectionModel::Bound::operator < (const TextElementArea &area) const {
	return
		(ParagraphNumber < area.ParagraphNumber) ||
		((ParagraphNumber == area.ParagraphNumber) &&
		 (TextElementNumber < area.TextElementNumber));
}
