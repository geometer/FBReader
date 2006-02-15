/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <iostream>

#include <gtk/gtk.h>
#include <gtk/gtkcheckbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkoptionmenu.h>
#include <gtk/gtkradiomenuitem.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkspinbutton.h>
#include <gtk/gtkdrawingarea.h>
#include <gtk/gtkhscale.h>
#include <gtk/gtkframe.h>

#include <gtk/GtkKeyUtil.h>

#include "GtkOptionView.h"
#include "GtkOptionsDialog.h"
#include "GtkDialogManager.h"

static GtkWidget *labelWithMyParams(const char *text) {
	GtkWidget *label = gtk_label_new(text);

	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);

	return label;
}

void GtkOptionView::_onValueChange(GtkWidget*, gpointer self) {
	((GtkOptionView*)self)->onValueChange();
}

void BooleanOptionView::_createItem() {
	myCheckBox = gtk_check_button_new_with_label(myOption->name().c_str());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(myCheckBox), ((ZLBooleanOptionEntry*)myOption)->initialState());
	g_signal_connect(myCheckBox, "toggled", G_CALLBACK(_onValueChange), this);
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

void BooleanOptionView::onValueChange() {
	((ZLBooleanOptionEntry*)myOption)->onValueChange(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(myCheckBox)));
}


void ChoiceOptionView::_createItem() {
	myFrame = GTK_FRAME(gtk_frame_new(myOption->name().c_str()));
	myVBox = GTK_BOX(gtk_vbox_new(true, 10));
	gtk_container_set_border_width(GTK_CONTAINER(myVBox), 5);

	int num = ((ZLChoiceOptionEntry*)myOption)->choiceNumber();
	myButtons = new (GtkRadioButton*)[num];
	GSList *group = 0;
	for (int i = 0; i < num; i++) {
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
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); i++) {
		gtk_widget_show(GTK_WIDGET(myButtons[i]));
	}
}

void ChoiceOptionView::_hide() {
	gtk_widget_hide(GTK_WIDGET(myFrame));
	gtk_widget_hide(GTK_WIDGET(myVBox));
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); i++) {
		gtk_widget_hide(GTK_WIDGET(myButtons[i]));
	}
}

void ChoiceOptionView::_onAccept() const {
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); i++) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(myButtons[i]))) {
			((ZLChoiceOptionEntry*)myOption)->onAccept(i);
			return;
		}
	}
}

void ComboOptionView::_createItem() {
	myLabel = labelWithMyParams(myOption->name().c_str());
	myComboBox = gtk_option_menu_new();

	const std::vector<std::string> &values = ((ZLComboOptionEntry*)myOption)->values();
	const std::string &initial = ((ZLComboOptionEntry*)myOption)->initialValue();
	int selectedIndex = -1;
	int index = 0;
	GtkWidget *menu = gtk_menu_new();

	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++, index++) {
		GtkWidget *menuItem = gtk_menu_item_new_with_label(it->c_str());
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
		if (*it == initial) {
			selectedIndex = index;
			gtk_menu_set_active(GTK_MENU(menu), index);
		}
		gtk_widget_show(menuItem);
	}

	gtk_option_menu_set_menu(GTK_OPTION_MENU(myComboBox), menu);

	if (selectedIndex >= 0) {
		gtk_option_menu_set_history(GTK_OPTION_MENU(myComboBox), selectedIndex);
	}

	int midColumn = (myFromColumn + myToColumn)/2;

	myTab->addItem(myLabel, myRow, myFromColumn, midColumn);
	myTab->addItem(myComboBox, myRow, midColumn, myToColumn);

	g_signal_connect(myComboBox, "changed", G_CALLBACK(_onValueChange), this);
}

void ComboOptionView::_show() {
	gtk_widget_show(myLabel);
	gtk_widget_show(myComboBox);
}

void ComboOptionView::_hide() {
	gtk_widget_hide(myLabel);
	gtk_widget_hide(myComboBox);
}

void ComboOptionView::_setActive(bool active) {
	gtk_widget_set_sensitive(myComboBox, active);
}

