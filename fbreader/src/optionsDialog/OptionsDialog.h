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

#ifndef __OPTIONSDIALOG_H__
#define __OPTIONSDIALOG_H__

#include <shared_ptr.h>

class ZLOptionsDialog;
class ZLDialogContent;
class OptionsPage;
class KeyBindingsPage;
class ConfigPage;
class NetworkLibraryPage;
class ZLPaintContext;
class ProgramCollection;

class ZLBooleanOptionEntry;
class ZLSpinOptionEntry;
class ZLOption;

class OptionsDialog {

public:
	OptionsDialog();
	~OptionsDialog();

	ZLOptionsDialog &dialog();

private:
	void createIntegrationTab(shared_ptr<ProgramCollection> collection, const ZLResourceKey &key, std::vector<std::pair<ZLResourceKey,ZLOptionEntry*> > &additionalOptions);
	void createIndicatorTab();
	void storeTemporaryOption(ZLOption *option);

private:
	shared_ptr<ZLOptionsDialog> myDialog;
	shared_ptr<OptionsPage> myFormatPage;
	shared_ptr<OptionsPage> myStylePage;
	shared_ptr<KeyBindingsPage> myKeyBindingsPage;
	shared_ptr<ConfigPage> myConfigPage;
	shared_ptr<NetworkLibraryPage> myNetworkLibraryPage;
	std::vector<shared_ptr<ZLOption> > myTemporaryOptions;

friend class OptionsApplyRunnable;
};

inline ZLOptionsDialog &OptionsDialog::dialog() { return *myDialog; }

#endif /* __OPTIONSDIALOG_H__ */
