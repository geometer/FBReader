/*
 * FBReader -- electronic book reader
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

#ifndef __OPTIONSDIALOG_H__
#define __OPTIONSDIALOG_H__

#include <shared_ptr.h>

class FBReader;

class ZLOptionsDialog;
class OptionsPage;
class ScrollingOptionsPage;
class KeyBindingsPage;
class ConfigPage;
class ZLPaintContext;

class ZLBooleanOptionEntry;
class ZLSpinOptionEntry;

class OptionsDialog {

public:
	OptionsDialog(FBReader &fbreader, ZLPaintContext &context);

	ZLOptionsDialog &dialog();

private:
	shared_ptr<ZLOptionsDialog> myDialog;
	shared_ptr<ScrollingOptionsPage> myScrollingPage;
	shared_ptr<OptionsPage> myFormatPage;
	shared_ptr<OptionsPage> myStylePage;
	shared_ptr<KeyBindingsPage> myKeyBindingsPage;
	shared_ptr<ConfigPage> myConfigPage;
};

inline ZLOptionsDialog &OptionsDialog::dialog() { return *myDialog; }

#endif /* __OPTIONSDIALOG_H__ */
