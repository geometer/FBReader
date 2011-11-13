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

#ifndef __NETWORKVIEW_H__
#define __NETWORKVIEW_H__

#include <set>
#include <map>

#include <ZLBlockTreeView.h>
#include <ZLTreeDialog.h>

#include "../network/NetworkItems.h"

class NetworkAuthorNode;
class NetworkCatalogNode;
class NetworkItem;
class FBReaderNode;

class NetworkView {

public:
	NetworkView();

	void invalidate();
	void invalidateAccountDependents();
	
	void showDialog();
	


//	void drawCoverLater(FBReaderNode *node, int vOffset);

private:
	const std::string &caption() const;


	void updateAccountDependents();
	void updateAccountDependents(NetworkCatalogNode &node);
	bool processAccountDependent(NetworkCatalogItem &item);
	void paint();
	void makeUpToDate();
	
	class AddCatalogAction;
	friend class AddCatalogAction;

private:
	shared_ptr<ZLTreeDialog> myDialog;
	bool myUpdateChildren;
	bool myUpdateAccountDependents;
//	std::map<FBReaderNode*,int> myNodesToPaintCovers;
	std::vector<shared_ptr<ZLImage> > myStartedCovers;
};

inline void NetworkView::invalidate() { myUpdateChildren = true; }
inline void NetworkView::invalidateAccountDependents() { myUpdateAccountDependents = true; }

#endif /* __NETWORKVIEW_H__ */
