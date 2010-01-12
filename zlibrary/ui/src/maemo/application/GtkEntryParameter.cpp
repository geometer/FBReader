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

#include <gtk/gtkmenutoolbutton.h>
#include <gtk/gtkentry.h>

#include "ZLGtkApplicationWindow.h"

#include "../../gtk/util/ZLGtkKeyUtil.h"
#include "../../gtk/util/ZLGtkSignalUtil.h"

void ZLGtkApplicationWindow::GtkEntryParameter::onKeyPressed(const std::string &keyName) {
	if (keyName == "<Enter>") {
		myWindow.application().doAction(myItem.actionId());
		myWindow.setFocusToMainWidget();
	} else if (keyName == "<Escape>") {
		restoreOldValue();
		myWindow.setFocusToMainWidget();
	}
}

static bool onKeyPressed(GtkEntry*, GdkEventKey *event, ZLGtkApplicationWindow::GtkEntryParameter *parameter) {
	parameter->onKeyPressed(ZLGtkKeyUtil::keyName(event));
	return false;
}

void ZLGtkApplicationWindow::GtkEntryParameter::onValueChanged() {
	GtkComboBox *comboBox = GTK_COMBO_BOX(myWidget);
	const int index = gtk_combo_box_get_active(comboBox);
	const int size = gtk_tree_model_iter_n_children(gtk_combo_box_get_model(comboBox), 0);
	if ((index >= 0) && (index < size)) {
		const char *text = gtk_combo_box_get_active_text(comboBox);
		if (text != 0) {
			std::string sText = text;
			if (!sText.empty()) {
				myWindow.application().doAction(myItem.actionId());
				//myWindow.setFocusToMainWidget();
			}
		}
	}
}

static bool onValueChanged(GtkComboBox*, ZLGtkApplicationWindow::GtkEntryParameter *parameter) {
	parameter->onValueChanged();
	return true;
}

ZLGtkApplicationWindow::GtkEntryParameter::GtkEntryParameter(ZLGtkApplicationWindow &window, const ZLToolbar::ParameterItem &item) : myWindow(window), myItem(item) {
	if (item.type() == ZLToolbar::Item::COMBO_BOX) {
		myWidget = gtk_combo_box_entry_new_text();
		myEntry = GTK_ENTRY(GTK_BIN(myWidget)->child);
		ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myEntry), "changed", GTK_SIGNAL_FUNC(::onValueChanged), this);
	} else {
		myWidget = gtk_entry_new();
		myEntry = GTK_ENTRY(myWidget);
	}
	gtk_entry_set_alignment(myEntry, 0.5);
	gtk_entry_set_width_chars(myEntry, item.maxWidth());
	gtk_entry_set_max_length(myEntry, item.maxWidth());
	if (item.symbolSet() == ZLToolbar::ParameterItem::SET_DIGITS) {
		hildon_gtk_entry_set_input_mode(myEntry, HILDON_GTK_INPUT_MODE_NUMERIC);
	} else {
		hildon_gtk_entry_set_input_mode(myEntry, HILDON_GTK_INPUT_MODE_FULL);
	}
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myEntry), "key_press_event", GTK_SIGNAL_FUNC(::onKeyPressed), this);
}

GtkToolItem *ZLGtkApplicationWindow::GtkEntryParameter::createToolItem() {
	GtkToolItem *toolItem = gtk_tool_item_new();
	gtk_container_add(GTK_CONTAINER(toolItem), myWidget);
	return toolItem;
}

void ZLGtkApplicationWindow::GtkEntryParameter::internalSetValue(const std::string &value) {
	gtk_entry_set_text(myEntry, value.c_str());
}

std::string ZLGtkApplicationWindow::GtkEntryParameter::internalValue() const {
	return gtk_entry_get_text(myEntry);
}

void ZLGtkApplicationWindow::GtkEntryParameter::setValueList(const std::vector<std::string> &values) {
	if (myItem.type() == ZLToolbar::Item::TEXT_FIELD) {
		return;
	}

	GtkComboBox *comboBox = GTK_COMBO_BOX(myWidget);

	int size = gtk_tree_model_iter_n_children(gtk_combo_box_get_model(comboBox), 0);
	for (; size > 0; --size) {
		gtk_combo_box_remove_text(comboBox, 0);
	}

	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ++it) {
		gtk_combo_box_append_text(comboBox, it->c_str());
	}
}