void ComboOptionView::_onAccept() const {
	int index = gtk_option_menu_get_history(GTK_OPTION_MENU(myComboBox));
	ZLComboOptionEntry *o = (ZLComboOptionEntry*)myOption;
	if ((index >= 0) && (index < (int)o->values().size())) {
		((ZLComboOptionEntry*)myOption)->onAccept(o->values()[index]);
	}
}

void ComboOptionView::onValueChange() {
	int index = gtk_option_menu_get_history(GTK_OPTION_MENU(myComboBox));
	ZLComboOptionEntry *o = (ZLComboOptionEntry*)myOption;
	if ((index >= 0) && (index < (int)o->values().size())) {
		o->onValueChange(o->values()[index]);
	}
}

void SpinOptionView::_createItem() {
	ZLSpinOptionEntry *tempo = (ZLSpinOptionEntry*)myOption;

	myLabel = labelWithMyParams(myOption->name().c_str());

	GtkAdjustment *adj = (GtkAdjustment *)gtk_adjustment_new(tempo->initialValue(), tempo->minValue(), tempo->maxValue(), tempo->step(), tempo->step(), 0);
	mySpinBox = gtk_spin_button_new(adj, 1, 0);

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
	((ZLSpinOptionEntry*)myOption)->onAccept((int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(mySpinBox)));
}

void StringOptionView::_createItem() {
	myLabel = labelWithMyParams(myOption->name().c_str());
	myLineEdit = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(myLineEdit), ((ZLStringOptionEntry*)myOption)->initialValue().c_str());

	int midColumn = (myFromColumn + myToColumn)/2;

	myTab->addItem(myLabel, myRow, myFromColumn, midColumn);
	myTab->addItem(myLineEdit, myRow, midColumn, myToColumn);
}

void StringOptionView::_show() {
	gtk_widget_show(myLabel);
	gtk_widget_show(myLineEdit);
}

void StringOptionView::_hide() {
	gtk_widget_hide(myLabel);
	gtk_widget_hide(myLineEdit);
}

void StringOptionView::_setActive(bool active) {
	gtk_entry_set_editable(GTK_ENTRY(myLineEdit), active);
}

void StringOptionView::_onAccept() const {
	((ZLStringOptionEntry*)myOption)->onAccept(gtk_entry_get_text(GTK_ENTRY(myLineEdit)));
}

