/*
 * Geometric Calculator -- dynamic geometry software
 * Copyright (C) 2003-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __GCSAVESCENEHANDLER_H__
#define __GCSAVESCENEHANDLER_H__

#include <map>
#include <set>
#include <vector>

#include <ZLSelectionDialog.h>
#include <ZLOptions.h>

class GCSaveSceneHandler : public ZLTreeSaveHandler {

public:
	GCSaveSceneHandler(const std::string &sceneName);

	const std::string sceneName() const;
	const std::string fileName() const;

private:
	bool accept(const std::string &state) const;
	void processNode(const ZLTreeNode &node) const;

	const std::string stateDisplayName() const;
	void changeFolder(const ZLTreeNode &node);
	const std::vector<ZLTreeNodePtr> &subnodes() const;
	std::string relativeId(const ZLTreeNode &node) const;

private:
	mutable std::string mySceneName;

	mutable bool myIsUpToDate;
	mutable std::vector<ZLTreeNodePtr> mySubnodes;
	mutable std::map<std::string,std::string> myFileNameBySceneName;
	mutable std::set<std::string> myFileNames;
};

#endif /* __GCSAVESCENEHANDLER_H__ */
