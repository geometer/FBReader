/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <QtCore/QVariantMap>

#include <ZLStringUtil.h>
#include <ZLDialogManager.h>
#include "../util/ZLQtKeyUtil.h"

#include "ZLQmlOptionView.h"
#include "ZLQmlDialogContent.h"
#include "ZLQtUtil.h"

ZLQmlOptionView::ZLQmlOptionView(const std::string &name, const std::string &tooltip,
                                 shared_ptr<ZLOptionEntry> option, QObject *parent)
    : QObject(parent), ZLOptionView(name, tooltip, option) {
	myVisible = false;
	myEnabled = true;
	myCreated = false;
}

ZLQmlOptionView::~ZLQmlOptionView() {
}

ZLQmlOptionView::Type ZLQmlOptionView::type() const {
	return static_cast<Type>(myOption->kind());
}

QString ZLQmlOptionView::name() const {
	return QString::fromStdString(myName);
}

bool ZLQmlOptionView::visible() const {
	return myVisible;
}

bool ZLQmlOptionView::enabled() const {
	return myEnabled;
}

bool ZLQmlOptionView::created() const {
	return myCreated;
}

void ZLQmlOptionView::_createItem() {
	// Do nothing, items are created through QML
	if (myCreated)
		return;
	myCreated = true;
	emit createdChanged(myCreated);
}

void ZLQmlOptionView::_show() {
	if (myVisible)
		return;
	myVisible = true;
	emit visibleChanged(myVisible);
}

void ZLQmlOptionView::_hide() {
	if (!myVisible)
		return;
	myVisible = false;
	emit visibleChanged(myVisible);
}

void ZLQmlOptionView::_setActive(bool active) {
	if (active == myEnabled)
		return;
	myEnabled = active;
	emit enabledChanged(myEnabled);
}

ZLQmlChoiceOptionView::ZLQmlChoiceOptionView(const std::string &name, const std::string &tooltip,
                                             shared_ptr<ZLOptionEntry> option, QObject *parent)
    : ZLQmlOptionView(name, tooltip, option, parent) {
	ZLChoiceOptionEntry *entry = static_cast<ZLChoiceOptionEntry*>(&*myOption);
	for (int i = 0; i < entry->choiceNumber(); ++i)
		myOptions << QString::fromStdString(entry->text(i));
	myCurrentIndex = entry->initialCheckedIndex();
}

ZLQmlChoiceOptionView::~ZLQmlChoiceOptionView() {
}

QStringList ZLQmlChoiceOptionView::options() const {
	return myOptions;
}

int ZLQmlChoiceOptionView::currentIndex() const {
	return myCurrentIndex;
}

void ZLQmlChoiceOptionView::setCurrentIndex(int index) {
	if (index == myCurrentIndex)
		return;
	myCurrentIndex = index;
	emit currentIndexChanged(myCurrentIndex);
}

void ZLQmlChoiceOptionView::_onAccept() const {
	static_cast<ZLChoiceOptionEntry*>(&*myOption)->onAccept(myCurrentIndex);
}

void ZLQmlChoiceOptionView::reset() {
	ZLChoiceOptionEntry *entry = static_cast<ZLChoiceOptionEntry*>(&*myOption);
	setCurrentIndex(entry->initialCheckedIndex());
}

ZLQmlBooleanOptionView::ZLQmlBooleanOptionView(const std::string &name, const std::string &tooltip,
                                               shared_ptr<ZLOptionEntry> option, QObject *parent)
    : ZLQmlOptionView(name, tooltip, option, parent) {
	ZLBooleanOptionEntry *entry = static_cast<ZLBooleanOptionEntry*>(&*myOption);
	myChecked = entry->initialState();
}

ZLQmlBooleanOptionView::~ZLQmlBooleanOptionView() {
}

bool ZLQmlBooleanOptionView::isChecked() {
	return myChecked;
}

void ZLQmlBooleanOptionView::setChecked(bool checked) {
	if (myChecked == checked)
		return;
	ZLBooleanOptionEntry *entry = static_cast<ZLBooleanOptionEntry*>(&*myOption);
	myChecked = checked;
	entry->onStateChanged(myChecked);
	emit checkedChanged(myChecked);
}

void ZLQmlBooleanOptionView::_onAccept() const {
	static_cast<ZLBooleanOptionEntry*>(&*myOption)->onAccept(myChecked);
}

