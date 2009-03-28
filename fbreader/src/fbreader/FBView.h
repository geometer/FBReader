/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

class FBReader;

class FBMargins {

public:
	ZLIntegerRangeOption LeftMarginOption;
	ZLIntegerRangeOption RightMarginOption;
	ZLIntegerRangeOption TopMarginOption;
	ZLIntegerRangeOption BottomMarginOption;

	FBMargins();

private:
	FBMargins(const FBMargins&);
	const FBMargins& operator = (const FBMargins&);
};

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

public:
	static FBMargins& margins();
	static FBIndicatorStyle& commonIndicatorInfo();
	static ZLBooleanOption &selectionOption();

	void scrollAndUpdatePage(bool forward, ScrollingMode mode, unsigned int value);

	virtual bool hasContents() const;

private:
	static shared_ptr<ZLTextPositionIndicatorInfo> ourIndicatorInfo;
	static shared_ptr<FBMargins> ourMargins;
	static shared_ptr<ZLBooleanOption> ourSelectionOption;

protected:
	void doTapScrolling(int y);

public:
	FBView(FBReader &reader, shared_ptr<ZLPaintContext> context);

	bool onFingerTap(int x, int y);

	const std::string &caption() const;
	void setCaption(const std::string &caption);

	int leftMargin() const;
	int rightMargin() const;
	int topMargin() const;
	int bottomMargin() const;

	bool isSelectionEnabled() const;

protected:
	FBReader &fbreader();
	const FBReader &fbreader() const;

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

	std::string word(const ZLTextElementArea &area) const;

	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo() const;

private:
	std::string myCaption;

	int myPressedX;
	int myPressedY;
	bool myIsReleasedWithoutMotion;
};

inline FBReader &FBView::fbreader() { return (FBReader&)application(); }
inline const FBReader &FBView::fbreader() const { return (const FBReader&)application(); }

inline FBMargins& FBView::margins() {
	if (ourMargins.isNull()) {
		ourMargins = new FBMargins();
	}
	return *ourMargins;
}

inline int FBView::pressedX() const { return myPressedX; }
inline int FBView::pressedY() const { return myPressedY; }
inline bool FBView::isReleasedWithoutMotion() const { return myIsReleasedWithoutMotion; }

#endif /* __FBVIEW_H__ */
