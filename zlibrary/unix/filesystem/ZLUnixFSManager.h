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

#ifndef __ZLUNIXFSMANAGER_H__
#define __ZLUNIXFSMANAGER_H__

#include <abstract/ZLFSManager.h>

class ZLUnixFSManager : public ZLFSManager {

public:
	static void createInstance() { ourInstance = new ZLUnixFSManager(); }
	
private:
	ZLUnixFSManager() {}
	
public:
	void normalize(std::string &path) const;

protected:
	ZLInputStream *createPlainInputStream(const std::string &path) const;
	ZLOutputStream *createOutputStream(const std::string &path) const;
	ZLFSDir *createPlainDirectory(const std::string &path) const;
	ZLFSDir *createNewDirectory(const std::string &path) const;
	FileInfo fileInfo(const std::string &path) const;
	bool isZipSupported() const;
};

#endif /* __ZLUNIXFSMANAGER_H__ */
