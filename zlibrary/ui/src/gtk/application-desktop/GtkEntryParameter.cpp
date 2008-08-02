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

#include "ZLGtkApplicationWindow.h"
#include "gtkwidgettoolitem.h"

#include "../util/ZLGtkKeyUtil.h"
#include "../util/ZLGtkSignalUtil.h"

void ZLGtkApplicationWindow::GtkEntryParameter::onKeyPressed(const std::string &keyName) {
	if (keyName == "<Return>") {
		myWindow.application().doAction(myActionId);
		myWindow.setFocusToMainWidget();
	} else if (keyName == "<Esc>") {
		myWindow.setFocusToMainWidget();
	}
}

static bool onKeyPressed(GtkEntry*, GdkEventKey *event, ZLGtkApplicationWindow::GtkEntryParameter *parameter) {
	parameter->onKeyPressed(ZLGtkKeyUtil::keyName(event));
	return false;
}

ZLGtkApplicationWindow::GtkEntryParameter::GtkEntryParameter(ZLGtkApplicationWindow &window, const std::string &actionId, int maxWidth) : myWindow(window), myActionId(actionId) {
	myEntry = GTK_ENTRY(gtk_entry_new());
	gtk_entry_set_alignment(myEntry, 0.5);
	gtk_entry_set_width_chars(myEntry, maxWidth);
	gtk_entry_set_max_length(myEntry, maxWidth);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myEntry), "key_press_event", GTK_SIGNAL_FUNC(::onKeyPressed), this);
}

GtkToolItem *ZLGtkApplicationWindow::GtkEntryParameter::createToolItem() {
	return gtk_widget_tool_item_new(GTK_WIDGET(myEntry));
}

void ZLGtkApplicationWindow::GtkEntryParameter::internalSetValue(const std::string &value) {
	gtk_entry_set_text(myEntry, value.c_str());
}

std::string ZLGtkApplicationWindow::GtkEntryParameter::internalValue() const {
	return gtk_entry_get_text(myEntry);
}
