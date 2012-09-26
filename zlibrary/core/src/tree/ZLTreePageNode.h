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

#ifndef ZLTREEPAGENODE_H
#define ZLTREEPAGENODE_H

#include "ZLTreeTitledNode.h"
#include <ZLDialogContent.h>

class ZLTreePageNode : public ZLTreeTitledNode {

public:
	static const ZLTypeId TYPE_ID;
	const ZLTypeId &typeId() const;
	
	shared_ptr<ZLDialogContent> content() const;
	
protected:
	virtual void fillContent(ZLDialogContent &content) const = 0;
	virtual ZLResourceKey contentKey() const = 0;
	
private:
        mutable shared_ptr<ZLDialogContent> myContent;
};

#endif // ZLTREEPAGENODE_H
