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

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkoptionmenu.h>
#include <gtk/gtkradiomenuitem.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkspinbutton.h>
#include <gtk/gtkdrawingarea.h>
// #include <gtk/gtkcolorseldialog.h>
#include <gtk/gtkhscale.h>
#include <gtk/gtkframe.h>

#include "GtkOptionView.h"
#include "GtkOptionsDialog.h"

// FIXME: geometer did some work on arranging the controls, mss will fix it
// later when the functionality is really working

static GtkWidget *labelWithMyParams(const char *text) {
	GtkWidget *label = gtk_label_new(text);

	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);

	return label;
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

void BooleanOptionView::_onValueChange(GtkWidget *, gpointer self) {
	((BooleanOptionView*)self)->onValueChange();
}

void BooleanOptionView::onValueChange() {
	((ZLBooleanOptionEntry*)myOption)->onValueChange(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(myCheckBox)));
}


#if 0
void ChoiceOptionView::_createItem() {
	myFrame = GTK_FRAME(gtk_frame_new(myOption->name().c_str()));

	gtk_widget_show(GTK_WIDGET(myFrame));

	GtkWidget *group = NULL;

    myTab;
	QVBoxLayout *layout = new QVBoxLayout(myGroup, 12);
	layout->addSpacing(myGroup->fontMetrics().height());
	myButtons = new (QRadioButton*)[((ZLChoiceOptionEntry*)myOption)->choiceNumber()];
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); i++) {
		myButtons[i] = new QRadioButton((QButtonGroup*)layout->parent());
		myButtons[i]->setText(((ZLChoiceOptionEntry*)myOption)->text(i).c_str());
		layout->addWidget(myButtons[i]);
	}
	myButtons[((ZLChoiceOptionEntry*)myOption)->initialCheckedIndex()]->setChecked(true);
	myTab->addItem(myGroup, myRow, myFromColumn, myToColumn);
}

void ChoiceOptionView::_show() {
	myGroup->show();
}

void ChoiceOptionView::_hide() {
	myGroup->hide();
}

void ChoiceOptionView::_onAccept() const {
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); i++) {
		if (myButtons[i]->isChecked()) {
			((ZLChoiceOptionEntry*)myOption)->onAccept(i);
			return;
		}
	}
}
#endif

void ComboOptionView::_createItem() {
	myLabel = labelWithMyParams(myOption->name().c_str());
	myComboBox = gtk_option_menu_new();

	const std::vector<std::string> &values = ((ZLComboOptionEntry*)myOption)->values();
	const std::string &initial = ((ZLComboOptionEntry*)myOption)->initialValue();
	int selectedIndex = -1;
	int index = 0;
	GtkWidget *menu;

	menu = gtk_menu_new();

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

void ComboOptionView::_onValueChange(GtkWidget *, gpointer self) {
	((ComboOptionView *)self)->onValueChange();
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
//	myWidget = gtk_button_new();
//	gtk_container_add(GTK_CONTAINER(myWidget), myDrawingArea);
//	g_signal_connect(G_OBJECT(myWidget), "clicked", G_CALLBACK(_onChangeColor), this);
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

#if 0
void ColorOptionView::_onChangeColor(GtkWidget *, gpointer self) {
	((ColorOptionView *)self)->onChangeColor();
}

void ColorOptionView::onChangeColor() {
	if (myColorSelectionDialog == NULL)
		myColorSelectionDialog = gtk_color_selection_dialog_new("Select Color");

	GtkColorSelection *colorSelection = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(myColorSelectionDialog)->colorsel);

	gtk_color_selection_set_previous_color(colorSelection, &myColor);
	gtk_color_selection_set_current_color(colorSelection, &myColor);
	gtk_color_selection_set_has_palette(colorSelection, TRUE);

	gint response = gtk_dialog_run(GTK_DIALOG(myColorSelectionDialog));

	if (response == GTK_RESPONSE_OK) {
		gtk_color_selection_get_current_color(colorSelection, &myColor);
		gtk_widget_modify_bg(myDrawingArea, GTK_STATE_NORMAL, &myColor);
	}

	gtk_widget_hide(myColorSelectionDialog);
}
#endif

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

// vim:ts=2:sw=2:noet
