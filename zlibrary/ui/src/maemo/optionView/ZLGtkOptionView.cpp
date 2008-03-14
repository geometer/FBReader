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

#include "../../gtk/util/ZLGtkKeyUtil.h"

#include <ZLResource.h>

#include "ZLGtkOptionView.h"
#include "ZLGtkOptionViewHolder.h"
#include "../dialogs/ZLGtkDialogManager.h"
#include "../dialogs/ZLGtkUtil.h"

static GtkLabel *gtkLabel(const std::string &text) {
	GtkLabel *label = GTK_LABEL(gtk_label_new(gtkString(text).c_str()));
	gtk_label_set_justify(label, GTK_JUSTIFY_RIGHT);
	return label;
}

void ZLGtkOptionView::_onValueChanged(GtkWidget*, gpointer self) {
	((ZLGtkOptionView*)self)->onValueChanged();
}

void BooleanOptionView::_createItem() {
	myCheckBox = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(gtkString(name()).c_str()));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(myCheckBox), ((ZLBooleanOptionEntry&)*myOption).initialState());
	g_signal_connect(GTK_WIDGET(myCheckBox), "toggled", G_CALLBACK(_onValueChanged), this);
	myHolder.attachWidget(*this, GTK_WIDGET(myCheckBox));
}

void BooleanOptionView::_show() {
	gtk_widget_show(GTK_WIDGET(myCheckBox));
}

void BooleanOptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myCheckBox));
}

void BooleanOptionView::_setActive(bool active) {
	gtk_widget_set_sensitive(GTK_WIDGET(myCheckBox), active);
}

void BooleanOptionView::_onAccept() const {
	((ZLBooleanOptionEntry&)*myOption).onAccept(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(myCheckBox)));
}

void BooleanOptionView::onValueChanged() {
	((ZLBooleanOptionEntry&)*myOption).onStateChanged(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(myCheckBox)));
}

ZLBoolean3 Boolean3OptionView::stateByIndex(int index) {
	switch (index) {
		case 0:
			return B3_TRUE;
		case 1:
			return B3_FALSE;
		default:
			return B3_UNDEFINED;
	}
}

void Boolean3OptionView::_createItem() {
	myLabel = gtkLabel(name());
	myComboBox = GTK_COMBO_BOX(gtk_combo_box_new_text());

	g_signal_connect(GTK_WIDGET(myComboBox), "changed", G_CALLBACK(_onValueChanged), this);

	myHolder.attachWidgets(*this, GTK_WIDGET(myLabel), 1, GTK_WIDGET(myComboBox), 1);

	const ZLResource &resource = ZLResource::resource("boolean3");
	gtk_combo_box_append_text(myComboBox, resource["on"].value().c_str());
	gtk_combo_box_append_text(myComboBox, resource["off"].value().c_str());
	gtk_combo_box_append_text(myComboBox, resource["unchanged"].value().c_str());

	reset();
}

void Boolean3OptionView::_show() {
	gtk_widget_show(GTK_WIDGET(myLabel));
	gtk_widget_show(GTK_WIDGET(myComboBox));
}

void Boolean3OptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myLabel));
	gtk_widget_hide(GTK_WIDGET(myComboBox));
}

void Boolean3OptionView::_setActive(bool active) {
	gtk_widget_set_sensitive(GTK_WIDGET(myComboBox), active);
}

void Boolean3OptionView::_onAccept() const {
	((ZLBoolean3OptionEntry&)*myOption).onAccept(stateByIndex(gtk_combo_box_get_active(myComboBox)));
}

void Boolean3OptionView::onValueChanged() {
	((ZLBoolean3OptionEntry&)*myOption).onStateChanged(stateByIndex(gtk_combo_box_get_active(myComboBox)));
}

void Boolean3OptionView::reset() {
	if (myComboBox == 0) {
		return;
	}

	int index;
	switch (((ZLBoolean3OptionEntry&)*myOption).initialState()) {
		case B3_TRUE:
			index = 0;
			break;
		case B3_FALSE:
			index = 1;
			break;
		default:
			index = 2;
			break;
	}

	gtk_combo_box_set_active(myComboBox, index);
}

