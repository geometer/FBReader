/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLWin32OptionView.h"
#include "ZLWin32DialogContent.h"
#include "ZLWin32DialogManager.h"

void ZLWin32OptionView::_createItem() {
}

BooleanOptionView::BooleanOptionView(ZLBooleanOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(option) {
	myCheckBox = new W32CheckBox(myOption->name());
	myCheckBox->setChecked(option->initialState());
	myCheckBox->addListener(this);
	tab.insertWidget(myCheckBox, from, to);
}

void BooleanOptionView::_onAccept() const {
	((ZLBooleanOptionEntry*)myOption)->onAccept(myCheckBox->isChecked());
}

void BooleanOptionView::onEvent(const std::string &event, W32EventSender&) {
	if (event == W32CheckBox::STATE_CHANGED_EVENT) {
		((ZLBooleanOptionEntry*)myOption)->onStateChanged(myCheckBox->isChecked());
	}
}

void BooleanOptionView::_show() {
	myCheckBox->setVisible(true);
}

void BooleanOptionView::_hide() {
	myCheckBox->setVisible(false);
}

ChoiceOptionView::ChoiceOptionView(ZLChoiceOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(option) {
	std::vector<std::string> names;
	int num = option->choiceNumber();
	names.reserve(num);
	for (int i = 0; i < num; ++i) {
		names.push_back(option->text(i));
	}
	myButtonGroup = new W32RadioButtonGroup(myOption->name(), names);
	myButtonGroup->setChecked(option->initialCheckedIndex());
	tab.insertWidget(myButtonGroup, from, to);
}

void ChoiceOptionView::_show() {
	myButtonGroup->setVisible(true);
}

void ChoiceOptionView::_hide() {
	myButtonGroup->setVisible(false);
}

void ChoiceOptionView::_onAccept() const {
	((ZLChoiceOptionEntry*)myOption)->onAccept(myButtonGroup->checkedIndex());
}

ComboOptionView::ComboOptionView(ZLComboOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(option) {
	const std::vector<std::string> &values = option->values();
	const std::string &initialValue = option->initialValue();
	int index = 0;
	for (size_t i = 0; i < values.size(); ++i) {
		if (values[i] == initialValue) {
			index = i;
			break;
		}
	}
	myComboBox = new W32ComboBox(values, index);
	myComboBox->setEditable(option->isEditable());
	myComboBox->addListener(this);
	const std::string &name = option->name();
	if (name.empty()) {
		myLabel = 0;
		tab.insertWidget(myComboBox, from, to);
	} else {
		myLabel = new W32Label(name, W32Label::ALIGN_RIGHT);
		tab.insertWidget(myLabel, from, (from + to) / 2);
		tab.insertWidget(myComboBox, (from + to) / 2 + 1, to);
	}
}

void ComboOptionView::_show() {
	myComboBox->setVisible(true);
	if (myLabel != 0) {
		myLabel->setVisible(true);
	}
}

void ComboOptionView::_hide() {
	myComboBox->setVisible(false);
	if (myLabel != 0) {
		myLabel->setVisible(false);
	}
}

void ComboOptionView::_setActive(bool active) {
	myComboBox->setEnabled(active);
}

void ComboOptionView::_onAccept() const {
	((ZLComboOptionEntry*)myOption)->onAccept(myComboBox->text());
}

void ComboOptionView::reset() {
	ZLComboOptionEntry &o = (ZLComboOptionEntry&)*myOption;
	const std::vector<std::string> &values = o.values();
	const std::string &initialValue = o.initialValue();
	int index = 0;
	for (size_t i = 0; i < values.size(); ++i) {
		if (values[i] == initialValue) {
			index = i;
			break;
		}
	}
	myComboBox->setList(values);
	myComboBox->setSelection(index);
}

void ComboOptionView::onEvent(const std::string &event, W32EventSender&) {
	ZLComboOptionEntry &o = (ZLComboOptionEntry&)*myOption;
	if (event == W32ComboBox::VALUE_EDITED_EVENT) {
		if (o.useOnValueEdited()) {
			o.onValueEdited(myComboBox->text());
		}
	} else if (event == W32ComboBox::SELECTION_CHANGED_EVENT) {
		const std::string text = myComboBox->text();
		size_t index;
		const std::vector<std::string> &values = o.values();
		for (index = 0; index < values.size(); ++index) {
			if (values[index] == text) {
				break;
			}
		}
		if (index < values.size()) {
  		o.onValueSelected(index);
		}
	}
}

SpinOptionView::SpinOptionView(ZLSpinOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(option) {
	mySpinBox = new W32SpinBox(option->minValue(), option->maxValue(), option->initialValue());
	const std::string &name = option->name();
	if (name.empty()) {
		myLabel = 0;
		tab.insertWidget(mySpinBox, from, to);
	} else {
		myLabel = new W32Label(name, W32Label::ALIGN_RIGHT);
		tab.insertWidget(myLabel, from, (from + to) / 2);
		tab.insertWidget(mySpinBox, (from + to) / 2 + 1, to);
	}
}

void SpinOptionView::_onAccept() const {
	((ZLSpinOptionEntry*)myOption)->onAccept(mySpinBox->value());
}

void SpinOptionView::_show() {
	mySpinBox->setVisible(true);
	if (myLabel != 0) {
		myLabel->setVisible(true);
	}
}

void SpinOptionView::_hide() {
	mySpinBox->setVisible(false);
	if (myLabel != 0) {
		myLabel->setVisible(false);
	}
}

StringOptionView::StringOptionView(ZLStringOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(option) {
	myLineEditor = new W32LineEditor(option->initialValue());
	myLineEditor->addListener(this);
	const std::string &name = option->name();
	if (name.empty()) {
		myLabel = 0;
		tab.insertWidget(myLineEditor, from, to);
	} else {
		myLabel = new W32Label(name, W32Label::ALIGN_RIGHT);
		tab.insertWidget(myLabel, from, (from + to) / 2);
		tab.insertWidget(myLineEditor, (from + to) / 2 + 1, to);
	}
}

void StringOptionView::reset() {
	myLineEditor->setText(((ZLStringOptionEntry*)myOption)->initialValue());
}

void StringOptionView::_show() {
	myLineEditor->setVisible(true);
	if (myLabel != 0) {
		myLabel->setVisible(true);
	}
}

void StringOptionView::_hide() {
	myLineEditor->setVisible(false);
	if (myLabel != 0) {
		myLabel->setVisible(false);
	}
}

void StringOptionView::onEvent(const std::string &event, W32EventSender&) {
	if (event == W32LineEditor::VALUE_EDITED_EVENT) {
		ZLStringOptionEntry &o = (ZLStringOptionEntry&)*myOption;
		if (o.useOnValueEdited()) {
			o.onValueEdited(myLineEditor->text());
		}
	}
}

void StringOptionView::_setActive(bool active) {
	myLineEditor->setEditable(active);
}

void StringOptionView::_onAccept() const {
	((ZLStringOptionEntry*)myOption)->onAccept(myLineEditor->text());
}

/*
void ColorOptionView::_createItem() {
	const ZLColor &color = ((ZLColorOptionEntry*)myOption)->color();

	myDrawingArea = gtk_drawing_area_new();

	gtk_widget_set_size_request(GTK_WIDGET(myDrawingArea), 60, 20);
	myWidget = gtk_table_new(3, 4, false);

	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new(""), 0, 3, 0, 1, (Win32AttachOptions)(GTK_FILL|GTK_SHRINK), (Win32AttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new("Red"), 0, 1, 1, 2, (Win32AttachOptions)(GTK_FILL|GTK_SHRINK), (Win32AttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new("Green"), 0, 1, 2, 3, (Win32AttachOptions)(GTK_FILL|GTK_SHRINK), (Win32AttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);
	gtk_table_attach(GTK_TABLE(myWidget), gtk_label_new("Blue"), 0, 1, 3, 4, (Win32AttachOptions)(GTK_FILL|GTK_SHRINK), (Win32AttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

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

	Win32Widget *frame = gtk_frame_new(NULL);

	gtk_container_add(GTK_CONTAINER(frame), myDrawingArea);

	gtk_table_attach(GTK_TABLE(myWidget), frame, 2, 3, 1, 4, (Win32AttachOptions)(GTK_FILL|GTK_SHRINK), (Win32AttachOptions)(GTK_FILL|GTK_EXPAND), 0, 0);

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

void ColorOptionView::_onSliderMove(Win32Range *, gpointer self) {
	((ColorOptionView *)self)->onSliderMove();
}

void ColorOptionView::onSliderMove() {
	myColor.red = (int)(gtk_range_get_value(GTK_RANGE(myRSlider)) * 65535 / 255);
	myColor.blue = (int)(gtk_range_get_value(GTK_RANGE(myBSlider)) * 65535 / 255);
	myColor.green = (int)(gtk_range_get_value(GTK_RANGE(myGSlider)) * 65535 / 255);

	gtk_widget_modify_bg(myDrawingArea, GTK_STATE_NORMAL, &myColor);
}

void ColorOptionView::_onAccept() const {
	((ZLColorOptionEntry*)myOption)->onAccept(ZLColor(
		myColor.red * 255 / 65535,
		myColor.green * 255 / 65535,
		myColor.blue * 255 / 65535
	));
}
*/

KeyOptionView::KeyOptionView(ZLKeyOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(option) {
	myVBox = new W32VBox();
	myHBox = new W32HBox();
	myHBox->setSpacing(8);
	myHBox->setHomogeneous(true);
	myHBox->setAlignment(W32HBox::FILL);
	myHBox->addElement(new W32Label("Action For Key", W32Label::ALIGN_RIGHT));
	myKeyNameEditor = new W32KeyNameEditor();
	myKeyNameEditor->addListener(this);
	myHBox->addElement(myKeyNameEditor);
	myComboBox = new W32ComboBox(((ZLKeyOptionEntry*)myOption)->actionNames(), 0);
	myComboBox->addListener(this);
	myVBox->addElement(myHBox);
	myVBox->addElement(myComboBox);
	tab.insertWidget(myVBox, from, to);
}

void KeyOptionView::onEvent(const std::string &event, W32EventSender&) {
	if (event == W32KeyNameEditor::TEXT_CHANGED_EVENT) {
		myCurrentKey = myKeyNameEditor->text();
		myComboBox->setVisible(!myCurrentKey.empty());
		myComboBox->setSelection(((ZLKeyOptionEntry*)myOption)->actionIndex(myCurrentKey));
	} else if (event == W32ComboBox::SELECTION_CHANGED_EVENT) {
		((ZLKeyOptionEntry*)myOption)->onValueChanged(myCurrentKey, myComboBox->index());
	}
}

void KeyOptionView::_show() {
	myHBox->setVisible(true);
	if (!myCurrentKey.empty()) {
		myComboBox->setVisible(true);
	}
}

void KeyOptionView::_hide() {
	myVBox->setVisible(false);
}

void KeyOptionView::_onAccept() const {
	((ZLKeyOptionEntry*)myOption)->onAccept();
}
