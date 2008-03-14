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

#include <cctype>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qlayout.h>
#include <qlistbox.h>

#include <ZLStringUtil.h>
#include <ZLDialogManager.h>
#include "../util/ZLQtKeyUtil.h"

#include "ZLQtOptionView.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"

void BooleanOptionView::_createItem() {
	myCheckBox = new QCheckBox(::qtString(ZLOptionView::name()), myTab->widget());
	myCheckBox->setChecked(((ZLBooleanOptionEntry&)*myOption).initialState());
	myTab->addItem(myCheckBox, myRow, myFromColumn, myToColumn);
	connect(myCheckBox, SIGNAL(toggled(bool)), this, SLOT(onStateChanged(bool)));
}

void BooleanOptionView::_show() {
	myCheckBox->show();
}

void BooleanOptionView::_hide() {
	myCheckBox->hide();
}

void BooleanOptionView::_setActive(bool active) {
	myCheckBox->setEnabled(active);
}

void BooleanOptionView::_onAccept() const {
	((ZLBooleanOptionEntry&)*myOption).onAccept(myCheckBox->isChecked());
}

void BooleanOptionView::onStateChanged(bool state) const {
	((ZLBooleanOptionEntry&)*myOption).onStateChanged(state);
}

void Boolean3OptionView::_createItem() {
	myCheckBox = new QCheckBox(::qtString(ZLOptionView::name()), myTab->widget());
	myCheckBox->setTristate(true);
	switch (((ZLBoolean3OptionEntry&)*myOption).initialState()) {
		case B3_FALSE:
			myCheckBox->setChecked(false);
			break;
		case B3_TRUE:
			myCheckBox->setChecked(true);
			break;
		case B3_UNDEFINED:
			myCheckBox->setNoChange();
			break;
	}
	myTab->addItem(myCheckBox, myRow, myFromColumn, myToColumn);
	connect(myCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
}

void Boolean3OptionView::_show() {
	myCheckBox->show();
}

void Boolean3OptionView::_hide() {
	myCheckBox->hide();
}

void Boolean3OptionView::_setActive(bool active) {
	myCheckBox->setEnabled(active);
}

void Boolean3OptionView::_onAccept() const {
	ZLBoolean3 value = B3_UNDEFINED;
	switch (myCheckBox->state()) {
		case QCheckBox::On:
			value = B3_TRUE;
			break;
		case QCheckBox::Off:
			value = B3_FALSE;
			break;
		case QCheckBox::NoChange:
			value = B3_UNDEFINED;
			break;
	}
	((ZLBoolean3OptionEntry&)*myOption).onAccept(value);
}

void Boolean3OptionView::onStateChanged(int state) const {
	ZLBoolean3 value = B3_UNDEFINED;
	switch (state) {
		case QCheckBox::On:
			value = B3_TRUE;
			break;
		case QCheckBox::Off:
			value = B3_FALSE;
			break;
		case QCheckBox::NoChange:
			value = B3_UNDEFINED;
			break;
	}
	((ZLBoolean3OptionEntry&)*myOption).onStateChanged(value);
}

void ChoiceOptionView::_createItem() {
	myGroup = new QButtonGroup(::qtString(ZLOptionView::name()), myTab->widget());
	QVBoxLayout *layout = new QVBoxLayout(myGroup, 12);
	layout->addSpacing(myGroup->fontMetrics().height());
	myButtons = new QRadioButton*[((ZLChoiceOptionEntry&)*myOption).choiceNumber()];
	for (int i = 0; i < ((ZLChoiceOptionEntry&)*myOption).choiceNumber(); ++i) {
		myButtons[i] = new QRadioButton((QButtonGroup*)layout->parent());
		myButtons[i]->setText(::qtString(((ZLChoiceOptionEntry&)*myOption).text(i)));
		layout->addWidget(myButtons[i]);
	}
	myButtons[((ZLChoiceOptionEntry&)*myOption).initialCheckedIndex()]->setChecked(true);
	myTab->addItem(myGroup, myRow, myFromColumn, myToColumn);
}

void ChoiceOptionView::_show() {
	myGroup->show();
}

void ChoiceOptionView::_hide() {
	myGroup->hide();
}

void ChoiceOptionView::_setActive(bool active) {
	myGroup->setEnabled(active);
}

void ChoiceOptionView::_onAccept() const {
	for (int i = 0; i < ((ZLChoiceOptionEntry&)*myOption).choiceNumber(); ++i) {
		if (myButtons[i]->isChecked()) {
			((ZLChoiceOptionEntry&)*myOption).onAccept(i);
			return;
		}
	}
}

void ComboOptionView::_createItem() {
	const ZLComboOptionEntry &comboOption = (ZLComboOptionEntry&)*myOption;
	const std::string &name = ZLOptionView::name();
	if (!name.empty()) {
		myLabel = new QLabel(::qtString(name), myTab->widget());
	}
	myComboBox = new QComboBox(myTab->widget());
	myComboBox->setEditable(comboOption.isEditable());

	connect(myTab->parentWidget(), SIGNAL(resized(const QSize&)), this, SLOT(onTabResized(const QSize&)));
	connect(myComboBox, SIGNAL(activated(int)), this, SLOT(onValueSelected(int)));
	connect(myComboBox, SIGNAL(textChanged(const QString&)), this, SLOT(onValueEdited(const QString&)));

	if (myLabel != 0) {
		int width = myToColumn - myFromColumn + 1;
		myTab->addItem(myLabel, myRow, myFromColumn, myFromColumn + width / 2 - 1);
		myTab->addItem(myComboBox, myRow, myToColumn - width / 2 + 1, myToColumn);
	} else {
		myTab->addItem(myComboBox, myRow, myFromColumn, myToColumn);
	}

	reset();
}

void ComboOptionView::reset() {
	if (myComboBox == 0) {
		return;
	}

	const int count = myComboBox->count();
	for (int i = 0; i < count; ++i) {
		myComboBox->removeItem(0);
	}

	const ZLComboOptionEntry &comboOption = (ZLComboOptionEntry&)*myOption;
	const std::vector<std::string> &values = comboOption.values();
	const std::string &initial = comboOption.initialValue();
	int selectedIndex = -1;
	int index = 0;
	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++index) {
		myComboBox->insertItem(::qtString(*it));
		if (*it == initial) {
			selectedIndex = index;
		}
	}
	if (selectedIndex >= 0) {
		myComboBox->setCurrentItem(selectedIndex);
	}
}