void ChoiceOptionView::_createItem() {
	myFrame = GTK_FRAME(gtk_frame_new(name().c_str()));
	myVBox = GTK_BOX(gtk_vbox_new(true, 10));
	gtk_container_set_border_width(GTK_CONTAINER(myVBox), 5);

	int num = ((ZLChoiceOptionEntry&)*myOption).choiceNumber();
	myButtons = new GtkRadioButton* [num];
	GSList *group = 0;
	for (int i = 0; i < num; ++i) {
		myButtons[i] = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label(group, ((ZLChoiceOptionEntry&)*myOption).text(i).c_str()));
		group = gtk_radio_button_get_group(myButtons[i]);
		gtk_box_pack_start (myVBox, GTK_WIDGET(myButtons[i]), true, true, 0);
	}
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(myButtons[((ZLChoiceOptionEntry&)*myOption).initialCheckedIndex()]), true);
	gtk_container_add(GTK_CONTAINER(myFrame), GTK_WIDGET(myVBox));
	myHolder.attachWidget(*this, GTK_WIDGET(myFrame));
}

void ChoiceOptionView::_show() {
	gtk_widget_show(GTK_WIDGET(myFrame));
	gtk_widget_show(GTK_WIDGET(myVBox));
	for (int i = 0; i < ((ZLChoiceOptionEntry&)*myOption).choiceNumber(); ++i) {
		gtk_widget_show(GTK_WIDGET(myButtons[i]));
	}
}

void ChoiceOptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myFrame));
	gtk_widget_hide(GTK_WIDGET(myVBox));
	for (int i = 0; i < ((ZLChoiceOptionEntry&)*myOption).choiceNumber(); ++i) {
		gtk_widget_hide(GTK_WIDGET(myButtons[i]));
	}
}

void ChoiceOptionView::_setActive(bool active) {
	gtk_widget_set_sensitive(GTK_WIDGET(myFrame), active);
	for (int i = 0; i < ((ZLChoiceOptionEntry&)*myOption).choiceNumber(); ++i) {
		gtk_widget_set_sensitive(GTK_WIDGET(myButtons[i]), active);
	}
}

void ChoiceOptionView::_onAccept() const {
	for (int i = 0; i < ((ZLChoiceOptionEntry&)*myOption).choiceNumber(); ++i) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(myButtons[i]))) {
			((ZLChoiceOptionEntry&)*myOption).onAccept(i);
			return;
		}
	}
}

void ComboOptionView::_createItem() {
	const ZLComboOptionEntry &comboOptionEntry = (ZLComboOptionEntry&)*myOption;
	if (!name().empty()) {
		myLabel = gtkLabel(name());
	}
	myComboBox = comboOptionEntry.isEditable() ?
		GTK_COMBO_BOX(gtk_combo_box_entry_new_text()) : 
		GTK_COMBO_BOX(gtk_combo_box_new_text());

	g_signal_connect(GTK_WIDGET(myComboBox), "changed", G_CALLBACK(_onValueChanged), this);

	if (myLabel != 0) {
		myHolder.attachWidgets(*this, GTK_WIDGET(myLabel), 1, GTK_WIDGET(myComboBox), 1);
	} else {
		myHolder.attachWidget(*this, GTK_WIDGET(myComboBox));
	}

	reset();
}

void ComboOptionView::_show() {
	if (myLabel != 0) {
		gtk_widget_show(GTK_WIDGET(myLabel));
	}
	gtk_widget_show(GTK_WIDGET(myComboBox));
}

void ComboOptionView::_hide() {
	if (myLabel != 0) {
		gtk_widget_hide(GTK_WIDGET(myLabel));
	}
	gtk_widget_hide(GTK_WIDGET(myComboBox));
}

void ComboOptionView::_setActive(bool active) {
	gtk_widget_set_sensitive(GTK_WIDGET(myComboBox), active);
}

void ComboOptionView::_onAccept() const {
	((ZLComboOptionEntry&)*myOption).onAccept(gtk_combo_box_get_active_text(myComboBox));
}

