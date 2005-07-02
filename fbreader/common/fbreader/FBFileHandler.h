/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __FBFILEHANDLER_H__
#define __FBFILEHANDLER_H__

#include <abstract/ZLOpenFileDialog.h>

#include "../description/BookDescription.h"

class FBFileHandler : public ZLTreeHandler {

public:
	FBFileHandler() FB_DIALOG_SECTION;
	~FBFileHandler() FB_DIALOG_SECTION;

	bool isNodeVisible(const ZLTreeNodePtr node) const FB_DIALOG_SECTION;
	const std::string &pixmapName(const ZLTreeNodePtr node) const FB_DIALOG_SECTION;
	void accept(const ZLTreeStatePtr state) const FB_DIALOG_SECTION;

	BookDescriptionPtr description() const FB_DIALOG_SECTION;

private:
	mutable BookDescriptionPtr myDescription;
};

inline FBFileHandler::FBFileHandler() : myDescription(0) {}
inline FBFileHandler::~FBFileHandler() {}
inline BookDescriptionPtr FBFileHandler::description() const { return myDescription; }

#endif /* __FBFILEHANDLER_H__ */
