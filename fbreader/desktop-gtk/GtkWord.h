/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __GTKWORD_H__
#define __GTKWORD_H__

#include <abstract/ZLUnicodeUtil.h>

#include "../common/textview/Word.h"

class GtkWord : public Word {

public:
	int length() const { return myUcs2Contents.size(); }
	unsigned short charAt(int i) const { return myUcs2Contents[i]; }
	std::string utf8String() const { return myUtf8Contents; }
	
private:
	GtkWord(const char *utf8String, int len, int startOffset);

private:
	GtkWord(const GtkWord&);
	GtkWord &operator = (const GtkWord&);

private:
	std::string myUtf8Contents;
	ZLUnicodeUtil::Ucs2String myUcs2Contents;

friend class GtkWordBuilder;
};

class GtkWordBuilder : public WordBuilder {

public:
	static void createInstance() { ourInstance = new GtkWordBuilder(); }

	Word *buildWord(const char *utf8String, int len, int startOffset) const { return new GtkWord(utf8String, len, startOffset); }

private:
	GtkWordBuilder() {}
};

#endif /* __GTKWORD_H__ */
