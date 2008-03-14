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

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>
#include <QtGui/QSlider>
#include <QtGui/QLayout>

#include <ZLStringUtil.h>
#include <ZLDialogManager.h>
#include "../util/ZLQtKeyUtil.h"

#include "ZLQtOptionView.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"

void ZLQtOptionView::_show() {
	for (std::vector<QWidget*>::iterator it = myWidgets.begin(); it != myWidgets.end(); ++it) {
		(*it)->show();
	}
}

void ZLQtOptionView::_hide() {
	for (std::vector<QWidget*>::iterator it = myWidgets.begin(); it != myWidgets.end(); ++it) {
		(*it)->hide();
	}
}

void BooleanOptionView::_createItem() {
	myCheckBox = new QCheckBox(::qtString(ZLOptionView::name()), myTab->widget());
	myCheckBox->setChecked(((ZLBooleanOptionEntry&)*myOption).initialState());
	myWidgets.push_back(myCheckBox);
	myTab->addItem(myCheckBox, myRow, myFromColumn, myToColumn);
	connect(myCheckBox, SIGNAL(toggled(bool)), this, SLOT(onStateChanged(bool)));
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
	Qt::CheckState state = Qt::PartiallyChecked;
	switch (((ZLBoolean3OptionEntry&)*myOption).initialState()) {
		case B3_FALSE:
			state = Qt::Unchecked;
			break;
		case B3_TRUE:
			state = Qt::Checked;
			break;
		case B3_UNDEFINED:
			state = Qt::PartiallyChecked;
			break;
	}
	myCheckBox->setCheckState(state);
	myWidgets.push_back(myCheckBox);
	myTab->addItem(myCheckBox, myRow, myFromColumn, myToColumn);
	connect(myCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
}

void Boolean3OptionView::_setActive(bool active) {
	myCheckBox->setEnabled(active);
}

void Boolean3OptionView::_onAccept() const {
	ZLBoolean3 value = B3_UNDEFINED;
	switch (myCheckBox->checkState()) {
		case Qt::Checked:
			value = B3_TRUE;
			break;
		case Qt::Unchecked:
			value = B3_FALSE;
			break;
		case Qt::PartiallyChecked:
			value = B3_UNDEFINED;
			break;
	}
	((ZLBoolean3OptionEntry&)*myOption).onAccept(value);
}

void Boolean3OptionView::onStateChanged(int state) const {
	ZLBoolean3 value = B3_UNDEFINED;
	switch (state) {
		case Qt::Checked:
			value = B3_TRUE;
			break;
		case Qt::Unchecked:
			value = B3_FALSE;
			break;
		case Qt::PartiallyChecked:
			value = B3_UNDEFINED;
			break;
	}
	((ZLBoolean3OptionEntry&)*myOption).onStateChanged(value);
}

void ChoiceOptionView::_createItem() {
	myGroupBox = new QGroupBox(::qtString(ZLOptionView::name()));
	myWidgets.push_back(myGroupBox);
	QVBoxLayout *layout = new QVBoxLayout(myGroupBox);
	myButtons = new QRadioButton*[((ZLChoiceOptionEntry&)*myOption).choiceNumber()];
	for (int i = 0; i < ((ZLChoiceOptionEntry&)*myOption).choiceNumber(); ++i) {
		myButtons[i] = new QRadioButton(myGroupBox);
		myButtons[i]->setText(::qtString(((ZLChoiceOptionEntry&)*myOption).text(i)));
		layout->addWidget(myButtons[i]);
	}
	myButtons[((ZLChoiceOptionEntry&)*myOption).initialCheckedIndex()]->setChecked(true);
	myTab->addItem(myGroupBox, myRow, myFromColumn, myToColumn);
}

void ChoiceOptionView::_setActive(bool active) {
	myGroupBox->setEnabled(active);
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
	QLabel *label = 0;
	const std::string &name = ZLOptionView::name();
	if (!name.empty()) {
		label = new QLabel(::qtString(name), myTab->widget());
	}
	myComboBox = new QComboBox(myTab->widget());
	myComboBox->setEditable(comboOption.isEditable());

	if (label != 0) {
		myWidgets.push_back(label);
	}
	myWidgets.push_back(myComboBox);

	connect(myComboBox, SIGNAL(activated(int)), this, SLOT(onValueSelected(int)));
	connect(myComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(onValueEdited(const QString&)));

	if (label != 0) {
		int width = myToColumn - myFromColumn + 1;
		myTab->addItem(label, myRow, myFromColumn, myFromColumn + width / 2 - 1);
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
		myComboBox->insertItem(it - values.begin(), ::qtString(*it));
		if (*it == initial) {
			selectedIndex = index;
		}
	}
	if (selectedIndex >= 0) {
		myComboBox->setCurrentIndex(selectedIndex);
	}
}

void ComboOptionView::_setActive(bool active) {
	myComboBox->setEnabled(active);
}

void ComboOptionView::_onAccept() const {
	((ZLComboOptionEntry&)*myOption).onAccept((const char*)myComboBox->currentText().toUtf8());
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
		o.onValueEdited((const char*)value.toUtf8());
	}
}

