/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __FBTREE_H__
#define __FBTREE_H__

#include <ZLTreeTitledNode.h>
#include <ZLTreeDialog.h>

class FBTree : public ZLTreeTitledNode {

public:
	static const ZLTypeId TYPE_ID;

private:
	const ZLTypeId &typeId() const;

private:
	class ExpandTreeAction;

public:
	static shared_ptr<const ZLImage> defaultCoverImage(const std::string &id);
	//static std::string defaultImageUrl(const std::string &id);

public: //TODO make protected
	void expand();

private:
	static std::map<std::string,shared_ptr<const ZLImage> > ourDefaultCovers;
	static std::map<std::string,std::string> ourDefaultUrls;

public:
	FBTree(ZLTreeNode *parent, std::size_t position = (std::size_t)-1);
//	shared_ptr<const ZLImage> coverImage() const;
	std::string subtitle() const;

protected:
	void registerExpandTreeAction();

private:
//	mutable bool myCoverImageIsStored;
//	mutable shared_ptr<const ZLImage> myStoredCoverImage;
};

//maybe RootTree should be nested class for NetworkLibrary?
class RootTree : public ZLTreeNode {

public:
    static const ZLTypeId TYPE_ID;

private:
    const ZLTypeId &typeId() const;

public:
    RootTree();
    void setDialog(shared_ptr<ZLTreeDialog> dialog);


protected:
    ZLTreeListener *listener() const;

private:
    shared_ptr<ZLTreeDialog> myListener;
};
#endif /* __FBTREE_H__ */
