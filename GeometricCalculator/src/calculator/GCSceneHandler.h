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

#ifndef __GCSCENEHANDLER_H__
#define __GCSCENEHANDLER_H__

#include <ZLSelectionDialog.h>

class ZLDir;

class GCSceneHandler {

protected:
	static const std::string SamplesFolderName;
	static const std::string UserFolderName;

	static shared_ptr<ZLDir> SamplesDirectory();
	static const std::string UserDirectoryName();
	static shared_ptr<ZLDir> UserDirectory(bool create);

protected:
	GCSceneHandler();

	void collectSubnodes(const std::string &folderName, shared_ptr<ZLDir> dir) const;
	void resetSubnodesList() const;
	const std::vector<ZLTreeNodePtr> &subnodes() const;
	bool isUpToDate() const;

private:
	mutable bool myIsUpToDate;
	mutable std::vector<ZLTreeNodePtr> mySubnodes;
};

#endif /* __GCSCENEHANDLER_H__ */
