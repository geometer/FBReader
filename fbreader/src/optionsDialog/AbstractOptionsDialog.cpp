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

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>

#include "AbstractOptionsDialog.h"

#include "../fbreader/FBReader.h"


class OptionsApplyRunnable : public ZLRunnable {

public:
	void run();
};

void OptionsApplyRunnable::run() {
	FBReader &fbreader = FBReader::Instance();
	fbreader.grabAllKeys(fbreader.KeyboardControlOption.value());
	fbreader.clearTextCaches();
	fbreader.refreshWindow();
}


AbstractOptionsDialog::AbstractOptionsDialog(const ZLResourceKey &key, bool showApplyButton) {
	myDialog = ZLDialogManager::Instance().createOptionsDialog(key, new OptionsApplyRunnable(), showApplyButton);
}

void AbstractOptionsDialog::storeTemporaryOption(ZLOption *option) {
	myTemporaryOptions.push_back(option);
}
