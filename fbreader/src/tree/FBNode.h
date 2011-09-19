/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __FBNODE_H__
#define __FBNODE_H__

#include <map>
#include <vector>

#include <ZLTreeTitledNode.h>

class ZLImage;
class ZLResource;

class FBNode : public ZLTreeTitledNode {

public:
	static shared_ptr<ZLImage> defaultCoverImage(const std::string &id);
	static std::string defaultImageUrl(const std::string &id);

private:
	static std::map<std::string,shared_ptr<ZLImage> > ourDefaultCovers;
	static std::map<std::string,std::string> ourDefaultUrls;

public:
	static const ZLTypeId TYPE_ID;

protected:
	virtual const ZLResource &resource() const;

protected:
	virtual shared_ptr<ZLImage> extractCoverImage() const;

private:
	const ZLTypeId &typeId() const;

public:
	shared_ptr<ZLImage> image() const;
	virtual std::string title() const = 0;
	virtual std::string subtitle() const = 0;

private:
	mutable bool myCoverImageIsStored;
	mutable shared_ptr<ZLImage> myStoredCoverImage;
};

#endif /* __FBNODE_H__ */
