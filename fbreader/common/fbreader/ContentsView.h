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

#ifndef __CONTENTSVIEW_H__
#define __CONTENTSVIEW_H__

#include "../textview/TextView.h"

class FBReader;

class ContentsView : public TextView {

public:
	ContentsView(FBReader &reader, PaintContext &context);
	const std::string caption() const { return myCaption; }
	void setCaption(const std::string &caption) { myCaption = caption; }

	bool isEmpty() const;

	bool onStylusPress(int x, int y);

protected:
	const std::string paragraphOptionName() const;
	const std::string wordOptionName() const;
	const std::string charOptionName() const;

private:
	FBReader &myReader;
	std::string myCaption;
};

#endif /* __CONTENTSVIEW_H__ */
