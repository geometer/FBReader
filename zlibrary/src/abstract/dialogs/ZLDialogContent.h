/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __ZLDIALOGCONTENT_H__
#define __ZLDIALOGCONTENT_H__

#include <string>
#include <vector>

class ZLOptionEntry;
class ZLOptionView;

class ZLDialogContent {

protected:
	ZLDialogContent();
	ZLDialogContent(const std::string &name);

public:
	virtual ~ZLDialogContent();

	const std::string &name() const;

	virtual void addOption(ZLOptionEntry *option) = 0;
	virtual void addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) = 0;

	void accept();

protected:
	void addView(ZLOptionView *view);

private:
	const std::string myName;
	std::vector<ZLOptionView*> myViews;
};

#endif /* __ZLDIALOGCONTENT_H__ */
