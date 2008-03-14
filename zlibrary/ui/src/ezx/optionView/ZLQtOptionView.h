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

#ifndef __ZLQTOPTIONVIEW_H__
#define __ZLQTOPTIONVIEW_H__

#include <qobject.h>

#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include "../../../../core/src/dialogs/ZLOptionView.h"

class ZLQtOptionViewHolder;

class QButtonGroup;
class QLabel;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QMultiLineEdit;
class QPushButton;
class QRadioButton;
class QComboBox;
class QSlider;
class QWidget;

class ZLQtOptionView : public ZLOptionView {

protected:
	ZLQtOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLOptionView(name, tooltip, option), myHolder(holder) {}

protected:
	ZLQtOptionViewHolder &myHolder;
};

class ChoiceOptionView : public ZLQtOptionView {

public:
	ChoiceOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLQtOptionView(name, tooltip, option, holder) {
		myButtons = 0;
	}
	~ChoiceOptionView() { if (myButtons != 0) delete[] myButtons; }

protected:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;

private:
	QButtonGroup *myGroup;
	QRadioButton **myButtons;
};

class BooleanOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	BooleanOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLQtOptionView(name, tooltip, option, holder) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;

private slots:
	void onStateChanged(bool) const;

private:
	QCheckBox *myCheckBox;
};

class Boolean3OptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	Boolean3OptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLQtOptionView(name, tooltip, option, holder) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;

private slots:
	void onStateChanged(int) const;

private:
	QCheckBox *myCheckBox;
};

class StringOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	StringOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLQtOptionView(name, tooltip, option, holder) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();

private slots:
	void onValueEdited(const QString &value);

private:
	QLabel *myLabel;
	QLineEdit *myLineEdit;
};

class MultilineOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	MultilineOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLQtOptionView(name, tooltip, option, holder) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();

private slots:
	void onValueEdited();

private:
	QMultiLineEdit *myMultiLineEdit;
};

class SpinOptionView : public ZLQtOptionView {

public:
	SpinOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLQtOptionView(name, tooltip, option, holder) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private:
	QLabel *myLabel;
	QSpinBox *mySpinBox;
};

class ComboOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	ComboOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLQtOptionView(name, tooltip, option, holder), myLabel(0), myComboBox(0) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();

private slots:
	void onValueSelected(int index);
	void onValueEdited(const QString &value);
	void onTabResized(const QSize &size);
	
private:
	QLabel *myLabel;
	QComboBox *myComboBox;
};

class KeyOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	KeyOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLQtOptionView(name, tooltip, option, holder) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;
	void reset();

private slots:
	void onValueChanged(int);

private:
	QWidget *myWidget;
	QLineEdit *myKeyEditor;
	QLabel *myLabel;
	QComboBox *myComboBox;
	std::string myCurrentKey;

friend class KeyLineEdit;
};

class ColorOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	ColorOptionView(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, ZLQtOptionViewHolder &holder) : ZLQtOptionView(name, tooltip, option, holder) {}
	~ColorOptionView();

private:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;
	void reset();

private slots:
	void onSliderMove(int);

private:
	QWidget *myWidget;
	QSlider *myRSlider, *myGSlider, *myBSlider;
	QLabel *myColorBar;
	QPixmap *myPixmap;
};

#endif /* __ZLQTOPTIONVIEW_H__ */
