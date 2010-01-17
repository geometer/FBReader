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

#ifndef __ZLAPPLICATIONWINDOW_H__
#define __ZLAPPLICATIONWINDOW_H__

#include <string>

#include "ZLApplication.h"
#include "ZLToolbar.h"

class ZLApplicationWindow {

public:
	static ZLApplicationWindow &Instance();
	
private:
	static ZLApplicationWindow *ourInstance;
	
protected:
	ZLApplicationWindow(ZLApplication *application);

public:
	virtual ~ZLApplicationWindow();

public:
	ZLApplication &application() const;

protected:
	virtual ZLViewWidget *createViewWidget() = 0;

	virtual void init();

	enum ToolbarType {
		WINDOW_TOOLBAR,
		FULLSCREEN_TOOLBAR
	};

private:
	void initToolbar(ToolbarType type);

protected:
	// TODO: change to pure virtual
	virtual void initMenu() {};

	ToolbarType type(const ZLToolbar::Item &item) const;
	bool hasFullscreenToolbar() const;
	void onButtonPress(const ZLToolbar::AbstractButtonItem &button);
	virtual void addToolbarItem(ZLToolbar::ItemPtr item) = 0;
	// TODO: change to pure virtual
	virtual void setToolbarItemState(ZLToolbar::ItemPtr /*item*/, bool /*visible*/, bool /*enabled*/) {};
	virtual void setToggleButtonState(const ZLToolbar::ToggleButtonItem &toggleButton) = 0;

	virtual void refresh();
	virtual void processAllEvents() = 0;

private:
	void refreshToolbar(ToolbarType type);

protected:
	// TODO: change to pure virtual
	virtual void present() {}

	virtual void close() = 0;

	virtual void setCaption(const std::string &caption) = 0;

	virtual void grabAllKeys(bool grab) = 0;

	virtual void setFullscreen(bool fullscreen) = 0;
	virtual bool isFullscreen() const = 0;

	// TODO: change to pure virtual (?)
	virtual void setHyperlinkCursor(bool) {}

	void setVisualParameter(const std::string &id, const std::string &value);
	void setParameterValueList(const std::string &id, const std::vector<std::string> &values);
	const std::string &visualParameter(const std::string &id);

protected:
	class VisualParameter {

	public:
		virtual ~VisualParameter();

		const std::string &value() const;
		void setValue(const std::string &value);
		virtual void setValueList(const std::vector<std::string> &values) = 0;

	protected:
		virtual std::string internalValue() const = 0;
		virtual void internalSetValue(const std::string &value) = 0;
		void restoreOldValue();

	private:
		mutable std::string myValue;
	};

protected:
	void addVisualParameter(const std::string &id, shared_ptr<VisualParameter> parameter);

private:
	ZLApplication *myApplication;
	bool myToggleButtonLock;
	std::map<std::string,shared_ptr<VisualParameter> > myParameterMap;

friend class ZLApplication;
};

inline ZLApplicationWindow::~ZLApplicationWindow() {}
inline ZLApplication &ZLApplicationWindow::application() const { return *myApplication; }

#endif /* __ZLAPPLICATIONWINDOW_H__ */