void ComboOptionView::reset() {
	if (myComboBox == 0) {
		return;
	}

	for (; myListSize > 0; --myListSize) {
		gtk_combo_box_remove_text(myComboBox, 0);
	}
	const ZLComboOptionEntry &comboOptionEntry = (ZLComboOptionEntry&)*myOption;
	const std::vector<std::string> &values = comboOptionEntry.values();
	const std::string &initial = comboOptionEntry.initialValue();
	myListSize = values.size();
	mySelectedIndex = -1;
	int index = 0;

	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++index) {
		if (*it == initial) {
			mySelectedIndex = index;
		}
		gtk_combo_box_append_text(myComboBox, it->c_str());
	}

	if (mySelectedIndex >= 0) {
		gtk_combo_box_set_active(myComboBox, mySelectedIndex);
	}
}

void ComboOptionView::onValueChanged() {
	int index = gtk_combo_box_get_active(myComboBox);
	ZLComboOptionEntry& o = (ZLComboOptionEntry&)*myOption;
	if ((index != mySelectedIndex) && (index >= 0) && (index < (int)o.values().size())) {
		mySelectedIndex = index;
  	o.onValueSelected(mySelectedIndex);
	} else if (o.useOnValueEdited()) {
		std::string text = gtk_combo_box_get_active_text(myComboBox);
  	o.onValueEdited(text);
	}
}

void SpinOptionView::_createItem() {
	ZLSpinOptionEntry &option = (ZLSpinOptionEntry&)*myOption;

	myLabel = gtkLabel(name());

	mySpinBox = HILDON_NUMBER_EDITOR(hildon_number_editor_new(option.minValue(), option.maxValue()));
	hildon_number_editor_set_value(mySpinBox, option.initialValue());

	myHolder.attachWidgets(*this, GTK_WIDGET(myLabel), 1, GTK_WIDGET(mySpinBox), 1);
}

void SpinOptionView::_show() {
	gtk_widget_show(GTK_WIDGET(myLabel));
	gtk_widget_show(GTK_WIDGET(mySpinBox));
}

void SpinOptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myLabel));
	gtk_widget_hide(GTK_WIDGET(mySpinBox));
}

void SpinOptionView::_onAccept() const {
	((ZLSpinOptionEntry&)*myOption).onAccept(hildon_number_editor_get_value(mySpinBox));
}

void StringOptionView::_createItem() {
	myLineEdit = GTK_ENTRY(gtk_entry_new());
	g_signal_connect(myLineEdit, "changed", G_CALLBACK(_onValueChanged), this);

	if (!name().empty()) {
		myLabel = gtkLabel(name());
		myHolder.attachWidgets(*this, GTK_WIDGET(myLabel), 1, GTK_WIDGET(myLineEdit), 2);
	} else {
		myLabel = 0;
		myHolder.attachWidget(*this, GTK_WIDGET(myLineEdit));
	}

	reset();
}

void StringOptionView::reset() {
	if (myLineEdit == 0) {
		return;
	}

	gtk_entry_set_text(myLineEdit, ((ZLStringOptionEntry&)*myOption).initialValue().c_str());
}

void StringOptionView::onValueChanged() {
	ZLStringOptionEntry &o = (ZLStringOptionEntry&)*myOption;
	if (o.useOnValueEdited()) {
		o.onValueEdited(gtk_entry_get_text(myLineEdit));
	}
}

void StringOptionView::_show() {
	if (myLabel != 0) {
		gtk_widget_show(GTK_WIDGET(myLabel));
	}
	gtk_widget_show(GTK_WIDGET(myLineEdit));
}

void StringOptionView::_hide() {
	if (myLabel != 0) {
		gtk_widget_hide(GTK_WIDGET(myLabel));
	}
	gtk_widget_hide(GTK_WIDGET(myLineEdit));
}

void StringOptionView::_setActive(bool active) {
	gtk_entry_set_editable(myLineEdit, active);
}

