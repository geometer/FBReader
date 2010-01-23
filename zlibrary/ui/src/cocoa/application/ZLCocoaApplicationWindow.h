/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLCOCOAAPPLICATIONWINDOW_H__
#define __ZLCOCOAAPPLICATIONWINDOW_H__

#include <vector>
#include <map>

#include "../../../../core/src/application/ZLApplicationWindow.h"
#include "../../../../core/src/application/ZLToolbar.h"

class ZLCocoaViewWidget;

class ZLCocoaApplicationWindow : public ZLApplicationWindow { 

public:
	ZLCocoaApplicationWindow(ZLApplication *application);
	~ZLCocoaApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLToolbar::ItemPtr item, int index);
	void addToolbarItem(ZLToolbar::ItemPtr item);
	void init();
	void refresh();
	void processAllEvents();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	void setHyperlinkCursor(bool hyperlink);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void setToggleButtonState(const ZLToolbar::ToggleButtonItem &button);
	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);

private:
	// NSWindow*
	void *myWindow;
	std::vector<std::pair<ZLToolbar::ItemPtr,bool> > myToolbarItems;
};

#endif /* __ZLCOCOAAPPLICATIONWINDOW_H__ */
