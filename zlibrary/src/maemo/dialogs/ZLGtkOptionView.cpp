/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <gtk/gtk.h>

#include <hildon-number-editor.h>

#include "../../gtk/util/ZLGtkKeyUtil.h"

#include "ZLGtkOptionView.h"
#include "ZLGtkDialogContent.h"
#include "ZLGtkDialogManager.h"
#include "ZLGtkUtil.h"

static GtkWidget *labelWithMyParams(const char *text) {
	GtkWidget *label = gtk_label_new(text);

	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);

	return label;
}

void ZLGtkOptionView::_onValueChanged(GtkWidget*, gpointer self) {
	((ZLGtkOptionView*)self)->onValueChanged();
}

void BooleanOptionView::_createItem() {
	/*
	if (ZLApplication::isKeyboardPresented()) {
		myCheckBox = gtk_check_button_new_with_mnemonic(gtkString(myOption->name(), true).c_str());
	} else {
		myCheckBox = gtk_check_button_new_with_label(gtkString(myOption->name(), false).c_str());
	}
	*/
	myCheckBox = gtk_check_button_new_with_label(gtkString(myOption->name()).c_str());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(myCheckBox), ((ZLBooleanOptionEntry*)myOption)->initialState());
	g_signal_connect(myCheckBox, "toggled", G_CALLBACK(_onValueChanged), this);
	myTab->addItem(myCheckBox, myRow, myFromColumn, myToColumn);
}

void BooleanOptionView::_show() {
	gtk_widget_show(myCheckBox);
}

void BooleanOptionView::_hide() {
	gtk_widget_hide(myCheckBox);
}

void BooleanOptionView::_onAccept() const {
	((ZLBooleanOptionEntry*)myOption)->onAccept(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(myCheckBox)));
}

void BooleanOptionView::onValueChanged() {
	((ZLBooleanOptionEntry*)myOption)->onStateChanged(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(myCheckBox)));
}

void ChoiceOptionView::_createItem() {
	myFrame = GTK_FRAME(gtk_frame_new(myOption->name().c_str()));
	myVBox = GTK_BOX(gtk_vbox_new(true, 10));
	gtk_container_set_border_width(GTK_CONTAINER(myVBox), 5);

	int num = ((ZLChoiceOptionEntry*)myOption)->choiceNumber();
	myButtons = new GtkRadioButton* [num];
	GSList *group = 0;
	for (int i = 0; i < num; ++i) {
		myButtons[i] = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label(group, ((ZLChoiceOptionEntry*)myOption)->text(i).c_str()));
		group = gtk_radio_button_get_group(myButtons[i]);
		gtk_box_pack_start (myVBox, GTK_WIDGET(myButtons[i]), true, true, 0);
	}
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(myButtons[((ZLChoiceOptionEntry*)myOption)->initialCheckedIndex()]), true);
	gtk_container_add(GTK_CONTAINER(myFrame), GTK_WIDGET(myVBox));
	myTab->addItem(GTK_WIDGET(myFrame), myRow, myFromColumn, myToColumn);
}

void ChoiceOptionView::_show() {
	gtk_widget_show(GTK_WIDGET(myFrame));
	gtk_widget_show(GTK_WIDGET(myVBox));
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); ++i) {
		gtk_widget_show(GTK_WIDGET(myButtons[i]));
	}
}

void ChoiceOptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myFrame));
	gtk_widget_hide(GTK_WIDGET(myVBox));
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); ++i) {
		gtk_widget_hide(GTK_WIDGET(myButtons[i]));
	}
}

void ChoiceOptionView::_onAccept() const {
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); ++i) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(myButtons[i]))) {
			((ZLChoiceOptionEntry*)myOption)->onAccept(i);
			return;
		}
	}
}

void ComboOptionView::_createItem() {
	const ZLComboOptionEntry &comboOptionEntry = *(ZLComboOptionEntry*)myOption;
	myLabel = labelWithMyParams(myOption->name().c_str());
	myComboBox = comboOptionEntry.isEditable() ?
		GTK_COMBO_BOX(gtk_combo_box_entry_new_text()) : 
		GTK_COMBO_BOX(gtk_combo_box_new_text());

	g_signal_connect(GTK_WIDGET(myComboBox), "changed", G_CALLBACK(_onValueChanged), this);

	int midColumn = (myFromColumn + myToColumn) / 2;
	myTab->addItem(myLabel, myRow, myFromColumn, midColumn);
	myTab->addItem(GTK_WIDGET(myComboBox), myRow, midColumn, myToColumn);

	reset();
}

void ComboOptionView::_show() {
	gtk_widget_show(myLabel);
	gtk_widget_show(GTK_WIDGET(myComboBox));
}

void ComboOptionView::_hide() {
	gtk_widget_hide(myLabel);
	gtk_widget_hide(GTK_WIDGET(myComboBox));
}

void ComboOptionView::_setActive(bool active) {
	gtk_widget_set_sensitive(GTK_WIDGET(myComboBox), active);
}

