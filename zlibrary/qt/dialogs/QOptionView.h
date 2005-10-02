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

#ifndef __QOPTIONVIEW_H__
#define __QOPTIONVIEW_H__

#include <qobject.h>

#include "../../abstract/dialogs/ZLOptionsDialog.h"
#include "../../abstract/dialogs/ZLOptionEntry.h"

class QOptionsDialogTab;

class QButtonGroup;
class QLabel;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QComboBox;
class QSlider;
class QWidget;

class QOptionView : public OptionView {

protected:
	QOptionView(ZLOptionEntry *option, QOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : OptionView(option), myTab(tab), myRow(row), myFromColumn(fromColumn), myToColumn(toColumn), myInitialized(false) {}
	QOptionView(ZLOptionEntry *option) : OptionView(option), myInitialized(false) {}

public:
	virtual ~QOptionView() {}
	void setPosition(QOptionsDialogTab *tab, int row, int fromColumn, int toColumn) {
		myTab = tab;
		myRow = row;
		myFromColumn = fromColumn;
		myToColumn = toColumn;
	}

	void setVisible(bool visible) {
		if (visible) {
			if (!myInitialized) _createItem(); myInitialized = true; setActive(myOption->isActive()); _show();
		} else {
			if (myInitialized) _hide();
		}
	}
	void setActive(bool active) {
		if (myInitialized) {
			_setActive(active);
		}
	}
	void onAccept() const { if (myInitialized) _onAccept(); }

protected:
	virtual void _createItem() = 0;
	virtual void _hide() = 0;
	virtual void _show() = 0;
	// TODO: replace by pure virtual method
	virtual void _setActive(bool active) {}
	virtual void _onAccept() const = 0;

protected:
	QOptionsDialogTab *myTab;
	int myRow, myFromColumn, myToColumn;

private:
	bool myInitialized;
};

class ChoiceOptionView : public QOptionView {

public:
	ChoiceOptionView(ZLChoiceOptionEntry *option, QOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : QOptionView(option, tab, row, fromColumn, toColumn) {
		myButtons = 0;
	}
	~ChoiceOptionView() { if (myButtons != 0) delete[] myButtons; }

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private:
	QButtonGroup *myGroup;
	QRadioButton **myButtons;
};

class BooleanOptionView : public QObject, public QOptionView {

Q_OBJECT

public:
	BooleanOptionView(ZLBooleanOptionEntry *option, QOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : QOptionView(option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private slots:
	void onValueChange(bool) const;

private:
	QCheckBox *myCheckBox;
};

class StringOptionView : public QOptionView {

public:
	StringOptionView(ZLStringOptionEntry *option, QOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : QOptionView(option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;

private:
	QLabel *myLabel;
	QLineEdit *myLineEdit;
};

class SpinOptionView : public QOptionView {

public:
	SpinOptionView(ZLSpinOptionEntry *option, QOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : QOptionView(option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private:
	QLabel *myLabel;
	QSpinBox *mySpinBox;
};

class ComboOptionView : public QObject, public QOptionView {

Q_OBJECT

public:
	ComboOptionView(ZLComboOptionEntry *option, QOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : QOptionView(option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;

private slots:
	void onValueChange(int);
	
private:
	QLabel *myLabel;
	QComboBox *myComboBox;
};

class KeyOptionView : public QObject, public QOptionView {

Q_OBJECT

public:
	KeyOptionView(ZLKeyOptionEntry *option, QOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : QOptionView(option, tab, row, fromColumn, toColumn) {}
	virtual ~KeyOptionView() {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private:
	QWidget *myWidget;
	QPushButton *myKeyButton;
	QLabel *myLabel;
	QComboBox *myComboBox;
	std::string myCurrentKey;

friend class KeyButton;
};

class ColorOptionView : public QObject, public QOptionView {

Q_OBJECT

public:
	ColorOptionView(ZLColorOptionEntry *option, QOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : QOptionView(option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private slots:
	void onSliderMove(int);

private:
	QWidget *myWidget;
	QSlider *myRSlider, *myGSlider, *myBSlider;
	QLabel *myColorBar;
};

#endif /* __QOPTIONVIEW_H__ */
