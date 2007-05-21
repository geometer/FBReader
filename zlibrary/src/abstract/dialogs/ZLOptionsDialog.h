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

class ZLOptionsDialog {

protected:
	ZLOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction);
	
public:
	virtual ~ZLOptionsDialog();
	virtual ZLDialogContent &createTab(const std::string &name) = 0;
	virtual bool run(const std::string &tabName);

protected:
	virtual const std::string &selectedTabName() const = 0;
	virtual void selectTab(const std::string &name) = 0;
	virtual bool run() = 0;

	void accept();
	const std::string &caption() const;

protected:
	ZLStringOption TabOption;

	const ZLResource &myResource;

	shared_ptr<ZLRunnable> myApplyAction;
	std::vector<shared_ptr<ZLDialogContent> > myTabs;
};

#endif /* __ZLOPTIONSDIALOG_H__ */
