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

#include <abstract/ZLStringUtil.h>
#include <qt/QKeyUtil.h>

#include "QOptionView.h"
#include "QOptionsDialog.h"

void BooleanOptionView::_createItem() {
	myCheckBox = new QCheckBox(myOption->name().c_str(), myTab);
	myCheckBox->setChecked(((ZLBooleanOptionEntry*)myOption)->initialState());
	myTab->addItem(myCheckBox, myRow, myFromColumn, myToColumn);
	connect(myCheckBox, SIGNAL(toggled(bool)), this, SLOT(onValueChange(bool)));
}

void BooleanOptionView::_show() {
	myCheckBox->show();
}

void BooleanOptionView::_hide() {
	myCheckBox->hide();
}

void BooleanOptionView::_onAccept() const {
	((ZLBooleanOptionEntry*)myOption)->onAccept(myCheckBox->isChecked());
}

void BooleanOptionView::onValueChange(bool state) const {
	((ZLBooleanOptionEntry*)myOption)->onValueChange(state);
}

void ChoiceOptionView::_createItem() {
	myGroup = new QButtonGroup(myOption->name().c_str(), myTab);
	QVBoxLayout *layout = new QVBoxLayout(myGroup, 12);
	layout->addSpacing(myGroup->fontMetrics().height());
	myButtons = new QRadioButton*[((ZLChoiceOptionEntry*)myOption)->choiceNumber()];
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); ++i) {
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
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); ++i) {
		if (myButtons[i]->isChecked()) {
			((ZLChoiceOptionEntry*)myOption)->onAccept(i);
			return;
		}
	}
}

void ComboOptionView::_createItem() {
	myLabel = new QLabel(myOption->name().c_str(), myTab);
	myComboBox = new QComboBox(myTab);
	const std::vector<std::string> &values = ((ZLComboOptionEntry*)myOption)->values();
	const std::string &initial = ((ZLComboOptionEntry*)myOption)->initialValue();
	int selectedIndex = -1;
	int index = 0;
	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++index) {
		myComboBox->insertItem(it->c_str());
		if (*it == initial) {
			selectedIndex = index;
		}
	}
	connect(myComboBox, SIGNAL(activated(int)), this, SLOT(onValueChange(int)));
	if (selectedIndex >= 0) {
		myComboBox->setCurrentItem(selectedIndex);
	}
	int width = myToColumn - myFromColumn + 1;
	myTab->addItem(myLabel, myRow, myFromColumn, myFromColumn + width / 2 - 1);
	myTab->addItem(myComboBox, myRow, myToColumn - width / 2 + 1, myToColumn);
}

void ComboOptionView::_show() {
	myLabel->show();
	myComboBox->show();
}

void ComboOptionView::_hide() {
	myLabel->hide();
	myComboBox->hide();
}

void ComboOptionView::_setActive(bool active) {
	myComboBox->setEnabled(active);
}

void ComboOptionView::_onAccept() const {
	((ZLComboOptionEntry*)myOption)->onAccept(myComboBox->currentText().ascii());
}

void ComboOptionView::onValueChange(int index) {
	ZLComboOptionEntry *o = (ZLComboOptionEntry*)myOption;
	if ((index >= 0) && (index < (int)o->values().size())) {
		o->onValueChange(o->values()[index]);
	}
}

void SpinOptionView::_createItem() {
	myLabel = new QLabel(myOption->name().c_str(), myTab);
	mySpinBox = new QSpinBox(
		((ZLSpinOptionEntry*)myOption)->minValue(),
		((ZLSpinOptionEntry*)myOption)->maxValue(),
		((ZLSpinOptionEntry*)myOption)->step(), myTab
	);
	mySpinBox->setValue(((ZLSpinOptionEntry*)myOption)->initialValue());
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
	((ZLSpinOptionEntry*)myOption)->onAccept(mySpinBox->value());
}

void StringOptionView::_createItem() {
	myLabel = new QLabel(myOption->name().c_str(), myTab);
	myLineEdit = new QLineEdit(myTab);
	myLineEdit->setText(QString::fromUtf8(((ZLStringOptionEntry*)myOption)->initialValue().c_str()));
	myLineEdit->cursorLeft(false, myLineEdit->text().length());
	int width = myToColumn - myFromColumn + 1;
	myTab->addItem(myLabel, myRow, myFromColumn, myFromColumn + width / 4 - 1);
	myTab->addItem(myLineEdit, myRow, myFromColumn + width / 4, myToColumn);
}

