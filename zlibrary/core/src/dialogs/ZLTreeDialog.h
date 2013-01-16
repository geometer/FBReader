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

#ifndef __ZLTREEDIALOG_H__
#define __ZLTREEDIALOG_H__

#include <ZLTreeListener.h>
#include <ZLResource.h>
#include <ZLTreeSearcher.h>

class ZLTreeDialog : public ZLTreeListener {

protected:
	ZLTreeDialog(const ZLResource &resource);

public:
	//TODO maybe run() should return an integer?
	virtual void run(ZLTreeNode *rootNode) = 0;

public:
	void setSearcher(shared_ptr<ZLTreeSearcher> searcher);

	void onNodeBeginInsert(ZLTreeNode *parent, std::size_t index);
	void onNodeEndInsert();
	void onNodeBeginRemove(ZLTreeNode *parent, std::size_t index);
	void onNodeEndRemove();

protected:
	const ZLResource &resource() const;

protected:
	shared_ptr<ZLTreeSearcher> mySearcher;

private:
	const ZLResource &myResource;
};

#endif /* __ZLTREEDIALOG_H__ */
