/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLOptions.h>
#include <ZLStringUtil.h>

#include "ZLWin32OptionView.h"
#include "ZLWin32DialogContent.h"
#include "ZLWin32DialogManager.h"

void ZLWin32OptionView::_createItem() {
}

BooleanOptionView::BooleanOptionView(const std::string &name, const std::string &tooltip, ZLBooleanOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(name, tooltip, option) {
	myCheckBox = new W32CheckBox(name);
	myCheckBox->setChecked(option->initialState());
	myCheckBox->addListener(this);
	tab.insertWidget(myCheckBox, from, to);
}

void BooleanOptionView::_onAccept() const {
	((ZLBooleanOptionEntry&)*myOption).onAccept(myCheckBox->isChecked());
}

void BooleanOptionView::onEvent(const std::string &event, W32EventSender&) {
	if (event == W32CheckBox::STATE_CHANGED_EVENT) {
		((ZLBooleanOptionEntry&)*myOption).onStateChanged(myCheckBox->isChecked());
	}
}

void BooleanOptionView::_setActive(bool active) {
	myCheckBox->setEditable(active);
}

void BooleanOptionView::_show() {
	myCheckBox->setVisible(true);
}

void BooleanOptionView::_hide() {
	myCheckBox->setVisible(false);
}

Boolean3OptionView::Boolean3OptionView(const std::string &name, const std::string &tooltip, ZLBoolean3OptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(name, tooltip, option) {
	myTristateBox = new W32TristateBox(name);
	myTristateBox->setState(option->initialState());
	myTristateBox->addListener(this);
	tab.insertWidget(myTristateBox, from, to);
}

void Boolean3OptionView::_onAccept() const {
	((ZLBoolean3OptionEntry&)*myOption).onAccept(myTristateBox->state());
}

void Boolean3OptionView::onEvent(const std::string &event, W32EventSender&) {
	if (event == W32CheckBox::STATE_CHANGED_EVENT) {
		((ZLBoolean3OptionEntry&)*myOption).onStateChanged(myTristateBox->state());
	}
}

void Boolean3OptionView::_setActive(bool active) {
	myTristateBox->setEditable(active);
}

void Boolean3OptionView::_show() {
	myTristateBox->setVisible(true);
}

void Boolean3OptionView::_hide() {
	myTristateBox->setVisible(false);
}

ChoiceOptionView::ChoiceOptionView(const std::string &name, const std::string &tooltip, ZLChoiceOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(name, tooltip, option) {
	std::vector<std::string> names;
	int num = option->choiceNumber();
	names.reserve(num);
	for (int i = 0; i < num; ++i) {
		names.push_back(option->text(i));
	}
	myButtonGroup = new W32RadioButtonGroup(name, names);
	myButtonGroup->setChecked(option->initialCheckedIndex());
	tab.insertWidget(myButtonGroup, from, to);
}

void ChoiceOptionView::_setActive(bool active) {
	myButtonGroup->setEditable(active);
}

void ChoiceOptionView::_show() {
	myButtonGroup->setVisible(true);
}

void ChoiceOptionView::_hide() {
	myButtonGroup->setVisible(false);
}

void ChoiceOptionView::_onAccept() const {
	((ZLChoiceOptionEntry&)*myOption).onAccept(myButtonGroup->checkedIndex());
}

ComboOptionView::ComboOptionView(const std::string &name, const std::string &tooltip, ZLComboOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(name, tooltip, option) {
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
	((ZLComboOptionEntry&)*myOption).onAccept(myComboBox->text());
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

SpinOptionView::SpinOptionView(const std::string &name, const std::string &tooltip, ZLSpinOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(name, tooltip, option) {
	mySpinBox = new W32SpinBox(option->minValue(), option->maxValue(), option->initialValue());
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
	((ZLSpinOptionEntry&)*myOption).onAccept(mySpinBox->value());
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

StringOptionView::StringOptionView(const std::string &name, const std::string &tooltip, ZLStringOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(name, tooltip, option) {
	myLineEditor = new W32LineEditor(option->initialValue());
	myLineEditor->addListener(this);
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
	myLineEditor->setText(((ZLStringOptionEntry&)*myOption).initialValue());
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
	((ZLStringOptionEntry&)*myOption).onAccept(myLineEditor->text());
}

ColorOptionView::ColorOptionView(const std::string &name, const std::string &tooltip, ZLColorOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(name, tooltip, option) {
	myStandardColorComboBox = new W32ColorComboBox(option->color());
	myStandardColorComboBox->addListener(this);
	myCustomColorButton = new W32PushButton(
		ZLResource::resource("color")["custom..."].value()
	);
	myCustomColorButton->addListener(this);
	int mid = (from + to) / 2 + 1;
	tab.insertWidget(myStandardColorComboBox, from, mid);
	tab.insertWidget(myCustomColorButton, mid + 1, to);
}

void ColorOptionView::onEvent(const std::string &event, W32EventSender&) {
	static std::string OPTION_PREFIX = "Color";
	static std::string COLOR_GROUP_NAME = "ColorDialog";
	if (event == W32PushButton::RELEASED_EVENT) {
		CHOOSECOLOR chooser;
		COLORREF refs[16];
		for (int i = 0; i < 16; ++i) {
			std::string optionName = OPTION_PREFIX;
			ZLStringUtil::appendNumber(optionName, i);
			ZLColor color = ZLColorOption(
				ZLCategoryKey::LOOK_AND_FEEL,
				COLOR_GROUP_NAME,
				optionName,
				ZLColor(0, 0, 0)
			).value();
			refs[i] = RGB(color.Red, color.Green, color.Blue);
		}
		chooser.lStructSize = sizeof(chooser);
		chooser.hwndOwner = 0; // TODO: !!!
		//chooser.hInstance = 0;
		ZLColor currentColor = myStandardColorComboBox->selectedColor();
		chooser.rgbResult = RGB(currentColor.Red, currentColor.Green, currentColor.Blue);
		chooser.lpCustColors = refs;
		chooser.Flags = CC_RGBINIT | CC_FULLOPEN | CC_SOLIDCOLOR;
		//chooser.lCustData = 0;
		//chooser.lpfnHook = 0;
		//chooser.lpTemplateName = 0;

		bool code = ChooseColor(&chooser);

		for (int i = 0; i < 16; ++i) {
			std::string optionName = OPTION_PREFIX;
			ZLStringUtil::appendNumber(optionName, i);
			ZLColor color(GetRValue(refs[i]), GetGValue(refs[i]), GetBValue(refs[i]));
			ZLColorOption(
				ZLCategoryKey::LOOK_AND_FEEL,
				COLOR_GROUP_NAME,
				optionName,
				ZLColor(0, 0, 0)
			).setValue(color);
		}

		if (code) {
			ZLColor color(GetRValue(chooser.rgbResult), GetGValue(chooser.rgbResult), GetBValue(chooser.rgbResult));
			myStandardColorComboBox->setSelectedColor(color);
		}
	}
}

void ColorOptionView::_show() {
	myStandardColorComboBox->setVisible(true);
	myCustomColorButton->setVisible(true);
}

void ColorOptionView::_hide() {
	myStandardColorComboBox->setVisible(false);
	myCustomColorButton->setVisible(false);
}

void ColorOptionView::reset() {
	ZLColorOptionEntry &colorEntry = (ZLColorOptionEntry&)*myOption;
	colorEntry.onReset(myStandardColorComboBox->selectedColor());
	myStandardColorComboBox->removeCustomColor();
	myStandardColorComboBox->setSelectedColor(colorEntry.color());
}

void ColorOptionView::_onAccept() const {
	((ZLColorOptionEntry&)*myOption).onAccept(myStandardColorComboBox->selectedColor());
}

KeyOptionView::KeyOptionView(const std::string &name, const std::string &tooltip, ZLKeyOptionEntry *option, ZLWin32DialogContent &tab, int from, int to) : ZLWin32OptionView(name, tooltip, option) {
	myVBox = new W32VBox();
	myVBox->setSpacing(4);
	myHBox = new W32HBox();
	myHBox->setSpacing(8);
	myHBox->setHomogeneous(true);
	myHBox->setAlignment(W32HBox::FILL);
	myHBox->addElement(new W32Label(ZLResource::resource("keyOptionView")["actionFor"].value(), W32Label::ALIGN_RIGHT));
	myKeyNameEditor = new W32KeyNameEditor();
	myKeyNameEditor->addListener(this);
	myHBox->addElement(myKeyNameEditor);
	myComboBox = new W32ComboBox(((ZLKeyOptionEntry&)*myOption).actionNames(), 0);
	myComboBox->addListener(this);
	myVBox->addElement(myHBox);
	myVBox->addElement(myComboBox);
	tab.insertWidget(myVBox, from, to);
}

void KeyOptionView::onEvent(const std::string &event, W32EventSender&) {
	if (event == W32KeyNameEditor::TEXT_CHANGED_EVENT) {
		myCurrentKey = myKeyNameEditor->text();
		myComboBox->setVisible(!myCurrentKey.empty());
		myComboBox->setSelection(((ZLKeyOptionEntry&)*myOption).actionIndex(myCurrentKey));
		((ZLKeyOptionEntry&)*myOption).onKeySelected(myCurrentKey);
	} else if (event == W32ComboBox::SELECTION_CHANGED_EVENT) {
		((ZLKeyOptionEntry&)*myOption).onValueChanged(myCurrentKey, myComboBox->index());
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
	((ZLKeyOptionEntry&)*myOption).onAccept();
}

void KeyOptionView::reset() {
	myCurrentKey.erase();
	myKeyNameEditor->clear();
	myComboBox->setVisible(false);
}

