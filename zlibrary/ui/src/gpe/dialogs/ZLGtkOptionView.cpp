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

#include <ZLResource.h>
#include <ZLDialogManager.h>

#include "ZLGtkOptionView.h"
#include "ZLGtkDialogContent.h"
#include "ZLGtkDialogManager.h"
#include "ZLGtkUtil.h"
#include "../../gtk/util/ZLGtkKeyUtil.h"

static GtkLabel *gtkLabel(const std::string &name) {
	GtkLabel *label = GTK_LABEL(gtk_label_new((gtkString(name) + ":").c_str()));
	gtk_label_set_justify(label, GTK_JUSTIFY_RIGHT);
	return label;
}

void ZLGtkOptionView::_onValueChanged(GtkWidget*, gpointer self) {
	((ZLGtkOptionView*)self)->onValueChanged();
}

void BooleanOptionView::_createItem() {
	myCheckBox = GTK_CHECK_BUTTON(gtk_check_button_new_with_mnemonic(gtkString(ZLOptionView::name()).c_str()));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(myCheckBox), ((ZLBooleanOptionEntry&)*myOption).initialState());
	g_signal_connect(GTK_WIDGET(myCheckBox), "toggled", G_CALLBACK(_onValueChanged), this);
	myTab->addItem(GTK_WIDGET(myCheckBox), myRow, myFromColumn, myToColumn);
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

void Boolean3OptionView::_onReleased(GtkButton *button, gpointer self) {
	Boolean3OptionView &view = *(Boolean3OptionView*)self;
	switch (view.myState) {
		case B3_TRUE:
			view.setState(B3_UNDEFINED);
			break;
		case B3_FALSE:
			view.setState(B3_TRUE);
			break;
		case B3_UNDEFINED:
			view.setState(B3_FALSE);
			break;
	}
	view.onValueChanged();
}

void Boolean3OptionView::setState(ZLBoolean3 state) {
	if (myState != state) {
		myState = state;
		bool active = false;
		bool inconsistent = false;
		switch (myState) {
			case B3_TRUE:
				active = true;
				break;
			case B3_FALSE:
				break;
			case B3_UNDEFINED:
				inconsistent = true;
				break;
		}
		gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(myCheckBox), inconsistent);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(myCheckBox), active);
	}
}

void Boolean3OptionView::_createItem() {
	myCheckBox = GTK_CHECK_BUTTON(gtk_check_button_new_with_mnemonic(gtkString(name()).c_str()));
	setState(((ZLBoolean3OptionEntry&)*myOption).initialState());
	g_signal_connect(GTK_WIDGET(myCheckBox), "released", G_CALLBACK(_onReleased), this);
	myTab->addItem(GTK_WIDGET(myCheckBox), myRow, myFromColumn, myToColumn);
}

void Boolean3OptionView::_show() {
	gtk_widget_show(GTK_WIDGET(myCheckBox));
}

void Boolean3OptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myCheckBox));
}

void Boolean3OptionView::_setActive(bool active) {
	gtk_widget_set_sensitive(GTK_WIDGET(myCheckBox), active);
}

void Boolean3OptionView::_onAccept() const {
	((ZLBoolean3OptionEntry&)*myOption).onAccept(myState);
}

void Boolean3OptionView::onValueChanged() {
	((ZLBoolean3OptionEntry&)*myOption).onStateChanged(myState);
}

