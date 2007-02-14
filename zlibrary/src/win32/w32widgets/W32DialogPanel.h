/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __W32DIALOGPANEL_H__
#define __W32DIALOGPANEL_H__

#include <map>
#include <string>

#include <windows.h>
#include <prsht.h>

#include <ZLUnicodeUtil.h>

#include "W32Widget.h"
#include "W32Event.h"
#include "W32ControlCollection.h"

class W32DialogPanel : public W32ControlCollection, public W32EventSender {

public:
	static const std::string PANEL_SELECTED_EVENT;
	static const std::string CANCEL_EVENT;
	static const std::string OK_EVENT;

private:
	static const UINT LAYOUT_MESSAGE;
	static BOOL CALLBACK StaticCallback(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static std::map<HWND,W32DialogPanel*> ourPanels;

public:
	W32DialogPanel(HWND mainWindow, const std::string &caption);
	~W32DialogPanel();
	DLGTEMPLATE *dialogTemplate();
	void setElement(W32WidgetPtr element);
	W32Widget::Size charDimension() const;
	const std::string &caption() const;

	bool runDialog();

	void setExitOnOk(bool exit);
	void setExitOnCancel(bool exit);

private:
	void calculateSize();
	W32Widget::Size size() const;
	void setSize(W32Widget::Size size);

	void init(HWND dialogWindow);
	bool commandCallback(WPARAM wParam);
	bool notificationCallback(WPARAM wParam, LPARAM lParam);
	void invalidate();
	void layout();
	void endDialog(bool code);

private:
	HWND myMainWindow;
	W32Widget::Size myCharDimension;
	W32Widget::Size mySize;
	std::string myCaption;

	W32WidgetPtr myElement;

	mutable WORD *myAddress;
	HWND myDialogWindow;
	bool myDoLayout;

	bool myExitOnCancel;
	bool myExitOnOk;

friend class W32PropertySheet;
};

class W32PropertySheet {

private:
	static BOOL CALLBACK StaticCallback(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam);

public:
	W32PropertySheet(HWND mainWindow, const std::string &caption);

	W32DialogPanel &createPanel(const std::string &name);
	bool run(const std::string &selectedTabName);

private:
	HWND myMainWindow;
	ZLUnicodeUtil::Ucs2String myCaption;
	typedef std::vector<shared_ptr<W32DialogPanel> > PanelList;
	PanelList myPanels;
};

#endif /* __W32DIALOGPANEL_H__ */
