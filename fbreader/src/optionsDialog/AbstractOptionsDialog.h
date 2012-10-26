/*
 * Copyright (C) 2010-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ABSTRACTOPTIONSDIALOG_H__
#define __ABSTRACTOPTIONSDIALOG_H__

#include <vector>
#include <shared_ptr.h>

class ZLOptionsDialog;
class ZLResourceKey;
class ProgramCollection;
class ZLOption;
class ZLOptionEntry;

class AbstractOptionsDialog {

public:
	AbstractOptionsDialog(const ZLResourceKey &key, bool showApplyButton);

	ZLOptionsDialog &dialog();

protected:
	void createIntegrationTab(shared_ptr<ProgramCollection> collection, const ZLResourceKey &key, std::vector<std::pair<ZLResourceKey,ZLOptionEntry*> > &additionalOptions);
	void storeTemporaryOption(ZLOption *option);

private:
	shared_ptr<ZLOptionsDialog> myDialog;
	std::vector<shared_ptr<ZLOption> > myTemporaryOptions;
};

inline ZLOptionsDialog &AbstractOptionsDialog::dialog() { return *myDialog; }

#endif /* __ABSTRACTOPTIONSDIALOG_H__ */
