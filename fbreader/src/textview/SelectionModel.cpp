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
	if (myElementMap.empty()) {
		return;
	}

	// TODO
	bound.After.CharNumber = 1;
	bound.Before.CharNumber = 1;

	TextElementMap::const_iterator it = myElementMap.begin();
	for (; it != myElementMap.end(); ++it) {
		if ((it->YStart > y) || ((it->YEnd > y) && (it->XEnd > x))) {
			break;
		}
	}

	if (it != myElementMap.end()) {
		bound.After.ParagraphNumber = it->ParagraphNumber;
		bound.After.TextElementNumber = it->TextElementNumber;
		bound.After.Exists = true;
		if (TextElementArea::RangeChecker(x, y)(*it)) {
			bound.Before.ParagraphNumber = bound.After.ParagraphNumber;
			bound.Before.TextElementNumber = bound.After.TextElementNumber;
			bound.Before.Exists = true;
		} else if (it == myElementMap.begin()) {
			bound.Before.Exists = false;
		} else {
			bound.Before.ParagraphNumber = (it - 1)->ParagraphNumber;
			bound.Before.TextElementNumber = (it - 1)->TextElementNumber;
			bound.Before.Exists = true;
		}
	} else {
		const TextElementArea &back = myElementMap.back();
		bound.Before.ParagraphNumber = back.ParagraphNumber;
		bound.Before.TextElementNumber = back.TextElementNumber;
		bound.Before.Exists = true;
		bound.After.Exists = false;
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

bool SelectionModel::BoundElement::operator != (const SelectionModel::BoundElement &element) const {
	return
		(Exists != element.Exists) ||
		(ParagraphNumber != element.ParagraphNumber) ||
		(TextElementNumber != element.TextElementNumber) ||
		(CharNumber != element.CharNumber);
}

bool SelectionModel::extendTo(int x, int y) {
	if (!myIsActive || myElementMap.empty()) {
		return false;
	}

	std::pair<BoundElement,BoundElement> oldRange = range();
	setBound(mySecondBound, x, y);
	std::pair<BoundElement,BoundElement> newRange = range();
	return
		(oldRange.first != newRange.first) ||
		(oldRange.second != newRange.second);
}

void SelectionModel::deactivate() {
	myIsActive = false;
}

void SelectionModel::clear() {
	myIsEmpty = true;
	myIsActive = false;
}

std::pair<SelectionModel::BoundElement,SelectionModel::BoundElement> SelectionModel::range() const {
	return
		(mySecondBound < myFirstBound) ?
		std::pair<BoundElement,BoundElement>(mySecondBound.After, myFirstBound.Before) :
		std::pair<BoundElement,BoundElement>(myFirstBound.After, mySecondBound.Before);
}

bool SelectionModel::Bound::operator < (const Bound &bound) const {
	if (!bound.Before.Exists) {
		return false;
	}
	if (!Before.Exists) {
		return true;
	}

	if (!After.Exists) {
		return false;
	}
	if (!bound.After.Exists) {
		return true;
	}

	if (Before.ParagraphNumber < bound.Before.ParagraphNumber) {
		return true;
	}
	if (Before.ParagraphNumber > bound.Before.ParagraphNumber) {
		return false;
	}
	return Before.TextElementNumber < bound.Before.TextElementNumber;
}

bool SelectionModel::isEmpty() const {
	if (myIsEmpty) {
		return true;
	}
	std::pair<BoundElement,BoundElement> r = range();
	return !r.first.Exists || !r.second.Exists;
}
