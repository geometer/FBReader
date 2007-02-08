/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <iostream>

#include "W32Container.h"

W32Table::W32Table() : myTopMargin(0), myBottomMargin(0), myLeftMargin(0), myRightMargin(0), myVerticalSpacing(0), myHorizontalSpacing(0) {
}

void W32Table::setMargins(int top, int bottom, int left, int right) {
	myTopMargin = top;
	myBottomMargin = bottom;
	myLeftMargin = left;
	myRightMargin = right;
}

void W32Table::setSpacings(int vertical, int horizontal) {
	myVerticalSpacing = vertical;
	myHorizontalSpacing = horizontal;
}

void W32Table::setElement(W32WidgetPtr element, int row, int column) {
	if (!element.isNull()) {
		if (row >= (int)myRows.size()) {
			myRows.insert(myRows.end(), row - myRows.size() + 1, W32WidgetList());
		}
		W32WidgetList &rowList = myRows[row];
		if (column >= (int)rowList.size()) {
			rowList.insert(rowList.end(), column - rowList.size() + 1, W32WidgetPtr());
		}
		rowList[column] = element;
	}
}

void W32Table::allocate(WORD *&p, short &id) const {
	for (std::vector<W32WidgetList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (W32WidgetList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			if (!jt->isNull()) {
				(*jt)->allocate(p, id);
			}
		}
	}
}

int W32Table::allocationSize() const {
	int size = 0;
	for (std::vector<W32WidgetList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (W32WidgetList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			if (!jt->isNull()) {
				size += (*jt)->allocationSize();
			}
		}
	}
	return size;
}

void W32Table::setVisible(bool visible) {
	// TODO: implement
}

bool W32Table::isVisible() const {
	// TODO: implement
	return true;
}

int W32Table::controlNumber() const {
	int counter = 0;
	for (std::vector<W32WidgetList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (W32WidgetList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			if (!jt->isNull()) {
				counter += (*jt)->controlNumber();
			}
		}
	}
	return counter;
}

void W32Table::calculateSizes(std::vector<short> &widths, std::vector<short> &heights) const {
	//std::cerr << "calculateSizes...";
	for (std::vector<W32WidgetList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		short currentHeight = 0;
		if (it->size() > widths.size()) {
			widths.insert(widths.end(), it->size() - widths.size(), 0);
		}
		for (W32WidgetList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			if (!jt->isNull() && (*jt)->isVisible()) {
				Size elementSize = (*jt)->minimumSize();
				currentHeight = std::max(currentHeight, elementSize.Height);
				widths[jt - it->begin()] = std::max(widths[jt - it->begin()], elementSize.Width); 
			}
		}
		heights.push_back(currentHeight);
	}
	//std::cerr << "OK\n";
	/*
	std::cerr << "widths: ";
	for (size_t i = 0; i < widths.size(); ++i) {
		std::cerr << widths[i] << ' ';
	}
	std::cerr << "\n";
	std::cerr << "heights: ";
	for (size_t i = 0; i < heights.size(); ++i) {
		std::cerr << heights[i] << ' ';
	}
	std::cerr << "\n";
	*/
}

W32Widget::Size W32Table::minimumSize() const {
	std::vector<short> widths, heights;
	calculateSizes(widths, heights);

	Size size(myLeftMargin + myRightMargin, myTopMargin + myBottomMargin);

	int wCount = 0;
	for (std::vector<short>::const_iterator it = widths.begin(); it != widths.end(); ++it) {
		size.Width += *it;
		if (*it != 0) {
			++wCount;
		}
	}
	if (wCount > 1) {
		size.Width += myHorizontalSpacing * (wCount - 1);
	}

	int hCount = 0;
	for (std::vector<short>::const_iterator it = heights.begin(); it != heights.end(); ++it) {
		size.Height += *it;
		if (*it != 0) {
			++hCount;
		}
	}
	if (hCount > 1) {
		size.Height += myVerticalSpacing * (hCount - 1);
	}

	return size;
}

void W32Table::setPosition(int x, int y, Size size) {
	std::vector<short> widths, heights;
	calculateSizes(widths, heights);

	Size minSize(myLeftMargin + myRightMargin, myTopMargin + myBottomMargin);
	int wCount = 0;
	for (std::vector<short>::const_iterator it = widths.begin(); it != widths.end(); ++it) {
		minSize.Width += *it;
		if (*it != 0) {
			++wCount;
		}
	}
	int hCount = 0;
	for (std::vector<short>::const_iterator it = heights.begin(); it != heights.end(); ++it) {
		minSize.Height += *it;
		if (*it != 0) {
			++hCount;
		}
	}

	if ((wCount == 0) || (hCount == 0)) {
		return;
	}

	minSize.Width += myHorizontalSpacing * (wCount - 1);
	minSize.Height += myVerticalSpacing * (hCount - 1);

	const short deltaW = (size.Width - minSize.Width) / wCount;
	for (std::vector<short>::iterator it = widths.begin(); it != widths.end(); ++it) {
		if (*it != 0) {
			*it += deltaW;
		}
	}
	const short deltaH = (size.Height - minSize.Height) / hCount;
	for (std::vector<short>::iterator it = heights.begin(); it != heights.end(); ++it) {
		if (*it != 0) {
			*it += deltaH;
		}
	}

	/*
	std::cerr << "widths: ";
	for (size_t i = 0; i < widths.size(); ++i) {
		std::cerr << widths[i] << ' ';
	}
	std::cerr << "\n";
	std::cerr << "heights: ";
	for (size_t i = 0; i < heights.size(); ++i) {
		std::cerr << heights[i] << ' ';
	}
	std::cerr << "\n";
	*/

	int ey = y + myTopMargin;
	for (std::vector<W32WidgetList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		short h = heights[it - myRows.begin()];
		if (h > 0) {
			int ex = x + myLeftMargin;
			for (W32WidgetList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
				short w = widths[jt - it->begin()];
				if (w > 0) {
					if (!jt->isNull() && (*jt)->isVisible()) {
						(*jt)->setPosition(ex, ey, Size(w, h));
					}
					ex += w + myHorizontalSpacing;
				}
			}
			ey += h + myVerticalSpacing;
		}
	}
}

void W32Table::setDimensions(Size charDimension) {
	for (std::vector<W32WidgetList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (W32WidgetList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			if (!jt->isNull()) {
				(*jt)->setDimensions(charDimension);
			}
		}
	}
}

void W32Table::init(HWND parent, W32ControlCollection *collection) {
	for (std::vector<W32WidgetList>::const_iterator it = myRows.begin(); it != myRows.end(); ++it) {
		for (W32WidgetList::const_iterator jt = it->begin(); jt != it->end(); ++jt) {
			if (!jt->isNull()) {
				(*jt)->init(parent, collection);
			}
		}
	}
}