void ComboOptionView::onTabResized(const QSize &size) {
	myComboBox->setMaximumWidth(size.width() / 2 - 30);
}

void ComboOptionView::_show() {
	if (myLabel != 0) {
		myLabel->show();
	}
	myComboBox->show();
}

void ComboOptionView::_hide() {
	if (myLabel != 0) {
		myLabel->hide();
	}
	myComboBox->hide();
}

void ComboOptionView::_setActive(bool active) {
	myComboBox->setEnabled(active);
}

void ComboOptionView::_onAccept() const {
	((ZLComboOptionEntry&)*myOption).onAccept((const char*)myComboBox->currentText().utf8());
}

void ComboOptionView::onValueSelected(int index) {
	ZLComboOptionEntry &o = (ZLComboOptionEntry&)*myOption;
	if ((index >= 0) && (index < (int)o.values().size())) {
		o.onValueSelected(index);
	}
}

void ComboOptionView::onValueEdited(const QString &value) {
	ZLComboOptionEntry &o = (ZLComboOptionEntry&)*myOption;
	if (o.useOnValueEdited()) {
		o.onValueEdited((const char*)value.utf8());
	}
}

void SpinOptionView::_createItem() {
	myLabel = new QLabel(::qtString(ZLOptionView::name()), myTab->widget());
	mySpinBox = new QSpinBox(
		((ZLSpinOptionEntry&)*myOption).minValue(),
		((ZLSpinOptionEntry&)*myOption).maxValue(),
		((ZLSpinOptionEntry&)*myOption).step(), myTab->widget()
	);
	mySpinBox->setValue(((ZLSpinOptionEntry&)*myOption).initialValue());
	int width = myToColumn - myFromColumn + 1;
	myTab->addItem(myLabel, myRow, myFromColumn, myFromColumn + width * 2 / 3 - 1);
	myTab->addItem(mySpinBox, myRow, myFromColumn + width * 2 / 3, myToColumn);
}

