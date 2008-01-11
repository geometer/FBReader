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

#ifndef __ZLAPPLICATIONWINDOW_H__
#define __ZLAPPLICATIONWINDOW_H__

#include <string>

#include <ZLApplication.h>

class ZLApplicationWindow {

protected:
	ZLApplicationWindow(ZLApplication *application);

public:
	ZLApplication &application() const;

protected:
	virtual void init();
	// TODO: change to pure virtual
	virtual void initMenu() {}

	void onButtonPress(ZLApplication::Toolbar::ButtonItem &button);
	// TODO: change to pure virtual
	virtual void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem&) {}
	// TODO: change to pure virtual
	virtual void setToolbarItemState(ZLApplication::Toolbar::ItemPtr /*item*/, bool /*visible*/, bool /*enabled*/) {}

	virtual ZLViewWidget *createViewWidget() = 0;
	virtual void addToolbarItem(ZLApplication::Toolbar::ItemPtr item) = 0;

	// TODO: change to non-virtual (?)
	virtual void refresh();
	// TODO: change to pure virtual
	virtual void present() {}

	virtual void close() = 0;

	virtual void setCaption(const std::string &caption) = 0;

	virtual void grabAllKeys(bool grab) = 0;

	virtual void setFullscreen(bool fullscreen) = 0;
	virtual bool isFullscreen() const = 0;

	// TODO: change to pure virtual (?)
	virtual void setHyperlinkCursor(bool) {}

public:
	virtual ~ZLApplicationWindow();

private:
	ZLApplication *myApplication;
	bool myToggleButtonLock;

friend class ZLApplication;
};

inline ZLApplicationWindow::~ZLApplicationWindow() {}
inline ZLApplication &ZLApplicationWindow::application() const { return *myApplication; }

#endif /* __ZLAPPLICATIONWINDOW_H__ */