void ChoiceOptionView::_createItem() {
	myFrame = GTK_FRAME(gtk_frame_new(ZLOptionView::name().c_str()));
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
	myTab->addItem(GTK_WIDGET(myFrame), myRow, myFromColumn, myToColumn);
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
	if (!ZLOptionView::name().empty()) {
		myLabel = gtkLabel(ZLOptionView::name());
	}
	myComboBox = comboOptionEntry.isEditable() ?
		GTK_COMBO_BOX(gtk_combo_box_entry_new_text()) : 
		GTK_COMBO_BOX(gtk_combo_box_new_text());

	g_signal_connect(GTK_WIDGET(myComboBox), "changed", G_CALLBACK(_onValueChanged), this);

	if (myLabel != 0) {
		int midColumn = (myFromColumn + myToColumn) / 2;
		myTab->addItem(GTK_WIDGET(myLabel), myRow, myFromColumn, midColumn);
		myTab->addItem(GTK_WIDGET(myComboBox), myRow, midColumn, myToColumn);
	} else {
		myTab->addItem(GTK_WIDGET(myComboBox), myRow, myFromColumn, myToColumn);
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

	myLabel = gtkLabel(ZLOptionView::name());

	GtkAdjustment *adj = (GtkAdjustment *)gtk_adjustment_new(option.initialValue(), option.minValue(), option.maxValue(), option.step(), option.step(), 0);
	mySpinBox = GTK_SPIN_BUTTON(gtk_spin_button_new(adj, 1, 0));

	int midColumn = (myFromColumn + myToColumn) / 2;

	myTab->addItem(GTK_WIDGET(myLabel), myRow, myFromColumn, midColumn);
	myTab->addItem(GTK_WIDGET(mySpinBox), myRow, midColumn, myToColumn);
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
	((ZLSpinOptionEntry&)*myOption).onAccept((int)gtk_spin_button_get_value(mySpinBox));
}

void StringOptionView::_createItem() {
	myLineEdit = GTK_ENTRY(gtk_entry_new());
	g_signal_connect(myLineEdit, "changed", G_CALLBACK(_onValueChanged), this);

	if (!ZLOptionView::name().empty()) {
		myLabel = gtkLabel(ZLOptionView::name());
		int midColumn = (myFromColumn + myToColumn) / 2;
		myTab->addItem(GTK_WIDGET(myLabel), myRow, myFromColumn, midColumn);
		myTab->addItem(GTK_WIDGET(myLineEdit), myRow, midColumn, myToColumn);
	} else {
		myLabel = 0;
		myTab->addItem(GTK_WIDGET(myLineEdit), myRow, myFromColumn, myToColumn);
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

void ColorOptionView::_createItem() {
	const ZLColor &color = ((ZLColorOptionEntry&)*myOption).color();

	myDrawingArea = gtk_drawing_area_new();

	gtk_widget_set_size_request(GTK_WIDGET(myDrawingArea), 60, 20);
	myWidget = gtk_table_new(3, 4, false);

	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new(""), 0, 3, 0, 1, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

	const ZLResource &resource = ZLResource::resource(ZLDialogManager::COLOR_KEY);
	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new(resource["red"].value().c_str()), 0, 1, 1, 2, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new(resource["green"].value().c_str()), 0, 1, 2, 3, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new(resource["blue"].value().c_str()), 0, 1, 3, 4, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

	myRSlider = gtk_hscale_new_with_range(0.0, 255.0, 1.0);
	gtk_scale_set_draw_value(GTK_SCALE(myRSlider), false);
	gtk_range_set_value(GTK_RANGE(myRSlider), color.Red);
	g_signal_connect(G_OBJECT(myRSlider), "value-changed", G_CALLBACK(_onSliderMove), this);

	myGSlider = gtk_hscale_new_with_range(0.0, 255.0, 1.0);
	gtk_scale_set_draw_value(GTK_SCALE(myGSlider), false);
	gtk_range_set_value(GTK_RANGE(myGSlider), color.Green);
	g_signal_connect(G_OBJECT(myGSlider), "value-changed", G_CALLBACK(_onSliderMove), this);

	myBSlider = gtk_hscale_new_with_range(0.0, 255.0, 1.0);
	gtk_scale_set_draw_value(GTK_SCALE(myBSlider), false);
	gtk_range_set_value(GTK_RANGE(myBSlider), color.Blue);
	g_signal_connect(G_OBJECT(myBSlider), "value-changed", G_CALLBACK(_onSliderMove), this);

	gtk_table_attach_defaults(GTK_TABLE(myWidget), myRSlider, 1, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), myGSlider, 1, 2, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), myBSlider, 1, 2, 3, 4);

	myColor.red = color.Red * 65535 / 255;
	myColor.blue = color.Blue * 65535 / 255;
	myColor.green = color.Green * 65535 / 255;

	gtk_widget_modify_bg(myDrawingArea, GTK_STATE_NORMAL, &myColor);

	GtkWidget *frame = gtk_frame_new(NULL);

	gtk_container_add(GTK_CONTAINER(frame), myDrawingArea);

	gtk_table_attach(GTK_TABLE(myWidget), frame, 2, 3, 1, 4, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

	gtk_table_set_col_spacings(GTK_TABLE(myWidget), 2);
	gtk_table_set_row_spacings(GTK_TABLE(myWidget), 2);

	gtk_widget_show_all(myWidget);

	myTab->addItem(myWidget, myRow, myFromColumn, myToColumn);
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

	gtk_range_set_value(GTK_RANGE(myRSlider), color.Red);
	gtk_range_set_value(GTK_RANGE(myGSlider), color.Green);
	gtk_range_set_value(GTK_RANGE(myBSlider), color.Blue);

	myColor.red = color.Red * 65535 / 255;
	myColor.blue = color.Blue * 65535 / 255;
	myColor.green = color.Green * 65535 / 255;

	gtk_widget_modify_bg(myDrawingArea, GTK_STATE_NORMAL, &myColor);
}

void ColorOptionView::_show() {
	gtk_widget_show(myWidget);
}

void ColorOptionView::_hide() {
	gtk_widget_hide(myWidget);
}

void ColorOptionView::_onSliderMove(GtkRange*, gpointer self) {
	((ColorOptionView*)self)->onSliderMove();
}

void ColorOptionView::onSliderMove() {
	myColor.red = (int)(gtk_range_get_value(GTK_RANGE(myRSlider)) * 65535 / 255);
	myColor.blue = (int)(gtk_range_get_value(GTK_RANGE(myBSlider)) * 65535 / 255);
	myColor.green = (int)(gtk_range_get_value(GTK_RANGE(myGSlider)) * 65535 / 255);

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
	return false;
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

	myTab->addItem(GTK_WIDGET(myTable), myRow, myFromColumn, myToColumn);
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