void SpinOptionView::_createItem() {
	ZLSpinOptionEntry &entry = (ZLSpinOptionEntry&)*myOption;
	QLabel *label = new QLabel(::qtString(ZLOptionView::name()), myTab->widget());
	mySpinBox = new QSpinBox(myTab->widget());

	myWidgets.push_back(label);
	myWidgets.push_back(mySpinBox);

	mySpinBox->setMinimum(entry.minValue());
	mySpinBox->setMaximum(entry.maxValue());
	mySpinBox->setSingleStep(entry.step());
	mySpinBox->setValue(entry.initialValue());
	int width = myToColumn - myFromColumn + 1;
	myTab->addItem(label, myRow, myFromColumn, myFromColumn + width * 2 / 3 - 1);
	myTab->addItem(mySpinBox, myRow, myFromColumn + width * 2 / 3, myToColumn);
}

void SpinOptionView::_onAccept() const {
	((ZLSpinOptionEntry&)*myOption).onAccept(mySpinBox->value());
}

void StringOptionView::_createItem() {
	myLineEdit = new QLineEdit(myTab->widget());
	myWidgets.push_back(myLineEdit);
	connect(myLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onValueEdited(const QString&)));
	if (!ZLOptionView::name().empty()) {
		QLabel *label = new QLabel(::qtString(ZLOptionView::name()), myTab->widget());
		myWidgets.push_back(label);
		int width = myToColumn - myFromColumn + 1;
		myTab->addItem(label, myRow, myFromColumn, myFromColumn + width / 4 - 1);
		myTab->addItem(myLineEdit, myRow, myFromColumn + width / 4, myToColumn);
	} else {
		myTab->addItem(myLineEdit, myRow, myFromColumn, myToColumn);
	}
	reset();
}

void StringOptionView::_setActive(bool active) {
	myLineEdit->setReadOnly(!active);
}

void StringOptionView::_onAccept() const {
	((ZLStringOptionEntry&)*myOption).onAccept((const char*)myLineEdit->text().toUtf8());
}

void StringOptionView::reset() {
	if (myLineEdit == 0) {
		return;
	}
	myLineEdit->setText(::qtString(((ZLStringOptionEntry&)*myOption).initialValue()));
	myLineEdit->cursorForward(false, -myLineEdit->text().length());
}

void StringOptionView::onValueEdited(const QString &value) {
	ZLStringOptionEntry &o = (ZLStringOptionEntry&)*myOption;
	if (o.useOnValueEdited()) {
		o.onValueEdited((const char*)value.toUtf8());
	}
}

class KeyLineEdit : public QLineEdit {

public:
	KeyLineEdit(KeyOptionView &keyView, QWidget *parent);

protected:
	void focusInEvent(QFocusEvent*);
	void focusOutEvent(QFocusEvent*);
	void keyPressEvent(QKeyEvent *keyEvent);

private:
	KeyOptionView &myKeyView;
};

KeyLineEdit::KeyLineEdit(KeyOptionView &keyView, QWidget *parent) : QLineEdit(parent), myKeyView(keyView) {
	focusOutEvent(0);
}

void KeyLineEdit::keyPressEvent(QKeyEvent *keyEvent) {
	std::string keyText = ZLQtKeyUtil::keyName(keyEvent);
	setText(keyText.c_str());
	if (!keyText.empty()) {
		myKeyView.myCurrentKey = keyText;
		myKeyView.myComboBox->setCurrentIndex(((ZLKeyOptionEntry&)*myKeyView.myOption).actionIndex(keyText));
		myKeyView.myComboBox->show();
	}
	((ZLKeyOptionEntry&)*myKeyView.myOption).onKeySelected(keyText);
}

void KeyLineEdit::focusInEvent(QFocusEvent*) {
	grabKeyboard();
}

