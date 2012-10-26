/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLHEXINPUTSTREAM_H__
#define __ZLHEXINPUTSTREAM_H__

#include <ZLInputStream.h>

class ZLHexInputStream : public ZLInputStream {
public:
		ZLHexInputStream(shared_ptr<ZLInputStream> base);
		~ZLHexInputStream();

		bool open();
		int read();
		size_t read(char *buffer, size_t maxSize);
		void close();

		void seek(int offset, bool absoluteOffset);
		size_t offset() const;
		size_t sizeOfOpened();

private:
		bool fillBuffer();
		static int decode(char b);

private:
		shared_ptr<ZLInputStream> myBaseStream;
		char* myBuffer;
		int myBufferOffset;
		int myBufferLength;
};

#endif /* __ZLHEXINPUTSTREAM_H__ */
