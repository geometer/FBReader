/*
 * Copyright (C) 2009-2013 Geometer Plus <contact@geometerplus.com>
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

class ZLTreePageInfo {
public:

	virtual bool isPageInfoLoaded() = 0;
	virtual void loadAll(shared_ptr<ZLNetworkRequest::Listener> listener) = 0;

	virtual std::string title() const = 0;
	virtual std::vector<std::string> authors() const = 0;
	virtual std::vector<std::string> tags() const = 0;
	virtual std::string summary() const = 0;
	virtual shared_ptr<const ZLImage> image() const = 0;
    virtual void updateActions() const = 0;

	virtual const std::vector<shared_ptr<ZLTreeAction> > &actions() const = 0;
	virtual std::string actionText(const shared_ptr<ZLTreeAction> &action) const = 0;
	virtual const std::vector<shared_ptr<ZLTreeAction> > relatedActions() const = 0;
};

class ZLTreePageNode : public ZLTreeTitledNode {

public:
	ZLTreePageNode(ZLTreeNode *parent = 0, std::size_t position = -1);
	static const ZLTypeId TYPE_ID;
	const ZLTypeId &typeId() const;

	virtual shared_ptr<ZLTreePageInfo> getPageInfo() = 0;
};

#endif // ZLTREEPAGENODE_H
