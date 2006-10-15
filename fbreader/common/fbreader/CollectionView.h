/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __COLLECTIONVIEW_H__
#define __COLLECTIONVIEW_H__

#include <map>

#include "TreeView.h"
#include "../description/BookDescription.h"
#include "../collection/BookCollection.h"

class TreeModel;
class PlainTextModel;
class Paragraph;

class CollectionView : public TreeView {

public:
	static const std::string DeleteBookImageFile;
	
public:
	CollectionView(FBReader &reader, ZLPaintContext &context);
	~CollectionView();
	const std::string &caption() const;

	void rebuild();
	bool onStylusPress(int x, int y);

	void paint();

	BookCollection &collection();

private:
	BookCollection myCollection;
	TreeModel *myTreeModel;
	std::map<Paragraph*,BookDescriptionPtr> myBooksMap;
	bool myTreeStateIsFrozen;
	ImageMap myImageMap;
};

inline BookCollection &CollectionView::collection() {
	return myCollection;
}

#endif /* __COLLECTIONVIEW_H__ */
