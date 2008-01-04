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

#ifndef __ZLOPTIONVIEW_H__
#define __ZLOPTIONVIEW_H__

#include <string>

#include <shared_ptr.h>

class ZLOptionEntry;

class ZLOptionView {

public:
	ZLOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	virtual ~ZLOptionView();

	// TODO: change to pure virtual
	virtual void reset() {}

	void setVisible(bool visible);
	void setActive(bool active);
	void onAccept() const;

protected:
	virtual void _createItem() = 0;
	virtual void _hide() = 0;
	virtual void _show() = 0;
	// TODO: replace by pure virtual method
	virtual void _setActive(bool active) {}
	virtual void _onAccept() const = 0;

	const std::string &name() const;
	const std::string &tooltip() const;

protected:
	const std::string myName;
	const std::string myTooltip;
	shared_ptr<ZLOptionEntry> myOption;

private:
	bool myInitialized;
};

#endif /* __ZLOPTIONVIEW_H__ */
