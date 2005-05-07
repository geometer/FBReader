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

#ifndef __COLLECTIONVIEW_H__
#define __COLLECTIONVIEW_H__

#include <map>

#include "../textview/TextView.h"

class FBReader;
class BookCollection;
class BookDescription;
class TreeModel;
class TreeParagraph;

class CollectionView : public TextView {

public:
	CollectionView(FBReader &reader, PaintContext &context) FB_VIEW_SECTION;
	~CollectionView() FB_VIEW_SECTION;
	const std::string &caption() const FB_VIEW_SECTION;

	void fill() FB_VIEW_SECTION;
	void gotoParagraph(int num, bool last = false) FB_VIEW_SECTION;
	bool onStylusPress(int x, int y) FB_VIEW_SECTION;

	void paint() FB_VIEW_SECTION;

private:
	FBReader &myReader;
	BookCollection *myCollection;
	TreeModel *myTreeModel;
	std::map<TreeParagraph*,BookDescription*> myBooksMap;
	bool myTreeStateIsFrozen;
};

#endif /* __COLLECTIONVIEW_H__ */