void SpinOptionView::_show() {
	myLabel->show();
	mySpinBox->show();
}

void SpinOptionView::_hide() {
	myLabel->hide();
	mySpinBox->hide();
}

void SpinOptionView::_onAccept() const {
	((ZLSpinOptionEntry&)*myOption).onAccept(mySpinBox->value());
}

void StringOptionView::_createItem() {
	myLineEdit = new QLineEdit(myTab->widget());
	connect(myLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onValueEdited(const QString&)));
	if (!ZLOptionView::name().empty()) {
		myLabel = new QLabel(::qtString(ZLOptionView::name()), myTab->widget());
		int width = myToColumn - myFromColumn + 1;
		myTab->addItem(myLabel, myRow, myFromColumn, myFromColumn + width / 4 - 1);
		myTab->addItem(myLineEdit, myRow, myFromColumn + width / 4, myToColumn);
	} else {
		myLabel = 0;
		myTab->addItem(myLineEdit, myRow, myFromColumn, myToColumn);
	}
	reset();
}

void StringOptionView::_show() {
	if (myLabel != 0) {
		myLabel->show();
	}
	myLineEdit->show();
}

void StringOptionView::_hide() {
	if (myLabel != 0) {
		myLabel->hide();
	}
	myLineEdit->hide();
}

void StringOptionView::_setActive(bool active) {
	myLineEdit->setReadOnly(!active);
}

void StringOptionView::_onAccept() const {
	((ZLStringOptionEntry&)*myOption).onAccept((const char*)myLineEdit->text().utf8());
}

void StringOptionView::reset() {
	if (myLineEdit == 0) {
		return;
	}
	myLineEdit->setText(::qtString(((ZLStringOptionEntry&)*myOption).initialValue()));
	myLineEdit->cursorLeft(false, myLineEdit->text().length());
}

void StringOptionView::onValueEdited(const QString &value) {
	ZLStringOptionEntry &o = (ZLStringOptionEntry&)*myOption;
	if (o.useOnValueEdited()) {
		o.onValueEdited((const char*)value.utf8());
	}
}

class KeyLineEdit : public QLineEdit {

public:
	KeyLineEdit(KeyOptionView &keyView);

protected:
	void focusInEvent(QFocusEvent*);
	void focusOutEvent(QFocusEvent*);
	void keyPressEvent(QKeyEvent *keyEvent);

private:
	KeyOptionView &myKeyView;
};

KeyLineEdit::KeyLineEdit(KeyOptionView &keyView) : QLineEdit(keyView.myWidget), myKeyView(keyView) {
	focusOutEvent(0);
}

void KeyLineEdit::focusInEvent(QFocusEvent*) {
	grabKeyboard();
}

void KeyLineEdit::focusOutEvent(QFocusEvent*) {
	releaseKeyboard();
}

void KeyLineEdit::keyPressEvent(QKeyEvent *keyEvent) {
	std::string keyText = ZLQtKeyUtil::keyName(keyEvent);
	setText(keyText.c_str());
	if (!keyText.empty()) {
		myKeyView.myCurrentKey = keyText;
		myKeyView.myComboBox->setCurrentItem(((ZLKeyOptionEntry&)*myKeyView.myOption).actionIndex(keyText));
		myKeyView.myComboBox->show();
	}
	((ZLKeyOptionEntry&)*myKeyView.myOption).onKeySelected(keyText);
}

