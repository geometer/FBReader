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

#ifndef __ZLQTFILEOUTPUTSTREAM_H__
#define __ZLQTFILEOUTPUTSTREAM_H__

#include <QtCore/QFile>

#include <ZLOutputStream.h>

class ZLQtFileOutputStream : public ZLOutputStream {

public:
        ZLQtFileOutputStream(const std::string &name);
        ~ZLQtFileOutputStream();
	bool open();
	void write(const char *data, size_t len);
	void write(const std::string &str);
	void close();

private:
	std::string myName;
	std::string myTemporaryName;
	bool myHasErrors;
        QFile myFile;
};

#endif /* __ZLQTFILEOUTPUTSTREAM_H__ */
