/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include "W32Container.h"

W32Table::W32Table() : myVerticalSpacing(0), myHorizontalSpacing(0) {
}

void W32Table::setSpacings(int vertical, int horizontal) {
	myVerticalSpacing = vertical;
	myHorizontalSpacing = horizontal;
}

void W32Table::setElement(W32WidgetPtr element, int row, int fromColumn, int toColumn) {
	if (!element.isNull()) {
		if (row >= (int)myRows.size()) {
			myRows.insert(myRows.end(), row - myRows.size() + 1, RowList());
		}
		RowList &rowList = myRows[row];

		RowList::iterator it = rowList.begin();
		bool canInsertAfter = true;
		for (; it != rowList.end(); ++it) {
			if (it->XFrom > toColumn) {
				break;
			}
			canInsertAfter = it->XTo < fromColumn;
		}
		if (canInsertAfter) {
			rowList.insert(it, CellInfo(fromColumn, toColumn, element));
		}
	}
}

void W32Table::allocate(WORD *&p, short &id) const {
	for (std::vector<RowList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (RowList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			jt->Widget->allocate(p, id);
		}
	}
}

int W32Table::allocationSize() const {
	int size = 0;
	for (std::vector<RowList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (RowList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			size += jt->Widget->allocationSize();
		}
	}
	return size;
}

void W32Table::setVisible(bool visible) {
	for (std::vector<RowList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (RowList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			jt->Widget->setVisible(visible);
		}
	}
}

bool W32Table::isVisible() const {
	for (std::vector<RowList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (RowList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			if (jt->Widget->isVisible()) {
				return true;
			}
		}
	}
	return false;
}

int W32Table::controlNumber() const {
	int counter = 0;
	for (std::vector<RowList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (RowList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			counter += jt->Widget->controlNumber();
		}
	}
	return counter;
}

void W32Table::calculateSizes(std::vector<short> &widths, std::vector<short> &heights) const {
	for (std::vector<RowList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		short currentHeight = 0;
		if (!it->empty()) {
			for (RowList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
				if (jt->Widget->isVisible()) {
					Size elementSize = jt->Widget->minimumSize();
					currentHeight = std::max(currentHeight, elementSize.Height);
					if (jt->XTo >= (short)widths.size()) {
						widths.insert(widths.end(), jt->XTo - widths.size() + 1, 0);
					}
					if (jt->XFrom == jt->XTo) {
						widths[jt->XTo] = std::max(widths[jt->XTo], elementSize.Width); 
					}
				}
			}
		}
		heights.push_back(currentHeight);
	}
	for (std::vector<RowList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		if (!it->empty()) {
			for (RowList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
				if ((jt->Widget->isVisible()) && (jt->XFrom < jt->XTo)) {
					short spanSize = myHorizontalSpacing * (jt->XTo - jt->XFrom - 1);
					for (int i = jt->XFrom; i <= jt->XTo; ++i) {
						spanSize += widths[i];
					}
					Size elementSize = jt->Widget->minimumSize();
					if (spanSize < elementSize.Width) {
						short delta = elementSize.Width - spanSize;
						for (int i = jt->XFrom; i <= jt->XTo; ++i) {
							short d = delta / (jt->XTo - i + 1);
							delta -= d;
							widths[i] += d;
						}
					}
				}
			}
		}
	}
}

W32Widget::Size W32Table::minimumSize() const {
	std::vector<short> widths, heights;
	calculateSizes(widths, heights);

	Size size(
		leftMargin() + rightMargin() + myHorizontalSpacing * (widths.size() - 1),
		topMargin() + bottomMargin()
	);
	for (std::vector<short>::const_iterator it = widths.begin(); it != widths.end(); ++it) {
		size.Width += *it;
	}
	int hCount = 0;
	for (std::vector<short>::const_iterator it = heights.begin(); it != heights.end(); ++it) {
		if (*it > 0) {
			size.Height += *it;
			++hCount;
		}
	}
	if (hCount > 1) {
		size.Height += myVerticalSpacing * (hCount - 1);
	}

	return size;
}

void W32Table::setPosition(int x, int y, Size size) {
	std::vector<short> widths, heights, lefts, rights;
	calculateSizes(widths, heights);

	if ((widths.size() == 0) || (heights.size() == 0)) {
		return;
	}

	Size minSize(
		leftMargin() + rightMargin() + myHorizontalSpacing * (widths.size() - 1),
		topMargin() + bottomMargin()
	);
	for (std::vector<short>::const_iterator it = widths.begin(); it != widths.end(); ++it) {
		minSize.Width += *it;
	}
	int hCount = 0;
	for (std::vector<short>::const_iterator it = heights.begin(); it != heights.end(); ++it) {
		if (*it > 0) {
			minSize.Height += *it;
			++hCount;
		}
	}
	if (hCount == 0) {
		return;
	}
	minSize.Height += myVerticalSpacing * (hCount - 1);

	{
		short delta = size.Width - minSize.Width;
		int len = widths.size();
		lefts.reserve(len);
		rights.reserve(len);
		short currentX = x + leftMargin();
		for (int i = 0; i < len; ++i) {
			short d = delta / (len - i);
			widths[i] += d;
			delta -= d;
			lefts.push_back(currentX);
			currentX += widths[i];
			rights.push_back(currentX);
			currentX += myHorizontalSpacing;
		}
	}

	{
		short delta = size.Height - minSize.Height;
		int len = hCount;
		for (std::vector<short>::iterator it = heights.begin(); len > 0; ++it) {
			if (*it > 0) {
				short d = delta / len;
				*it += d;
				delta -= d;
				--len;
			}
		}
	}

	int ey = y + topMargin();
	for (std::vector<RowList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		short h = heights[it - myRows.begin()];
		if (h > 0) {
			for (RowList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
				if (jt->Widget->isVisible()) {
					jt->Widget->setPosition(lefts[jt->XFrom], ey, Size(rights[jt->XTo] - lefts[jt->XFrom], h));
				}
			}
			ey += h + myVerticalSpacing;
		}
	}
}

void W32Table::init(HWND parent, W32ControlCollection *collection) {
	for (std::vector<RowList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (RowList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			jt->Widget->init(parent, collection);
		}
	}
}
