/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLWIN32FILEOUTPUTSTREAM_H__
#define __ZLWIN32FILEOUTPUTSTREAM_H__

#include <windows.h>

#include <ZLOutputStream.h>

class ZLWin32FileOutputStream : public ZLOutputStream {

public:
	ZLWin32FileOutputStream(const std::string &name);
	~ZLWin32FileOutputStream();
	bool open();
	void write(const char *data, size_t len);
	void write(const std::string &str);
	void close();

private:
	std::string myName;
	std::string myTemporaryName;
	bool myHasErrors;
	HANDLE myFile;
};

#endif /* __ZLWIN32FILEOUTPUTSTREAM_H__ */