void KeyOptionView::_createItem() {
	myWidget = new QWidget(myTab->widget());
	QGridLayout *layout = new QGridLayout(myWidget, 2, 2, 0, 10);

	myLabel = new QLabel(myWidget);
	myLabel->setText(::qtString(ZLResource::resource("keyOptionView")["actionFor"].value()));
	layout->addWidget(myLabel, 0, 0);

	myKeyEditor = new KeyLineEdit(*this);
	layout->addWidget(myKeyEditor, 0, 1);

	myComboBox = new QComboBox(myWidget);
	const std::vector<std::string> &actions = ((ZLKeyOptionEntry&)*myOption).actionNames();
	for (std::vector<std::string>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
		myComboBox->insertItem(::qtString(*it));
	}
	connect(myComboBox, SIGNAL(activated(int)), this, SLOT(onValueChanged(int)));
	layout->addMultiCellWidget(myComboBox, 1, 1, 0, 1);
	myTab->addItem(myWidget, myRow, myFromColumn, myToColumn);
}

void KeyOptionView::reset() {
	if (myWidget == 0) {
		return;
	}
	myCurrentKey.erase();
	myKeyEditor->setText("");
	((ZLKeyOptionEntry&)*myOption).onKeySelected(myCurrentKey);
	myComboBox->hide();
}

void KeyOptionView::_show() {
	myWidget->show();
	myLabel->show();
	myKeyEditor->show();
	if (!myCurrentKey.empty()) {
		myComboBox->show();
	} else {
		myComboBox->hide();
	}
}

void KeyOptionView::_hide() {
	myKeyEditor->hide();
	myWidget->hide();
	myLabel->hide();
	myComboBox->hide();
	myCurrentKey.erase();
	myKeyEditor->setText("");
	((ZLKeyOptionEntry&)*myOption).onKeySelected(myCurrentKey);
}

void KeyOptionView::_onAccept() const {
	((ZLKeyOptionEntry&)*myOption).onAccept();
}

void KeyOptionView::onValueChanged(int index) {
	if (!myCurrentKey.empty()) {
		((ZLKeyOptionEntry&)*myOption).onValueChanged(myCurrentKey, index);
	}
}

void ColorOptionView::_createItem() {
	myWidget = new QWidget(myTab->widget());
	QGridLayout *layout = new QGridLayout(myWidget, 3, 3, 0, 10);
	const ZLResource &resource = ZLResource::resource(ZLDialogManager::COLOR_KEY);
	layout->addWidget(new QLabel(::qtString(resource["red"].value()), myWidget), 0, 0);
	layout->addWidget(new QLabel(::qtString(resource["green"].value()), myWidget), 1, 0);
	layout->addWidget(new QLabel(::qtString(resource["blue"].value()), myWidget), 2, 0);
	const ZLColor &color = ((ZLColorOptionEntry&)*myOption).color();
	myRSlider = new QSlider(0, 255, 1, color.Red, QSlider::Horizontal, myWidget);
	myGSlider = new QSlider(0, 255, 1, color.Green, QSlider::Horizontal, myWidget);
	myBSlider = new QSlider(0, 255, 1, color.Blue, QSlider::Horizontal, myWidget);
	connect(myRSlider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMove(int)));
	connect(myGSlider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMove(int)));
	connect(myBSlider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMove(int)));
	layout->addWidget(myRSlider, 0, 1);
	layout->addWidget(myGSlider, 1, 1);
	layout->addWidget(myBSlider, 2, 1);
	myColorBar = new QLabel("                  ", myWidget);
	myColorBar->setBackgroundColor(QColor(color.Red, color.Green, color.Blue));
	myColorBar->setFrameStyle(QFrame::Panel | QFrame::Plain);
	layout->addMultiCellWidget(myColorBar, 0, 2, 2, 2);
	myTab->addItem(myWidget, myRow, myFromColumn, myToColumn);
}

