/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <algorithm>

#include "../../abstract/dialogs/ZLOptionEntry.h"

#include "GtkOptionsDialog.h"

GtkOptionsDialog::GtkOptionsDialog(const char *caption) {
}

ZLOptionsDialogTab *GtkOptionsDialog::createTab(const std::string &name) {
	GtkOptionsDialogTab *tab = new GtkOptionsDialogTab();
	return tab;
}

std::string GtkOptionsDialog::selectedTabName() {
	return "";
}

void GtkOptionsDialog::selectTab(const std::string &name) {
}

int GtkOptionsDialog::run() {
	return 0;
}

/*
void GtkOptionsDialog::accept() {
}
*/

void GtkOptionsDialogTab::accept() {
}

void GtkOptionsDialogTab::close() {
}

GtkOptionsDialogTab::GtkOptionsDialogTab() {
}

GtkOptionsDialogTab::~GtkOptionsDialogTab() {
}

void GtkOptionsDialogTab::addOption(ZLOptionEntry *option) {
}

void GtkOptionsDialogTab::addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) {
}
