/*
 * Copyright (C) 2009-2011 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLTREETITLEDNODE_H__
#define __ZLTREETITLEDNODE_H__

#include "ZLTreeNode.h"
#include <ZLImage.h>

class ZLTreeTitledNode : public ZLTreeNode {

public:
	static const ZLTypeId TYPE_ID;
	const ZLTypeId &typeId() const;

	static const std::string ZLIMAGE_SCHEME;
	static const std::string LOCALFILE_SCHEME;
	static const std::string SCHEME_POSTFIX;

public:
	ZLTreeTitledNode(ZLTreeNode *parent = 0, size_t position = -1);
	virtual ~ZLTreeTitledNode();

	virtual std::string title() const = 0;
	virtual std::string subtitle() const;

	// Url to image
	//virtual std::string imageUrl() const;
	// Or the image if there is no url
	virtual shared_ptr<const ZLImage> image() const;
};

#endif /* __ZLTREETITLEDNODE_H__ */