void StringOptionView::_onAccept() const {
	((ZLStringOptionEntry&)*myOption).onAccept(gtk_entry_get_text(myLineEdit));
}

void MultilineOptionView::_createItem() {
	myTextBuffer = GTK_TEXT_BUFFER(gtk_text_buffer_new(0));
	g_signal_connect(myTextBuffer, "changed", G_CALLBACK(_onValueChanged), this);
	myTextView = GTK_TEXT_VIEW(gtk_text_view_new_with_buffer(myTextBuffer));
	gtk_text_view_set_wrap_mode(myTextView, GTK_WRAP_WORD);
	myHolder.attachWidget(*this, GTK_WIDGET(myTextView));
	reset();
}

void MultilineOptionView::reset() {
	if (myTextBuffer == 0) {
		return;
	}

	const std::string &value = ((ZLMultilineOptionEntry&)*myOption).initialValue();
	gtk_text_buffer_set_text(myTextBuffer, value.data(), value.length());
}

void MultilineOptionView::onValueChanged() {
	ZLMultilineOptionEntry &o = (ZLMultilineOptionEntry&)*myOption;
	if (o.useOnValueEdited()) {
		GtkTextIter start, end;
		gtk_text_buffer_get_bounds(myTextBuffer, &start, &end);
		gchar *value = gtk_text_buffer_get_text(myTextBuffer, &start, &end, true);
		o.onValueEdited(value);
		g_free(value);
	}
}

void MultilineOptionView::_show() {
	gtk_widget_show(GTK_WIDGET(myTextView));
}

void MultilineOptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myTextView));
}

void MultilineOptionView::_setActive(bool active) {
	gtk_text_view_set_editable(myTextView, active);
}

void MultilineOptionView::_onAccept() const {
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(myTextBuffer, &start, &end);
	gchar *value = gtk_text_buffer_get_text(myTextBuffer, &start, &end, true);
	((ZLMultilineOptionEntry&)*myOption).onAccept(value);
	g_free(value);
}

static const int COLOR_STEPS_NUMBER = 32;

static void attachColorLabel(GtkTable *table, const ZLResource &colorResource, int row) {
	gtk_table_attach(
		table,
		gtk_label_new(colorResource.value().c_str()),
		0, 1, row, row + 1,
		(GtkAttachOptions)(GTK_FILL | GTK_SHRINK), (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), 0, 0
	);
}

