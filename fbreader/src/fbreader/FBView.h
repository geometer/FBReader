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

#ifndef __FBVIEW_H__
#define __FBVIEW_H__

#include <ZLTextView.h>

class FBReader;

class FBView : public ZLTextView {

public:
	FBView(FBReader &reader, shared_ptr<ZLPaintContext> context);

	bool onFingerTap(int x, int y);

	const std::string &caption() const;
	void setCaption(const std::string &caption);

protected:
	FBReader &fbreader();
	const FBReader &fbreader() const;

	bool onStylusPress(int x, int y);
	virtual bool _onStylusPress(int x, int y);

	std::string word(const ZLTextElementArea &area) const;

private:
	std::string myCaption;
};

inline FBReader &FBView::fbreader() { return (FBReader&)application(); }
inline const FBReader &FBView::fbreader() const { return (const FBReader&)application(); }

#endif /* __FBVIEW_H__ */
