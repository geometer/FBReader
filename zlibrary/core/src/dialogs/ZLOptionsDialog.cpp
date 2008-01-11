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

#include <ZLDialogManager.h>

#include "ZLOptionsDialog.h"

ZLOptionsDialog::ZLOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction) : TabOption(ZLCategoryKey::LOOK_AND_FEEL, resource.name(), "SelectedTab", ""), myResource(resource), myApplyAction(applyAction) {
}

ZLOptionsDialog::~ZLOptionsDialog() {
}

const std::string &ZLOptionsDialog::caption() const {
	return myResource[ZLDialogManager::DIALOG_TITLE].value();
}

const ZLResource &ZLOptionsDialog::tabResource(const ZLResourceKey &key) const {
	return myResource["tab"][key];
}

bool ZLOptionsDialog::run() {
	selectTab(ZLResourceKey(TabOption.value()));
	bool code = runInternal();
	if (code) {
		accept();
	}
	TabOption.setValue(selectedTabKey());
	return code;
}

void ZLOptionsDialog::accept() {
	for (std::vector<shared_ptr<ZLDialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		(*it)->accept();
	}
	if (!myApplyAction.isNull()) {
		myApplyAction->run();
	}
}