void ColorOptionView::_createItem() {
	myDrawingArea = gtk_drawing_area_new();

	gtk_widget_set_size_request(GTK_WIDGET(myDrawingArea), 60, 20);
	myTable = GTK_TABLE(gtk_table_new(3, 4, false));

	gtk_table_attach(myTable, gtk_label_new(""), 0, 3, 0, 1, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

	const ZLResource &resource = ZLResource::resource(ZLDialogManager::COLOR_KEY);
	attachColorLabel(myTable, resource["red"], 1);
	attachColorLabel(myTable, resource["green"], 2);
	attachColorLabel(myTable, resource["blue"], 3);

	const ZLColor &color = ((ZLColorOptionEntry&)*myOption).color();

	myRSlider = HILDON_CONTROLBAR(hildon_controlbar_new());
	hildon_controlbar_set_range(myRSlider, 0, COLOR_STEPS_NUMBER);
	hildon_controlbar_set_value(myRSlider, (int)color.Red * COLOR_STEPS_NUMBER / 255);
	g_signal_connect(G_OBJECT(myRSlider), "value-changed", G_CALLBACK(_onSliderMove), this);

	myGSlider = HILDON_CONTROLBAR(hildon_controlbar_new());
	hildon_controlbar_set_range(myGSlider, 0, COLOR_STEPS_NUMBER);
	hildon_controlbar_set_value(myGSlider, (int)color.Green * COLOR_STEPS_NUMBER / 255);
	g_signal_connect(G_OBJECT(myGSlider), "value-changed", G_CALLBACK(_onSliderMove), this);

	myBSlider = HILDON_CONTROLBAR(hildon_controlbar_new());
	hildon_controlbar_set_range(myBSlider, 0, COLOR_STEPS_NUMBER);
	hildon_controlbar_set_value(myBSlider, (int)color.Blue * COLOR_STEPS_NUMBER / 255);
	g_signal_connect(G_OBJECT(myBSlider), "value-changed", G_CALLBACK(_onSliderMove), this);

	gtk_table_attach_defaults(myTable, GTK_WIDGET(myRSlider), 1, 2, 1, 2);
	gtk_table_attach_defaults(myTable, GTK_WIDGET(myGSlider), 1, 2, 2, 3);
	gtk_table_attach_defaults(myTable, GTK_WIDGET(myBSlider), 1, 2, 3, 4);

	GtkWidget *frame = gtk_frame_new(0);

	gtk_container_add(GTK_CONTAINER(frame), myDrawingArea);
	myColor.red = color.Red * 65535 / 255;
	myColor.blue = color.Blue * 65535 / 255;
	myColor.green = color.Green * 65535 / 255;
	gtk_widget_modify_bg(myDrawingArea, GTK_STATE_NORMAL, &myColor);

	gtk_table_attach(myTable, frame, 2, 3, 1, 4, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

	gtk_table_set_col_spacings(myTable, 8);
	gtk_table_set_row_spacings(myTable, 2);

	gtk_widget_show_all(GTK_WIDGET(myTable));

	myHolder.attachWidget(*this, GTK_WIDGET(myTable));
}

void ColorOptionView::reset() {
	if (myDrawingArea == 0) {
		return;
	}

	ZLColorOptionEntry &colorEntry = (ZLColorOptionEntry&)*myOption;

	colorEntry.onReset(ZLColor(
		myColor.red * 255 / 65535,
		myColor.green * 255 / 65535,
		myColor.blue * 255 / 65535
	));

	const ZLColor &color = colorEntry.color();

	hildon_controlbar_set_value(myRSlider, (int)color.Red * COLOR_STEPS_NUMBER / 255);
	hildon_controlbar_set_value(myGSlider, (int)color.Green * COLOR_STEPS_NUMBER / 255);
	hildon_controlbar_set_value(myBSlider, (int)color.Blue * COLOR_STEPS_NUMBER / 255);

	myColor.red = color.Red * 65535 / 255;
	myColor.blue = color.Blue * 65535 / 255;
	myColor.green = color.Green * 65535 / 255;

	gtk_widget_modify_bg(myDrawingArea, GTK_STATE_NORMAL, &myColor);
}


void ColorOptionView::_show() {
	gtk_widget_show(GTK_WIDGET(myTable));
}

void ColorOptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myTable));
}

void ColorOptionView::_onSliderMove(GtkRange *, gpointer self) {
	((ColorOptionView *)self)->onSliderMove();
}

void ColorOptionView::onSliderMove() {
	myColor.red = hildon_controlbar_get_value(myRSlider) * 65535 / COLOR_STEPS_NUMBER;
	myColor.green = hildon_controlbar_get_value(myGSlider) * 65535 / COLOR_STEPS_NUMBER;
	myColor.blue = hildon_controlbar_get_value(myBSlider) * 65535 / COLOR_STEPS_NUMBER;

	gtk_widget_modify_bg(myDrawingArea, GTK_STATE_NORMAL, &myColor);
}

void ColorOptionView::_onAccept() const {
	((ZLColorOptionEntry&)*myOption).onAccept(ZLColor(
		myColor.red * 255 / 65535,
		myColor.green * 255 / 65535,
		myColor.blue * 255 / 65535
	));
}

static bool key_view_focus_in_event(GtkWidget *entry, GdkEventFocus*, gpointer) {
	gdk_keyboard_grab(entry->window, true, GDK_CURRENT_TIME);
	((ZLGtkDialogManager&)ZLGtkDialogManager::instance()).grabKeyboard(true);
	return true;
}

static bool key_view_focus_out_event(GtkWidget*, GdkEventFocus*, gpointer) {
	((ZLGtkDialogManager&)ZLGtkDialogManager::instance()).grabKeyboard(false);
	gdk_keyboard_ungrab(GDK_CURRENT_TIME);
	return false;
}