void ZLQmlBooleanOptionView::reset() {
	ZLBooleanOptionEntry *entry = static_cast<ZLBooleanOptionEntry*>(&*myOption);
	setChecked(entry->initialState());
}

ZLQmlBoolean3OptionView::ZLQmlBoolean3OptionView(const std::string &name, const std::string &tooltip,
                                                 shared_ptr<ZLOptionEntry> option, QObject *parent)
    : ZLQmlOptionView(name, tooltip, option, parent) {
	ZLBoolean3OptionEntry *entry = static_cast<ZLBoolean3OptionEntry*>(&*myOption);
	myState = static_cast<State>(entry->initialState());
}

ZLQmlBoolean3OptionView::~ZLQmlBoolean3OptionView() {
}

ZLQmlBoolean3OptionView::State ZLQmlBoolean3OptionView::state() const {
	return myState;
}

void ZLQmlBoolean3OptionView::setState(ZLQmlBoolean3OptionView::State state) {
	if (myState == state)
		return;
	ZLBoolean3OptionEntry *entry = static_cast<ZLBoolean3OptionEntry*>(&*myOption);
	myState = state;
	entry->onStateChanged(static_cast<ZLBoolean3>(myState));
	emit stateChanged(myState);
}

void ZLQmlBoolean3OptionView::_onAccept() const {
	ZLBoolean3OptionEntry *entry = static_cast<ZLBoolean3OptionEntry*>(&*myOption);
	entry->onAccept(static_cast<ZLBoolean3>(myState));
}

void ZLQmlBoolean3OptionView::reset() {
	ZLBoolean3OptionEntry *entry = static_cast<ZLBoolean3OptionEntry*>(&*myOption);
	setState(static_cast<State>(entry->initialState()));
}

ZLQmlSpinOptionView::ZLQmlSpinOptionView(const std::string &name, const std::string &tooltip,
                                         shared_ptr<ZLOptionEntry> option, QObject *parent)
    : ZLQmlOptionView(name, tooltip, option, parent) {
	ZLSpinOptionEntry *entry = static_cast<ZLSpinOptionEntry*>(&*myOption);
	myValue = entry->initialValue();
}

ZLQmlSpinOptionView::~ZLQmlSpinOptionView() {
}

int ZLQmlSpinOptionView::minimumValue() const {
	return static_cast<ZLSpinOptionEntry*>(&*myOption)->minValue();
}

int ZLQmlSpinOptionView::maximumValue() const {
	return static_cast<ZLSpinOptionEntry*>(&*myOption)->maxValue();
}

int ZLQmlSpinOptionView::stepSize() const {
	return static_cast<ZLSpinOptionEntry*>(&*myOption)->step();
}

int ZLQmlSpinOptionView::value() {
	return myValue;
}

void ZLQmlSpinOptionView::setValue(int value) {
	if (value == myValue)
		return;
	myValue = value;
	emit valueChanged(myValue);
}

void ZLQmlSpinOptionView::_onAccept() const {
	static_cast<ZLSpinOptionEntry*>(&*myOption)->onAccept(myValue);
}

void ZLQmlSpinOptionView::reset() {
	ZLSpinOptionEntry *entry = static_cast<ZLSpinOptionEntry*>(&*myOption);
	setValue(entry->initialValue());
}

ZLQmlComboOptionView::ZLQmlComboOptionView(const std::string &name, const std::string &tooltip,
                                           shared_ptr<ZLOptionEntry> option, QObject *parent)
      : ZLQmlOptionView(name, tooltip, option, parent) {
	ZLComboOptionEntry *entry = static_cast<ZLComboOptionEntry*>(&*myOption);
	foreach (const std::string &str, entry->values())
		myValues << QString::fromStdString(str);
	myCurrentText = QString::fromStdString(entry->initialValue());
	myCurrentIndex = myValues.indexOf(myCurrentText);
}

ZLQmlComboOptionView::~ZLQmlComboOptionView() {
}

QStringList ZLQmlComboOptionView::values() const {
	return myValues;
}

bool ZLQmlComboOptionView::isEditable() const {
	ZLComboOptionEntry *entry = static_cast<ZLComboOptionEntry*>(&*myOption);
	return entry->isEditable();
}

int ZLQmlComboOptionView::currentIndex() const {
	return myCurrentIndex;
}

void ZLQmlComboOptionView::setCurrentIndex(int index) {
	if (myCurrentIndex != index)
		setCurrentText(myValues.value(index));
}

