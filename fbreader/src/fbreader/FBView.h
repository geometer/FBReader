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

#ifndef __FBVIEW_H__
#define __FBVIEW_H__

#include <ZLTextView.h>
#include <ZLTextStyle.h>
#include <ZLRunnable.h>

class FBIndicatorStyle : public ZLTextPositionIndicatorInfo {

public:
	ZLIntegerRangeOption TypeOption;
	ZLBooleanOption IsSensitiveOption;
	ZLBooleanOption ShowTextPositionOption;
	ZLBooleanOption ShowTimeOption;
	ZLColorOption ColorOption;
	ZLIntegerRangeOption HeightOption;
	ZLIntegerRangeOption OffsetOption;
	ZLIntegerRangeOption FontSizeOption;

public:
	FBIndicatorStyle();

	Type type() const;
	bool isSensitive() const;
	bool isTextPositionShown() const;
	bool isTimeShown() const;
	ZLColor color() const;
	int height() const;
	int offset() const;
	int fontSize() const;
};

class FBView : public ZLTextView {

private:
	class TapScroller;

public:
	static FBIndicatorStyle& commonIndicatorInfo();
	static ZLBooleanOption &selectionOption();

	virtual bool hasContents() const;

private:
	static shared_ptr<ZLTextPositionIndicatorInfo> ourIndicatorInfo;
	static shared_ptr<ZLBooleanOption> ourSelectionOption;

protected:
	void doTapScrolling(int y);

public:
	FBView(ZLPaintContext &context);

	void setCaption(const std::string &caption);

private:
	bool onFingerTap(int x, int y);

	const std::string &caption() const;

	int leftMargin() const;
	int rightMargin() const;
	int topMargin() const;
	int bottomMargin() const;
	ZLColor backgroundColor() const;
	ZLColor color(const std::string &colorStyle) const;
	shared_ptr<ZLTextStyle> baseStyle() const;

	bool isSelectionEnabled() const;
	int doubleClickDelay() const;

protected:
	bool onStylusPress(int x, int y);
	virtual bool _onStylusPress(int x, int y);
	bool onStylusRelease(int x, int y);
	virtual bool _onStylusRelease(int x, int y);
	bool onStylusMove(int x, int y);
	virtual bool _onStylusMove(int x, int y);
	bool onStylusMovePressed(int x, int y);
	virtual bool _onStylusMovePressed(int x, int y);

	int pressedX() const;
	int pressedY() const;
	bool isReleasedWithoutMotion() const;

	std::string word(const ZLTextElementRectangle &rectangle) const;

	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo() const;

private:
	std::string myCaption;

	int myPressedX;
	int myPressedY;
	bool myIsReleasedWithoutMotion;

	shared_ptr<ZLRunnable> myTapScroller;
};

inline int FBView::pressedX() const { return myPressedX; }
inline int FBView::pressedY() const { return myPressedY; }
inline bool FBView::isReleasedWithoutMotion() const { return myIsReleasedWithoutMotion; }

#endif /* __FBVIEW_H__ */
