/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLUnicodeUtil.h>

#include "W32Widget.h"
#include "W32Event.h"
#include "W32ControlCollection.h"

class W32DialogPanel : public W32ControlCollection, public W32EventSender {

public:
	static const std::string PANEL_SELECTED_EVENT;
	static const std::string APPLY_BUTTON_PRESSED_EVENT;

private:
	static UINT LAYOUT_MESSAGE;

private:
	static std::map<HWND,W32DialogPanel*> ourPanels;

public:
	W32DialogPanel(HWND mainWindow, const std::string &caption);
	virtual ~W32DialogPanel();
	DLGTEMPLATE *dialogTemplate();
	void setElement(W32WidgetPtr element);
	const std::string &caption() const;

	W32Widget::Size size() const;
	void setSize(W32Widget::Size size);

private:
	void calculateSize();
	void updateElementSize();

	void init(HWND dialogWindow);
	virtual bool commandCallback(WPARAM wParam);
	bool notificationCallback(WPARAM wParam, LPARAM lParam);
	bool drawItemCallback(WPARAM wParam, DRAWITEMSTRUCT &di);
	void invalidate();
	void layout();

	virtual DWORD style() const;

	void hScroll(WORD command);
	void vScroll(WORD command);

protected:
	W32Widget::Size minimumSize() { return myMinimumSize; }

private:
	HWND myMainWindow;
	W32Widget::Size myMinimumSize;
	W32Widget::Size mySize;
	W32Widget::Size myRealSize;
	std::string myCaption;

	W32WidgetPtr myElement;

	mutable WORD *myAddress;
	HWND myPanelWindow;
	bool myDoLayout;

friend class W32StandaloneDialogPanel;
friend class W32PropertySheet;
};

class W32StandaloneDialogPanel : public W32DialogPanel {

public:
	static const std::string CANCEL_EVENT;
	static const std::string OK_EVENT;

private:
	static BOOL CALLBACK StaticCallback(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam);

public:
	W32StandaloneDialogPanel(HWND mainWindow, const std::string &caption);

	bool runDialog(short width = 0, short height = 0);
	void endDialog(bool code);

	void setResizeable(bool resizeable);

	void setExitOnOk(bool exit);
	void setExitOnCancel(bool exit);

private:
	bool commandCallback(WPARAM wParam);
	DWORD style() const;

private:
	bool myResizeable;
	bool myExitOnCancel;
	bool myExitOnOk;
};

class W32PropertySheet {

private:
	static bool ourPropertySheetStarted;

private:
	static int CALLBACK PSCallback(HWND, UINT message, LPARAM lParam);
	static BOOL CALLBACK StaticCallback(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam);

public:
	W32PropertySheet(HWND mainWindow, const std::string &caption, bool showApplyButton);

	W32DialogPanel &createPanel(const std::string &name);
	bool run(const std::string &selectedTabName);

private:
	HWND myMainWindow;
	HWND myDialogWindow;
	const bool myShowApplyButton;
	ZLUnicodeUtil::Ucs2String myCaption;
	typedef std::vector<shared_ptr<W32DialogPanel> > PanelList;
	PanelList myPanels;
};

#endif /* __W32DIALOGPANEL_H__ */