QString ZLQmlComboOptionView::currentText() const {
	return myCurrentText;
}

void ZLQmlComboOptionView::setCurrentText(const QString &text) {
	if (text == myCurrentText)
		return;
	ZLComboOptionEntry *entry = static_cast<ZLComboOptionEntry*>(&*myOption);
	myCurrentText = text;
	int index = myValues.indexOf(text);
	if (index != -1)
		entry->onValueSelected(index);
	else
		entry->onValueEdited(myCurrentText.toStdString());
	if (index != myCurrentIndex) {
		myCurrentIndex = index;
		emit currentIndexChanged(index);
	}
	emit currentTextChanged(text);
}
void ZLQmlComboOptionView::_onAccept() const {
	ZLComboOptionEntry *entry = static_cast<ZLComboOptionEntry*>(&*myOption);
	entry->onAccept(myCurrentText.toStdString());
}

void ZLQmlComboOptionView::reset() {
	// May be we should refill values
	ZLComboOptionEntry *entry = static_cast<ZLComboOptionEntry*>(&*myOption);
	setCurrentText(QString::fromStdString(entry->initialValue()));
}

ZLQmlTextOptionView::ZLQmlTextOptionView(const std::string &name, const std::string &tooltip,
                                         shared_ptr<ZLOptionEntry> option, QObject *parent)
    : ZLQmlOptionView(name, tooltip, option, parent) {
  ZLComboOptionEntry *entry = static_cast<ZLComboOptionEntry*>(&*myOption);
  myText = QString::fromStdString(entry->initialValue());
}

ZLQmlTextOptionView::~ZLQmlTextOptionView() {
}

QString ZLQmlTextOptionView::text() const {
	return myText;
}

void ZLQmlTextOptionView::setText(const QString &text) {
	if (myText == text)
		return;
	ZLTextOptionEntry *entry = static_cast<ZLTextOptionEntry*>(&*myOption);
	myText = text;
	entry->onValueEdited(myText.toStdString());
	emit textChanged(myText);
}

bool ZLQmlTextOptionView::isMultiline() const {
	return myOption->kind() == ZLOptionEntry::MULTILINE;
}

bool ZLQmlTextOptionView::isPassword() const {
	return myOption->kind() == ZLOptionEntry::PASSWORD;
}

void ZLQmlTextOptionView::_onAccept() const {
	ZLTextOptionEntry *entry = static_cast<ZLTextOptionEntry*>(&*myOption);
	entry->onAccept(myText.toStdString());
}

void ZLQmlTextOptionView::reset() {
	ZLTextOptionEntry *entry = static_cast<ZLTextOptionEntry*>(&*myOption);
	setText(QString::fromStdString(entry->initialValue()));
}

ZLQmlColorOptionView::ZLQmlColorOptionView(const std::string &name, const std::string &tooltip,
                                           shared_ptr<ZLOptionEntry> option, QObject *parent)
    : ZLQmlOptionView(name, tooltip, option, parent) {
    ZLColorOptionEntry *entry = static_cast<ZLColorOptionEntry*>(&*myOption);
	myColor = qtColor(entry->initialColor());
}

ZLQmlColorOptionView::~ZLQmlColorOptionView() {
}

QColor ZLQmlColorOptionView::color() const {
	return myColor;
}

void ZLQmlColorOptionView::setColor(const QColor &color) {
	if (color == myColor)
		return;
	myColor = color;
	emit colorChanged(myColor);
}

QVariant ZLQmlColorOptionView::parsedColor() const {
	QVariantMap result;
	result.insert(QLatin1String("red"), myColor.redF());
	result.insert(QLatin1String("green"), myColor.greenF());
	result.insert(QLatin1String("blue"), myColor.blueF());
	return result;
}

void ZLQmlColorOptionView::_onAccept() const {
    ZLColorOptionEntry *entry = static_cast<ZLColorOptionEntry*>(&*myOption);
	entry->onAccept(ZLColor(myColor.red(), myColor.green(), myColor.blue()));
}

void ZLQmlColorOptionView::reset() {
    ZLColorOptionEntry *entry = static_cast<ZLColorOptionEntry*>(&*myOption);
	entry->onReset(ZLColor(myColor.red(), myColor.green(), myColor.blue()));
	setColor(qtColor(entry->color()));
}

//class KeyLineEdit : public QLineEdit {

