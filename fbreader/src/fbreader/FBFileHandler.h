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

#ifndef __FBFILEHANDLER_H__
#define __FBFILEHANDLER_H__

#include <ZLSelectionDialog.h>

#include "../description/BookDescription.h"

class FBFileHandler : public ZLTreeHandler {

public:
	ZLStringOption DirectoryOption;

public:
	FBFileHandler();
	~FBFileHandler();

	BookDescriptionPtr description() const;

private:
	bool isWriteable() const;

	const std::string accept(const std::string &nodeId) const;

	const std::string stateDisplayName() const;
	void changeFolder(const std::string &nodeId);
	const std::vector<ZLTreeNodePtr> &subnodes() const;
	std::string relativeId(const ZLTreeNode &node) const;

private:
	shared_ptr<ZLDir> myDir;
	mutable bool myIsUpToDate;
	mutable std::vector<ZLTreeNodePtr> mySubnodes;

	mutable BookDescriptionPtr myDescription;
};

inline BookDescriptionPtr FBFileHandler::description() const { return myDescription; }

#endif /* __FBFILEHANDLER_H__ */
