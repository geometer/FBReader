/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTOPTIONVIEW_H__
#define __ZLQTOPTIONVIEW_H__

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtGui/QColor>
#include <QtCore/QVariant>

#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include "../../../../core/src/dialogs/ZLOptionView.h"
#include "../../../../core/src/dialogs/ZLOptionEntry.h"

class ZLQmlDialogContent;

class QLabel;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QGroupBox;
class QRadioButton;
class QComboBox;
class QSlider;
class QWidget;
class QGridLayout;

class ZLQmlOptionView : public QObject, public ZLOptionView {
	Q_OBJECT
	Q_ENUMS(Type)
	Q_PROPERTY(QString name READ name CONSTANT)
	Q_PROPERTY(Type type READ type CONSTANT)
	Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
	Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged)
	Q_PROPERTY(bool created READ created NOTIFY createdChanged)

public:
	enum Type {
		Choice = ZLOptionEntry::CHOICE,
		Boolean = ZLOptionEntry::BOOLEAN,
		Boolean3 = ZLOptionEntry::BOOLEAN3,
		String = ZLOptionEntry::STRING,
		Password = ZLOptionEntry::PASSWORD,
		Spin = ZLOptionEntry::SPIN,
		Combo = ZLOptionEntry::COMBO,
		Color = ZLOptionEntry::COLOR,
		Key = ZLOptionEntry::KEY,
		Order = ZLOptionEntry::ORDER,
		Multiline = ZLOptionEntry::MULTILINE,
		Static = ZLOptionEntry::STATIC,
		Path = ZLOptionEntry::PATH
	};
	
	ZLQmlOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	virtual ~ZLQmlOptionView();
	
	Type type() const;
	QString name() const;
	bool visible() const;
	bool enabled() const;
	bool created() const;
	
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	
Q_SIGNALS:
	void visibleChanged(bool visible);
	void enabledChanged(bool enabled);
	void createdChanged(bool created);

protected:
	bool myVisible;
	bool myEnabled;
	bool myCreated;
};

class ZLQmlChoiceOptionView : public ZLQmlOptionView {
	Q_OBJECT
	Q_PROPERTY(QStringList options READ options CONSTANT)
	Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
public:
	ZLQmlChoiceOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	~ZLQmlChoiceOptionView();
	
	QStringList options() const;
	int currentIndex() const;
	void setCurrentIndex(int index);

Q_SIGNALS:
	void currentIndexChanged(int currentIndex);

protected:
	void _onAccept() const;
	void reset();

private:
	QStringList myOptions;
	int myCurrentIndex;
};

class ZLQmlBooleanOptionView : public ZLQmlOptionView {
	Q_OBJECT
	Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)

public:
	ZLQmlBooleanOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	~ZLQmlBooleanOptionView();
	
	bool isChecked();
	void setChecked(bool checked);

protected:
	void _onAccept() const;
	void reset();

Q_SIGNALS:
	void checkedChanged(bool checked);

private:
	bool myChecked;
};

class ZLQmlBoolean3OptionView : public ZLQmlOptionView {
	Q_OBJECT
	Q_ENUMS(State)
	Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
public:
	enum State {
		Unchecked = B3_FALSE,
        PartiallyChecked = B3_UNDEFINED,
        Checked = B3_TRUE
	};

	ZLQmlBoolean3OptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	~ZLQmlBoolean3OptionView();
	
	State state() const;
	void setState(State state);

protected:
	void _onAccept() const;
	void reset();

Q_SIGNALS:
	void stateChanged(ZLQmlBoolean3OptionView::State state);

private:
	State myState;
};

class ZLQmlSpinOptionView : public ZLQmlOptionView {
	Q_OBJECT
	Q_PROPERTY(int minimumValue READ minimumValue CONSTANT)
	Q_PROPERTY(int maximumValue READ maximumValue CONSTANT)
	Q_PROPERTY(int stepSize READ stepSize CONSTANT)
	Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
public:
	ZLQmlSpinOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	~ZLQmlSpinOptionView();
	
	int minimumValue() const;
	int maximumValue() const;
	int stepSize() const;
	int value();
	void setValue(int value);

protected:
	void _onAccept() const;
	void reset();
	
Q_SIGNALS:
	void valueChanged(int value);

private:
	int myValue;
};

class ZLQmlComboOptionView : public ZLQmlOptionView {
	Q_OBJECT
	Q_PROPERTY(QStringList values READ values CONSTANT)
	Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
	Q_PROPERTY(QString currentText READ currentText WRITE setCurrentText NOTIFY currentTextChanged)
	Q_PROPERTY(bool editable READ isEditable CONSTANT)
public:
	ZLQmlComboOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	~ZLQmlComboOptionView();
	
	QStringList values() const;
	bool isEditable() const;
	int currentIndex() const;
	void setCurrentIndex(int index);
	QString currentText() const;
	void setCurrentText(const QString &text);

private:
	void _onAccept() const;
	void reset();
	
Q_SIGNALS:
	void currentIndexChanged(int currentIndex);
	void currentTextChanged(const QString &currentText);
	
private:
	QStringList myValues;
	QString myCurrentText;
	int myCurrentIndex;
};

class ZLQmlTextOptionView : public ZLQmlOptionView {
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(bool multiline READ isMultiline CONSTANT)
	Q_PROPERTY(bool password READ isPassword CONSTANT)
	Q_PROPERTY(QString pathDelimiter READ pathDelimiter CONSTANT)
public:
	ZLQmlTextOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	~ZLQmlTextOptionView();
	
	QString text() const;
	void setText(const QString &text);
	bool isMultiline() const;
	bool isPassword() const;
	QString pathDelimiter() const;
	
private:
	void _onAccept() const;
	void reset();

Q_SIGNALS:
	void textChanged(const QString &text);

private:
	QString myText;
};

class ZLQmlColorOptionView : public ZLQmlOptionView {
	Q_OBJECT
	Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
	ZLQmlColorOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	~ZLQmlColorOptionView();
	
	QColor color() const;
	void setColor(const QColor &color);
	Q_INVOKABLE QVariant parsedColor() const;
	
private:
	void _onAccept() const;
	void reset();
	
Q_SIGNALS:
	void colorChanged(const QColor &color);

private:
	QColor myColor;
};

//class KeyOptionView : public QObject, public ZLQmlOptionView {

//Q_OBJECT

//public:
//	KeyOptionView(const std::string &name, const std::string &tooltip, ZLKeyOptionEntry *option, ZLQmlDialogContent *tab, int row, int fromColumn, int toColumn) : ZLQmlOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myComboBox(0), myKeyEditor(0) {}

//private:
//	void _createItem();
//	void _show();
//	void _hide();
//	void _onAccept() const;
//	void reset();

//private Q_SLOTS:
//	void onValueChanged(int);

//private:
//	QComboBox *myComboBox;
//	QLineEdit *myKeyEditor;
//	std::string myCurrentKey;

//friend class KeyLineEdit;
//};

class ZLQmlStaticTextOptionView : public ZLQmlOptionView {
	Q_OBJECT
	Q_PROPERTY(QString text READ text CONSTANT)
public:
	ZLQmlStaticTextOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option);
	~ZLQmlStaticTextOptionView();
	
	QString text() const;
	
private:
	void _onAccept() const;

private:
	QString myText;
};

#endif /* __ZLQTOPTIONVIEW_H__ */