void ColorOptionView::_createItem() {
	const ZLColor &color = ((ZLColorOptionEntry*)myOption)->color();

	myDrawingArea = gtk_drawing_area_new();

	gtk_widget_set_size_request(GTK_WIDGET(myDrawingArea), 60, 20);
	myWidget = gtk_table_new(3, 4, false);

	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new(""), 0, 3, 0, 1, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new("Red"), 0, 1, 1, 2, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new("Green"), 0, 1, 2, 3, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new("Blue"), 0, 1, 3, 4, (GtkAttachOptions)(GTK_FILL|GTK_SHRINK), (GtkAttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

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
	myColor.red = (int)(gtk_range_get_value(GTK_RANGE(myRSlider)) * 65535 / 255);
	myColor.blue = (int)(gtk_range_get_value(GTK_RANGE(myBSlider)) * 65535 / 255);
	myColor.green = (int)(gtk_range_get_value(GTK_RANGE(myGSlider)) * 65535 / 255);

	gtk_widget_modify_bg(myDrawingArea, GTK_STATE_NORMAL, &myColor);
}

void ColorOptionView::_onAccept() const {
	((ZLColorOptionEntry*)myOption)->onAccept(ZLColor(myColor.red/256, myColor.green/256, myColor.blue/256));
}

static void handleKeyEvent(GtkWidget*, GdkEventKey *event, gpointer data) {
}

static void key_view_focus_in_event(GtkWidget *button, GdkEventFocus*, gpointer) {
	gtk_button_set_label(GTK_BUTTON(button), "Press key to set action");
	gdk_keyboard_grab(button->window, true, GDK_CURRENT_TIME);
	((GtkDialogManager&)GtkDialogManager::instance()).grabKeyboard(true);
}

static void key_view_focus_out_event(GtkWidget *button, GdkEventFocus*, gpointer) {
	gtk_button_set_label(GTK_BUTTON(button), "Press this button to select key");
	((GtkDialogManager&)GtkDialogManager::instance()).grabKeyboard(false);
	gdk_keyboard_ungrab(GDK_CURRENT_TIME);
}

static bool key_view_key_press_event(GtkWidget*, GdkEventKey *event, gpointer data) {
	((KeyOptionView*)data)->setKey(GtkKeyUtil::keyName(event));
	return true;
}

static void key_view_button_press_event(GtkWidget *button, GdkEventButton*, gpointer) {
	gtk_widget_grab_focus(button);
}

void KeyOptionView::_createItem() {
	myKeyButton = gtk_button_new();
	gtk_signal_connect(GTK_OBJECT(myKeyButton), "focus_in_event", G_CALLBACK(key_view_focus_in_event), 0);
	gtk_signal_connect(GTK_OBJECT(myKeyButton), "focus_out_event", G_CALLBACK(key_view_focus_out_event), 0);
	gtk_signal_connect(GTK_OBJECT(myKeyButton), "key_press_event", G_CALLBACK(key_view_key_press_event), this);
	gtk_signal_connect(GTK_OBJECT(myKeyButton), "button_press_event", G_CALLBACK(key_view_button_press_event), 0);
	key_view_focus_out_event(myKeyButton, 0, 0);

	myLabel = gtk_label_new("");

	myComboBox = gtk_option_menu_new();
	const std::vector<std::string> &actions = ((ZLKeyOptionEntry*)myOption)->actionNames();
	GtkWidget *menu = gtk_menu_new();
	for (std::vector<std::string>::const_iterator it = actions.begin(); it != actions.end(); it++) {
		GtkWidget *menuItem = gtk_menu_item_new_with_label(it->c_str());
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
		gtk_widget_show(menuItem);
	}
	gtk_option_menu_set_menu(GTK_OPTION_MENU(myComboBox), menu);

	myWidget = gtk_table_new(2, 2, false);
	gtk_table_set_col_spacings(GTK_TABLE(myWidget), 5);
	gtk_table_set_row_spacings(GTK_TABLE(myWidget), 5);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), myKeyButton, 0, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), myLabel, 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(myWidget), myComboBox, 1, 2, 1, 2);
	g_signal_connect(myComboBox, "changed", G_CALLBACK(_onValueChange), this);

	myTab->addItem(myWidget, myRow, myFromColumn, myToColumn);
}

void KeyOptionView::onValueChange() {
	if (!myCurrentKey.empty()) {
		((ZLKeyOptionEntry*)myOption)->onValueChange(
			myCurrentKey,
			gtk_option_menu_get_history(GTK_OPTION_MENU(myComboBox))
		);
	}
}

void KeyOptionView::setKey(const std::string &key) {
	if (!key.empty()) {
		myCurrentKey = key;
		gtk_label_set_text(GTK_LABEL(myLabel), ("Action For " + key).c_str());
		gtk_widget_show(myLabel);
		gtk_option_menu_set_history(GTK_OPTION_MENU(myComboBox), ((ZLKeyOptionEntry*)myOption)->actionIndex(key));
		gtk_widget_show(myComboBox);
	}
}

void KeyOptionView::_show() {
	gtk_widget_show(myWidget);
	gtk_widget_show(myKeyButton);
	if (!myCurrentKey.empty()) {
		gtk_widget_show(myLabel);
		gtk_widget_show(myComboBox);
	} else {
		gtk_widget_hide(myLabel);
		gtk_widget_hide(myComboBox);
	}
}

void KeyOptionView::_hide() {
	gtk_widget_hide(myWidget);
}

void KeyOptionView::_onAccept() const {
	((ZLKeyOptionEntry*)myOption)->onAccept();
}

// vim:ts=2:sw=2:noet