void ComboOptionView::_onAccept() const {
	((ZLComboOptionEntry*)myOption)->onAccept(gtk_combo_box_get_active_text(myComboBox));
}

void ComboOptionView::reset() {
	if (myComboBox == 0) {
		return;
	}

	for (; myListSize > 0; --myListSize) {
		gtk_combo_box_remove_text(myComboBox, 0);
	}
	const ZLComboOptionEntry &comboOptionEntry = *(ZLComboOptionEntry*)myOption;
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
	ZLComboOptionEntry& o = *(ZLComboOptionEntry*)myOption;
	if ((index != mySelectedIndex) && (index >= 0) && (index < (int)o.values().size())) {
		mySelectedIndex = index;
  	o.onValueSelected(mySelectedIndex);
	} else {
		std::string text = gtk_combo_box_get_active_text(myComboBox);
  	o.onValueEdited(text);
	}
}

void SpinOptionView::_createItem() {
	ZLSpinOptionEntry *tempo = (ZLSpinOptionEntry*)myOption;

	myLabel = labelWithMyParams(myOption->name().c_str());

	mySpinBox = hildon_number_editor_new(tempo->minValue(), tempo->maxValue());
	hildon_number_editor_set_value(HILDON_NUMBER_EDITOR(mySpinBox), tempo->initialValue());

	int midColumn = (myFromColumn + myToColumn)/2;

	myTab->addItem(myLabel, myRow, myFromColumn, midColumn);
	myTab->addItem(mySpinBox, myRow, midColumn, myToColumn);
}

void SpinOptionView::_show() {
	gtk_widget_show(myLabel);
	gtk_widget_show(mySpinBox);
}

void SpinOptionView::_hide() {
	gtk_widget_hide(myLabel);
	gtk_widget_hide(mySpinBox);
}

void SpinOptionView::_onAccept() const {
	((ZLSpinOptionEntry*)myOption)->onAccept(hildon_number_editor_get_value(HILDON_NUMBER_EDITOR(mySpinBox)));
}

void StringOptionView::_createItem() {
	myLineEdit = GTK_ENTRY(gtk_entry_new());
	g_signal_connect(myLineEdit, "changed", G_CALLBACK(_onValueChanged), this);

	if (!myOption->name().empty()) {
		myLabel = labelWithMyParams(myOption->name().c_str());
		int width = myToColumn - myFromColumn;
		myTab->addItem(myLabel, myRow, myFromColumn, myFromColumn + width / 4);
		myTab->addItem(GTK_WIDGET(myLineEdit), myRow, myFromColumn + width / 4, myToColumn);
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

	gtk_entry_set_text(myLineEdit, ((ZLStringOptionEntry*)myOption)->initialValue().c_str());
}

void StringOptionView::onValueChanged() {
	((ZLStringOptionEntry*)myOption)->onValueEdited(gtk_entry_get_text(myLineEdit));
}

void StringOptionView::_show() {
	if (myLabel != 0) {
		gtk_widget_show(myLabel);
	}
	gtk_widget_show(GTK_WIDGET(myLineEdit));
}

void StringOptionView::_hide() {
	if (myLabel != 0) {
		gtk_widget_hide(myLabel);
	}
	gtk_widget_hide(GTK_WIDGET(myLineEdit));
}

void StringOptionView::_setActive(bool active) {
	gtk_entry_set_editable(myLineEdit, active);
}

void StringOptionView::_onAccept() const {
	((ZLStringOptionEntry*)myOption)->onAccept(gtk_entry_get_text(myLineEdit));
}

static const int COLOR_STEPS_NUMBER = 32;

void ColorOptionView::_createItem() {
	myDrawingArea = gtk_drawing_area_new();

	gtk_widget_set_size_request(GTK_WIDGET(myDrawingArea), 60, 20);
	myWidget = gtk_table_new(3, 4, false);

	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new(""), 0, 3, 0, 1, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new("Red"), 0, 1, 1, 2, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new("Green"), 0, 1, 2, 3, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new("Blue"), 0, 1, 3, 4, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

	const ZLColor &color = ((ZLColorOptionEntry*)myOption)->color();

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

	gtk_table_attach_defaults(GTK_TABLE(myWidget), GTK_WIDGET(myRSlider), 1, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), GTK_WIDGET(myGSlider), 1, 2, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), GTK_WIDGET(myBSlider), 1, 2, 3, 4);

	GtkWidget *frame = gtk_frame_new(NULL);

	gtk_container_add(GTK_CONTAINER(frame), myDrawingArea);
	myColor.red = color.Red * 65535 / 255;
	myColor.blue = color.Blue * 65535 / 255;
	myColor.green = color.Green * 65535 / 255;
	gtk_widget_modify_bg(myDrawingArea, GTK_STATE_NORMAL, &myColor);

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

	ZLColorOptionEntry &colorEntry = *(ZLColorOptionEntry*)myOption;

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
	gtk_widget_show(myWidget);
}

