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

#ifndef __ZLDIALOGCONTENT_H__
#define __ZLDIALOGCONTENT_H__

#include <string>
#include <vector>

#include <ZLOptions.h>
#include <ZLResource.h>

class ZLOptionEntry;
class ZLOptionView;

class ZLDialogContent {

protected:
	ZLDialogContent(const ZLResource &resource);

public:
	virtual ~ZLDialogContent();

	const std::string &key() const;
	const std::string &displayName() const;
	const std::string &value(const ZLResourceKey &key) const;
	const ZLResource &resource(const ZLResourceKey &key) const;

	virtual void addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option) = 0;
	void addOption(const ZLResourceKey &key, ZLOptionEntry *option);
	void addOption(const ZLResourceKey &key, ZLSimpleOption &option);
	virtual void addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0,
													const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1) = 0;
	void addOptions(const ZLResourceKey &key0, ZLOptionEntry *option0,
									const ZLResourceKey &key1, ZLOptionEntry *option1);
	void addOptions(const ZLResourceKey &key0, ZLSimpleOption &option0,
									const ZLResourceKey &key1, ZLSimpleOption &option1);

	void accept();

protected:
	void addView(ZLOptionView *view);

private:
	const ZLResource &myResource;
	std::vector<ZLOptionView*> myViews;
};

#endif /* __ZLDIALOGCONTENT_H__ */
