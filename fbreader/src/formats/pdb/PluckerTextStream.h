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

#ifndef __PLUCKERTEXTSTREAM_H__
#define __PLUCKERTEXTSTREAM_H__

#include "PdbStream.h"

class ZLFile;

class PluckerTextStream : public PdbStream {

public:
	PluckerTextStream(ZLFile &file);
	~PluckerTextStream();
	bool open();
	void close();

protected:
	bool fillBuffer();

private:
	void processRecord(size_t recordSize);
	void processTextParagraph(char *start, char *end);

private:
	unsigned short myCompressionVersion;
	char *myFullBuffer;
	size_t myRecordIndex;
};

#endif /* __PLUCKERTEXTSTREAM_H__ */