static bool key_view_key_press_event(GtkWidget *entry, GdkEventKey *event, gpointer data) {
	gtk_entry_set_text(GTK_ENTRY(entry), ZLGtkKeyUtil::keyName(event).c_str());
	((KeyOptionView*)data)->setKey(ZLGtkKeyUtil::keyName(event));
	return true;
}

void KeyOptionView::_createItem() {
	myKeyEntry = GTK_ENTRY(gtk_entry_new());
	gtk_signal_connect(GTK_OBJECT(myKeyEntry), "focus_in_event", G_CALLBACK(key_view_focus_in_event), 0);
	gtk_signal_connect(GTK_OBJECT(myKeyEntry), "focus_out_event", G_CALLBACK(key_view_focus_out_event), 0);
	gtk_signal_connect(GTK_OBJECT(myKeyEntry), "key_press_event", G_CALLBACK(key_view_key_press_event), this);
	key_view_focus_out_event(GTK_WIDGET(myKeyEntry), 0, 0);

	myLabel = GTK_LABEL(gtkLabel(ZLResource::resource("keyOptionView")["actionFor"].value()));

	myComboBox = GTK_COMBO_BOX(gtk_combo_box_new_text());
	const std::vector<std::string> &actions = ((ZLKeyOptionEntry&)*myOption).actionNames();
	for (std::vector<std::string>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
		gtk_combo_box_append_text(myComboBox, it->c_str());
	}

	myTable = GTK_TABLE(gtk_table_new(2, 2, false));
	gtk_table_set_col_spacings(myTable, 5);
	gtk_table_set_row_spacings(myTable, 5);
	gtk_table_attach_defaults(myTable, GTK_WIDGET(myLabel), 0, 1, 0, 1);
	gtk_table_attach_defaults(myTable, GTK_WIDGET(myKeyEntry), 1, 2, 0, 1);
	gtk_table_attach_defaults(myTable, GTK_WIDGET(myComboBox), 0, 2, 1, 2);
	g_signal_connect(GTK_WIDGET(myComboBox), "changed", G_CALLBACK(_onValueChanged), this);

	myHolder.attachWidget(*this, GTK_WIDGET(myTable));
}

void KeyOptionView::onValueChanged() {
	if (!myCurrentKey.empty()) {
		((ZLKeyOptionEntry&)*myOption).onValueChanged(
			myCurrentKey,
			gtk_combo_box_get_active(myComboBox)
		);
	}
}

void KeyOptionView::setKey(const std::string &key) {
	myCurrentKey = key;
	if (!key.empty()) {
		gtk_combo_box_set_active(myComboBox, ((ZLKeyOptionEntry&)*myOption).actionIndex(key));
		gtk_widget_show(GTK_WIDGET(myComboBox));
	}
	((ZLKeyOptionEntry&)*myOption).onKeySelected(myCurrentKey);
}

void KeyOptionView::reset() {
	if (myTable == 0) {
		return;
	}
	myCurrentKey.erase();
	gtk_entry_set_text(myKeyEntry, "");
	((ZLKeyOptionEntry&)*myOption).onKeySelected(myCurrentKey);
	gtk_widget_hide(GTK_WIDGET(myComboBox));
}

void KeyOptionView::_show() {
	gtk_widget_show(GTK_WIDGET(myTable));
	gtk_widget_show(GTK_WIDGET(myKeyEntry));
	gtk_widget_show(GTK_WIDGET(myLabel));
	if (!myCurrentKey.empty()) {
		gtk_widget_show(GTK_WIDGET(myComboBox));
	} else {
		gtk_widget_hide(GTK_WIDGET(myComboBox));
	}
}

void KeyOptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myTable));
	myCurrentKey.erase();
	gtk_entry_set_text(myKeyEntry, "");
	((ZLKeyOptionEntry&)*myOption).onKeySelected(myCurrentKey);
}

void KeyOptionView::_onAccept() const {
	((ZLKeyOptionEntry&)*myOption).onAccept();
}