void StringOptionView::_show() {
	myLabel->show();
	myLineEdit->show();
}

void StringOptionView::_hide() {
	myLabel->hide();
	myLineEdit->hide();
}

void StringOptionView::_setActive(bool active) {
	myLineEdit->setReadOnly(!active);
}

void StringOptionView::_onAccept() const {
	((ZLStringOptionEntry*)myOption)->onAccept((const char*)myLineEdit->text().utf8());
}

class KeyButton : public QPushButton {

public:
	KeyButton(KeyOptionView &keyView);

protected:
	void focusInEvent(QFocusEvent*);
	void focusOutEvent(QFocusEvent*);
	void mousePressEvent(QMouseEvent *);
	void keyPressEvent(QKeyEvent *keyEvent);

private:
	KeyOptionView &myKeyView;
};

KeyButton::KeyButton(KeyOptionView &keyView) : QPushButton(keyView.myWidget), myKeyView(keyView) {
	focusOutEvent(0);
}

void KeyButton::focusInEvent(QFocusEvent*) {
	setText("Press key to set action");
	grabKeyboard();
}

void KeyButton::focusOutEvent(QFocusEvent*) {
	releaseKeyboard();
	setText("Press this button to select key");
}

void KeyButton::mousePressEvent(QMouseEvent*) {
	setFocus();
}

void KeyButton::keyPressEvent(QKeyEvent *keyEvent) {
	std::string keyText = QKeyUtil::keyName(keyEvent);
	if (!keyText.empty()) {
		myKeyView.myCurrentKey = keyText;
		myKeyView.myLabel->setText("Action For " + QString::fromUtf8(keyText.c_str()));
		myKeyView.myLabel->show();
		myKeyView.myComboBox->setCurrentItem(((ZLKeyOptionEntry*)myKeyView.myOption)->actionIndex(keyText));
		myKeyView.myComboBox->show();
	}
}

void KeyOptionView::_createItem() {
	myWidget = new QWidget(myTab);
	QGridLayout *layout = new QGridLayout(myWidget, 2, 2, 0, 10);
	myKeyButton = new KeyButton(*this);
	layout->addMultiCellWidget(myKeyButton, 0, 0, 0, 1);
	myLabel = new QLabel(myWidget);
	myLabel->setTextFormat(QObject::PlainText);
	layout->addWidget(myLabel, 1, 0);
	myComboBox = new QComboBox(myWidget);
	const std::vector<std::string> &actions = ((ZLKeyOptionEntry*)myOption)->actionNames();
	for (std::vector<std::string>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
		myComboBox->insertItem(it->c_str());
	}
	connect(myComboBox, SIGNAL(activated(int)), this, SLOT(onValueChange(int)));
	layout->addWidget(myComboBox, 1, 1);
	myTab->addItem(myWidget, myRow, myFromColumn, myToColumn);
}

void KeyOptionView::_show() {
	myWidget->show();
	myKeyButton->show();
	if (!myCurrentKey.empty()) {
		myLabel->show();
		myComboBox->show();
	} else {
		myLabel->hide();
		myComboBox->hide();
	}
}

void KeyOptionView::_hide() {
	myKeyButton->hide();
	myWidget->hide();
	myLabel->hide();
	myComboBox->hide();
	myCurrentKey.erase();
}

void KeyOptionView::_onAccept() const {
	((ZLKeyOptionEntry*)myOption)->onAccept();
}

void KeyOptionView::onValueChange(int index) {
	if (!myCurrentKey.empty()) {
		((ZLKeyOptionEntry*)myOption)->onValueChange(myCurrentKey, index);
	}
}

void ColorOptionView::_createItem() {
	myWidget = new QWidget(myTab);
	QGridLayout *layout = new QGridLayout(myWidget, 3, 3, 0, 10);
	layout->addWidget(new QLabel("Red", myWidget), 0, 0);
	layout->addWidget(new QLabel("Green", myWidget), 1, 0);
	layout->addWidget(new QLabel("Blue", myWidget), 2, 0);
	const ZLColor &color = ((ZLColorOptionEntry*)myOption)->color();
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
	((ZLColorOptionEntry*)myOption)->onAccept(ZLColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
}
