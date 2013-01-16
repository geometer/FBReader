/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLOPTIONSDIALOG_H__
#define __ZLOPTIONSDIALOG_H__

#include <vector>
#include <string>

#include <shared_ptr.h>

#include <ZLRunnable.h>
#include <ZLDialogContent.h>
#include <ZLOptions.h>
#include <ZLResource.h>

class ZLOptionEntry;

class ZLDialogContentBuilder;

class ZLOptionsDialog {

protected:
	static void addPlatformDependentBuilder(shared_ptr<ZLDialogContentBuilder> builder);

private:
	static std::vector<shared_ptr<ZLDialogContentBuilder> > ourPlatformDependentBuilders;

protected:
	ZLOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction);
	
public:
	virtual ~ZLOptionsDialog();
	virtual ZLDialogContent &createTab(const ZLResourceKey &pageKey, const ZLResourceKey &tabKey) = 0;
	void createPlatformDependentTabs();
	virtual bool run() = 0;

protected:
	void accept();

	const std::string &caption() const;

private:
protected:
	const ZLResource &myResource;

protected:
	shared_ptr<ZLRunnable> myApplyAction;
	std::vector<shared_ptr<ZLDialogContent> > myTabs;
};

#endif /* __ZLOPTIONSDIALOG_H__ */
