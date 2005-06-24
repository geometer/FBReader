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

#ifndef __ZLOPENFILEDIALOG_H__
#define __ZLOPENFILEDIALOG_H__

#include <abstract/ZLOptions.h>

class ZLDir;

class ZLFileHandler {

protected:
	ZLFileHandler() DIALOG_SECTION;
	virtual ~ZLFileHandler() DIALOG_SECTION;
	
public:
	virtual bool isFileVisible(const std::string &shortFileName, bool dir) const DIALOG_SECTION = 0;
	virtual const std::string &pixmapName(const std::string &shortFileName, bool dir) const DIALOG_SECTION = 0;
	virtual void accept(const std::string &fullFileName, bool dir) const DIALOG_SECTION = 0;
};

class ZLOpenFileDialog {

public:
	static ZLStringOption DirectoryOption;

protected:
	ZLOpenFileDialog(const ZLFileHandler &handler) DIALOG_SECTION;
	virtual ~ZLOpenFileDialog() DIALOG_SECTION;

public:
	virtual void run() DIALOG_SECTION = 0;

protected:
	const ZLFileHandler &handler() const DIALOG_SECTION;

protected:
	ZLDir *myCurrentDir;

private:
	const ZLFileHandler &myHandler;
};

inline ZLFileHandler::ZLFileHandler() {}
inline ZLFileHandler::~ZLFileHandler() {}
inline const ZLFileHandler &ZLOpenFileDialog::handler() const { return myHandler; }

#endif /* __ZLOPENFILEDIALOG_H__ */