//public:
//	KeyLineEdit(KeyOptionView &keyView, QWidget *parent);

//protected:
//	void focusInEvent(QFocusEvent*);
//	void focusOutEvent(QFocusEvent*);
//	void keyPressEvent(QKeyEvent *keyEvent);

//private:
//	KeyOptionView &myKeyView;
//};

//KeyLineEdit::KeyLineEdit(KeyOptionView &keyView, QWidget *parent) : QLineEdit(parent), myKeyView(keyView) {
//	focusOutEvent(0);
//}

//void KeyLineEdit::keyPressEvent(QKeyEvent *keyEvent) {
//	std::string keyText = ZLQtKeyUtil::keyName(keyEvent);
//	setText(keyText.c_str());
//	if (!keyText.empty()) {
//		myKeyView.myCurrentKey = keyText;
//		myKeyView.myComboBox->setCurrentIndex(((ZLKeyOptionEntry&)*myKeyView.myOption).actionIndex(keyText));
//		myKeyView.myComboBox->show();
//	}
//	((ZLKeyOptionEntry&)*myKeyView.myOption).onKeySelected(keyText);
//}

//void KeyLineEdit::focusInEvent(QFocusEvent*) {
//	grabKeyboard();
//}

//void KeyLineEdit::focusOutEvent(QFocusEvent*) {
//	releaseKeyboard();
//}

//void KeyOptionView::_createItem() {
//	QWidget *widget = new QWidget(myTab->widget());
//	QGridLayout *layout = new QGridLayout(widget);

//	QLabel *label = new QLabel(widget);
//	label->setText(QString::fromStdString(ZLResource::resource("keyOptionView")["actionFor"].value()));
//	layout->addWidget(label, 0, 0);

//	myKeyEditor = new KeyLineEdit(*this, widget);
//	layout->addWidget(myKeyEditor, 0, 1);

//	myWidgets.push_back(widget);
//	myWidgets.push_back(label);
//	myWidgets.push_back(myKeyEditor);

//	myComboBox = new QComboBox(widget);
//	const std::vector<std::string> &actions = ((ZLKeyOptionEntry&)*myOption).actionNames();
//	for (std::vector<std::string>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
//		myComboBox->insertItem(it - actions.begin(), QString::fromStdString(*it));
//	}
//	connect(myComboBox, SIGNAL(activated(int)), this, SLOT(onValueChanged(int)));
//	layout->addWidget(myComboBox, 1, 0, 1, 2);
//	myTab->addItem(widget, myRow, myFromColumn, myToColumn);
//}

//void KeyOptionView::reset() {
//	if (myKeyEditor == 0) {
//		return;
//	}
//	myCurrentKey.erase();
//	myKeyEditor->setText("");
//	((ZLKeyOptionEntry&)*myOption).onKeySelected(myCurrentKey);
//	myComboBox->hide();
//}

//void KeyOptionView::_show() {
//	ZLQmlOptionView::_show();
//	if (!myCurrentKey.empty()) {
//		myComboBox->show();
//	} else {
//		myComboBox->hide();
//	}
//}

//void KeyOptionView::_hide() {
//	ZLQmlOptionView::_hide();
//	myComboBox->hide();
//	myCurrentKey.erase();
//	myKeyEditor->setText("");
//	((ZLKeyOptionEntry&)*myOption).onKeySelected(myCurrentKey);
//}

//void KeyOptionView::_onAccept() const {
//	((ZLKeyOptionEntry&)*myOption).onAccept();
//}

//void KeyOptionView::onValueChanged(int index) {
//	if (!myCurrentKey.empty()) {
//		((ZLKeyOptionEntry&)*myOption).onValueChanged(myCurrentKey, index);
//	}
//}

ZLQmlStaticTextOptionView::ZLQmlStaticTextOptionView(const std::string &name, const std::string &tooltip,
                                                     shared_ptr<ZLOptionEntry> option, QObject *parent)
    : ZLQmlOptionView(name, tooltip, option, parent) {
	ZLStaticTextOptionEntry *entry = static_cast<ZLStaticTextOptionEntry*>(&*myOption);
	myText = QString::fromStdString(entry->initialValue());
}

ZLQmlStaticTextOptionView::~ZLQmlStaticTextOptionView() {
}

QString ZLQmlStaticTextOptionView::text() const {
	return myText;
}

void ZLQmlStaticTextOptionView::_onAccept() const {
}