void ColorOptionView::reset() {
	if (myWidget == 0) {
		return;
	}
	ZLColorOptionEntry &colorEntry = (ZLColorOptionEntry&)*myOption;
	colorEntry.onReset(ZLColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
	const ZLColor &color = colorEntry.color();
	myRSlider->setValue(color.Red);
	myGSlider->setValue(color.Green);
	myBSlider->setValue(color.Blue);
	myColorBar->setBackgroundColor(QColor(color.Red, color.Green, color.Blue));
}

void ColorOptionView::_show() {
	myWidget->show();
}

void ColorOptionView::_hide() {
	myWidget->hide();
}

void ColorOptionView::onSliderMove(int) {
	myColorBar->setBackgroundColor(QColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
}

void ColorOptionView::_onAccept() const {
	((ZLColorOptionEntry&)*myOption).onAccept(ZLColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
}

void OrderOptionView::onChangeSelection() {
	QListBoxItem *selected = myListBox->selectedItem();
	myUpButton->setEnabled((selected != 0) && (selected->prev() != 0));
	myDownButton->setEnabled((selected != 0) && (selected->next() != 0));
}

void OrderOptionView::onUpButtonPressed() {
	QListBoxItem *selected = myListBox->selectedItem();
	if ((selected == 0) || (selected->prev() == 0)) {
		return;
	}
	int index = myListBox->index(selected);
	const QString s0 = myListBox->text(index - 1);
	const QString s1 = myListBox->text(index);
	myListBox->changeItem(s0, index);
	myListBox->changeItem(s1, index - 1);
	myListBox->setSelected(index - 1, true);
}

void OrderOptionView::onDownButtonPressed() {
	QListBoxItem *selected = myListBox->selectedItem();
	if ((selected == 0) || (selected->next() == 0)) {
		return;
	}
	int index = myListBox->index(selected);
	const QString s0 = myListBox->text(index);
	const QString s1 = myListBox->text(index + 1);
	myListBox->changeItem(s0, index + 1);
	myListBox->changeItem(s1, index);
	myListBox->setSelected(index + 1, true);
}

void OrderOptionView::_createItem() {
	myWidget = new QWidget(myTab->widget());
	QGridLayout *layout = new QGridLayout(myWidget, 2, 2, 0, 10);

	myListBox = new QListBox(myWidget);
	myListBox->setSelectionMode(QListBox::Single);
	connect(myListBox, SIGNAL(selectionChanged()), this, SLOT(onChangeSelection()));
	layout->addMultiCellWidget(myListBox, 0, 1, 0, 0);

	myUpButton = new QPushButton(myWidget);
	myUpButton->setText("Up");
	layout->addWidget(myUpButton, 0, 1);
	connect(myUpButton, SIGNAL(pressed()), this, SLOT(onUpButtonPressed()));

	myDownButton = new QPushButton(myWidget);
	myDownButton->setText("Down");
	layout->addWidget(myDownButton, 1, 1);
	connect(myDownButton, SIGNAL(pressed()), this, SLOT(onDownButtonPressed()));

	const std::vector<std::string> &values = ((ZLOrderOptionEntry&)*myOption).values();
	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ++it) {
		myListBox->insertItem(::qtString(*it));
	}
	myListBox->setSelected(0, true);

	myTab->addItem(myWidget, myRow, myFromColumn, myToColumn);
}

void OrderOptionView::_show() {
	myWidget->show();
}

void OrderOptionView::_hide() {
	myWidget->hide();
}

void OrderOptionView::_onAccept() const {
	std::vector<std::string> &values = ((ZLOrderOptionEntry&)*myOption).values();
	values.clear();
	const int size = myListBox->numRows();
	for (int i = 0; i < size; ++i) {
		values.push_back((const char*)myListBox->text(i).utf8());
	}
}
