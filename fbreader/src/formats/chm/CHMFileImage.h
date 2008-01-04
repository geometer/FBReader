/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __CHMFILEIMAGE_H__
#define __CHMFILEIMAGE_H__

#include <ZLStreamImage.h>

#include "CHMFile.h"

class CHMFileImage : public ZLStreamImage {

public:
	CHMFileImage(shared_ptr<CHMFileInfo> info, const std::string &entry);

private:
	shared_ptr<ZLInputStream> inputStream() const;

private:
	shared_ptr<CHMFileInfo> myInfo;
	std::string myEntry;
};

#endif /* __CHMFILEIMAGE_H__ */
