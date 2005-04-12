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

class FBFileHandler : public ZLFileHandler {

public:
	FBFileHandler() : myDescription(0) {}

	bool isFileVisible(const std::string &shortFileName, bool dir) const;
	const std::string &pixmapName(const std::string &shortFileName, bool dir) const;
	void accept(const std::string &fullFileName, bool dir) const;

	BookDescription *description() const { return myDescription; }

private:
	mutable BookDescription *myDescription;
};

#endif /* __FBFILEHANDLER_H__ */
