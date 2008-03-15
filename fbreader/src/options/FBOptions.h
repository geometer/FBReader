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

#ifndef __FBOPTIONS_H__
#define __FBOPTIONS_H__

#include <ZLOptions.h>

class FBCategoryKey : public ZLCategoryKey {

public:
	static const FBCategoryKey BOOKS;
	static const FBCategoryKey SEARCH;
	static const FBCategoryKey EXTERNAL;

private:
	FBCategoryKey(const std::string &name);
};

#endif /* __FBOPTIONS_H__ */
