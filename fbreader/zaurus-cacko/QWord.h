/*
 * FBReader -- electronic book reader
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

#ifndef __QWORD_H__
#define __QWORD_H__

#include <qstring.h>

#include "../common/textview/Word.h"

class QWord : public Word {

public:
	int length() const { return myValue.length(); }
	unsigned short charAt(int i) const { return myValue[i].unicode(); }
	std::string utf8String() const { return (const char*)myValue.utf8(); }
	
private:
	QWord(const char *utf8String, int len, int startOffset) : Word(startOffset) { myValue = QString::fromUtf8(utf8String, len); }

private:
	QWord(const QWord&);
	QWord &operator = (const QWord&);

private:
	QString myValue;

friend class QPaintContext;
friend class QWordBuilder;
};

class QWordBuilder : public WordBuilder {

public:
	static void createInstance() { ourInstance = new QWordBuilder(); }

	Word *buildWord(const char *utf8String, int len, int startOffset) const { return new QWord(utf8String, len, startOffset); }

private:
	QWordBuilder() {}
};

#endif /* __QWORD_H__ */