void ColorOptionView::_hide() {
	gtk_widget_hide(myWidget);
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
	((ZLColorOptionEntry*)myOption)->onAccept(ZLColor(
		myColor.red * 255 / 65535,
		myColor.green * 255 / 65535,
		myColor.blue * 255 / 65535
	));
}

static void key_view_focus_in_event(GtkWidget *button, GdkEventFocus*, gpointer) {
	gtk_button_set_label(GTK_BUTTON(button), "Press key to set action");
	gdk_keyboard_grab(button->window, true, GDK_CURRENT_TIME);
	((ZLGtkDialogManager&)ZLGtkDialogManager::instance()).grabKeyboard(true);
}

static void key_view_focus_out_event(GtkWidget *button, GdkEventFocus*, gpointer) {
	gtk_button_set_label(GTK_BUTTON(button), "Press this button to select key");
	((ZLGtkDialogManager&)ZLGtkDialogManager::instance()).grabKeyboard(false);
	gdk_keyboard_ungrab(GDK_CURRENT_TIME);
}

static bool key_view_key_press_event(GtkWidget*, GdkEventKey *event, gpointer data) {
	((KeyOptionView*)data)->setKey(ZLGtkKeyUtil::keyName(event));
	gtk_widget_grab_focus(((KeyOptionView*)data)->comboBox());
	return true;
}

static void key_view_button_press_event(GtkWidget *button, GdkEventButton*, gpointer data) {
	((KeyOptionView*)data)->setKey("");
	gtk_widget_grab_focus(button);
}

void KeyOptionView::_createItem() {
	myKeyButton = gtk_button_new();
	gtk_signal_connect(GTK_OBJECT(myKeyButton), "focus_in_event", G_CALLBACK(key_view_focus_in_event), 0);
	gtk_signal_connect(GTK_OBJECT(myKeyButton), "focus_out_event", G_CALLBACK(key_view_focus_out_event), 0);
	gtk_signal_connect(GTK_OBJECT(myKeyButton), "key_press_event", G_CALLBACK(key_view_key_press_event), this);
	gtk_signal_connect(GTK_OBJECT(myKeyButton), "button_press_event", G_CALLBACK(key_view_button_press_event), this);
	key_view_focus_out_event(myKeyButton, 0, 0);

	myLabel = gtk_label_new("");

	myComboBox = GTK_COMBO_BOX(gtk_combo_box_new_text());
	const std::vector<std::string> &actions = ((ZLKeyOptionEntry*)myOption)->actionNames();
	for (std::vector<std::string>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
		gtk_combo_box_append_text(myComboBox, it->c_str());
	}

	myWidget = gtk_table_new(2, 2, false);
	gtk_table_set_col_spacings(GTK_TABLE(myWidget), 5);
	gtk_table_set_row_spacings(GTK_TABLE(myWidget), 5);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), myKeyButton, 0, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), myLabel, 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), GTK_WIDGET(myComboBox), 1, 2, 1, 2);
	g_signal_connect(GTK_WIDGET(myComboBox), "changed", G_CALLBACK(_onValueChanged), this);

	myTab->addItem(myWidget, myRow, myFromColumn, myToColumn);
}

void KeyOptionView::onValueChanged() {
	if (!myCurrentKey.empty()) {
		((ZLKeyOptionEntry*)myOption)->onValueChanged(
			myCurrentKey,
			gtk_combo_box_get_active(myComboBox)
		);
	}
}

void KeyOptionView::setKey(const std::string &key) {
	myCurrentKey = key;
	if (!key.empty()) {
		gtk_label_set_text(GTK_LABEL(myLabel), ("Action For " + key).c_str());
		gtk_widget_show(myLabel);
		gtk_combo_box_set_active(myComboBox, ((ZLKeyOptionEntry*)myOption)->actionIndex(key));
		gtk_widget_show(GTK_WIDGET(myComboBox));
	} else {
		gtk_widget_hide(myLabel);
		gtk_widget_hide(GTK_WIDGET(myComboBox));
	}
}

void KeyOptionView::reset() {
	if (myWidget == 0) {
		return;
	}
	myCurrentKey.erase();
	gtk_widget_hide(myLabel);
	gtk_widget_hide(GTK_WIDGET(myComboBox));
}

void KeyOptionView::_show() {
	gtk_widget_show(myWidget);
	gtk_widget_show(myKeyButton);
	if (!myCurrentKey.empty()) {
		gtk_widget_show(myLabel);
		gtk_widget_show(GTK_WIDGET(myComboBox));
	} else {
		gtk_widget_hide(myLabel);
		gtk_widget_hide(GTK_WIDGET(myComboBox));
	}
}

void KeyOptionView::_hide() {
	gtk_widget_hide(myWidget);
	myCurrentKey.erase();
}

void KeyOptionView::_onAccept() const {
	((ZLKeyOptionEntry*)myOption)->onAccept();
}

// vim:ts=2:sw=2:noet