void KeyLineEdit::focusOutEvent(QFocusEvent*) {
	releaseKeyboard();
}

void KeyOptionView::_createItem() {
	QWidget *widget = new QWidget(myTab->widget());
	QGridLayout *layout = new QGridLayout(widget);

	QLabel *label = new QLabel(widget);
	label->setText(::qtString(ZLResource::resource("keyOptionView")["actionFor"].value()));
	layout->addWidget(label, 0, 0);

	myKeyEditor = new KeyLineEdit(*this, widget);
	layout->addWidget(myKeyEditor, 0, 1);

	myWidgets.push_back(widget);
	myWidgets.push_back(label);
	myWidgets.push_back(myKeyEditor);

	myComboBox = new QComboBox(widget);
	const std::vector<std::string> &actions = ((ZLKeyOptionEntry&)*myOption).actionNames();
	for (std::vector<std::string>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
		myComboBox->insertItem(it - actions.begin(), ::qtString(*it));
	}
	connect(myComboBox, SIGNAL(activated(int)), this, SLOT(onValueChanged(int)));
	layout->addWidget(myComboBox, 1, 0, 1, 2);
	myTab->addItem(widget, myRow, myFromColumn, myToColumn);
}

void KeyOptionView::reset() {
	if (myKeyEditor == 0) {
		return;
	}
	myCurrentKey.erase();
	myKeyEditor->setText("");
	((ZLKeyOptionEntry&)*myOption).onKeySelected(myCurrentKey);
	myComboBox->hide();
}

void KeyOptionView::_show() {
	ZLQtOptionView::_show();
	if (!myCurrentKey.empty()) {
		myComboBox->show();
	} else {
		myComboBox->hide();
	}
}

void KeyOptionView::_hide() {
	ZLQtOptionView::_hide();
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

QSlider *ColorOptionView::createColorSlider(QGridLayout *layout, int index, const ZLResource &resource, int value) {
	layout->addWidget(new QLabel(::qtString(resource.value()), layout->parentWidget()), index, 0);
	QSlider *slider = new QSlider(Qt::Horizontal, layout->parentWidget());
	layout->addWidget(slider, index, 1);
	slider->setMinimum(0);
	slider->setMaximum(255);
	slider->setSingleStep(5);
	slider->setTracking(true);
	slider->setValue(value);
	connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMove(int)));
	return slider;
}

void ColorOptionView::_createItem() {
	QWidget *widget = new QWidget(myTab->widget());
	myWidgets.push_back(widget);
	QGridLayout *layout = new QGridLayout(widget);
	const ZLColor &color = ((ZLColorOptionEntry&)*myOption).color();
	const ZLResource &resource = ZLResource::resource(ZLDialogManager::COLOR_KEY);
	myRSlider = createColorSlider(layout, 0, resource["red"], color.Red);
	myGSlider = createColorSlider(layout, 1, resource["green"], color.Green);
	myBSlider = createColorSlider(layout, 2, resource["blue"], color.Blue);

	myColorBar = new QLabel("                  ", widget);
	QPalette palette = myColorBar->palette();
	palette.setColor(myColorBar->backgroundRole(), QColor(color.Red, color.Green, color.Blue));
	myColorBar->setPalette(palette);
	myColorBar->setFrameStyle(QFrame::Panel | QFrame::Plain);
	layout->addWidget(myColorBar, 0, 2, 3, 1);
	myColorBar->setAutoFillBackground(true);

	myTab->addItem(widget, myRow, myFromColumn, myToColumn);
}

void ColorOptionView::reset() {
	if (myColorBar == 0) {
		return;
	}
	ZLColorOptionEntry &colorEntry = (ZLColorOptionEntry&)*myOption;
	colorEntry.onReset(ZLColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
	const ZLColor &color = colorEntry.color();
	myRSlider->setValue(color.Red);
	myGSlider->setValue(color.Green);
	myBSlider->setValue(color.Blue);
	QPalette palette = myColorBar->palette();
	palette.setColor(myColorBar->backgroundRole(), QColor(color.Red, color.Green, color.Blue));
	myColorBar->setPalette(palette);
}

void ColorOptionView::onSliderMove(int) {
	QPalette palette = myColorBar->palette();
	palette.setColor(myColorBar->backgroundRole(), QColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
	myColorBar->setPalette(palette);
}

void ColorOptionView::_onAccept() const {
	((ZLColorOptionEntry&)*myOption).onAccept(ZLColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
}
