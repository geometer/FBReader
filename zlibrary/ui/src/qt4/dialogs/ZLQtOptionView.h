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

#include <QtCore/QObject>
#include <QtCore/QSize>

#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include "../../../../core/src/dialogs/ZLOptionView.h"

class ZLQtDialogContent;

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

class ZLQtOptionView : public ZLOptionView {

protected:
	ZLQtOptionView(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, ZLQtDialogContent *tab, int row, int fromColumn, int toColumn) : ZLOptionView(name, tooltip, option), myTab(tab), myRow(row), myFromColumn(fromColumn), myToColumn(toColumn) {}

	void _show();
	void _hide();

protected:
	ZLQtDialogContent *myTab;
	int myRow, myFromColumn, myToColumn;
	std::vector<QWidget*> myWidgets;
};

class ChoiceOptionView : public ZLQtOptionView {

public:
	ChoiceOptionView(const std::string &name, const std::string &tooltip, ZLChoiceOptionEntry *option, ZLQtDialogContent *tab, int row, int fromColumn, int toColumn) : ZLQtOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myButtons(0) {}
	~ChoiceOptionView() { if (myButtons != 0) delete[] myButtons; }

protected:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;

private:
	QGroupBox *myGroupBox;
	QRadioButton **myButtons;
};

class BooleanOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	BooleanOptionView(const std::string &name, const std::string &tooltip, ZLBooleanOptionEntry *option, ZLQtDialogContent *tab, int row, int fromColumn, int toColumn) : ZLQtOptionView(name, tooltip, option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;

private Q_SLOTS:
	void onStateChanged(bool) const;

private:
	QCheckBox *myCheckBox;
};

class Boolean3OptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	Boolean3OptionView(const std::string &name, const std::string &tooltip, ZLBoolean3OptionEntry *option, ZLQtDialogContent *tab, int row, int fromColumn, int toColumn) : ZLQtOptionView(name, tooltip, option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;

private Q_SLOTS:
	void onStateChanged(int) const;

private:
	QCheckBox *myCheckBox;
};

class StringOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	StringOptionView(const std::string &name, const std::string &tooltip, ZLStringOptionEntry *option, ZLQtDialogContent *tab, int row, int fromColumn, int toColumn) : ZLQtOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myLineEdit(0) {}

private:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();

private Q_SLOTS:
	void onValueEdited(const QString &value);

private:
	QLineEdit *myLineEdit;
};

class SpinOptionView : public ZLQtOptionView {

public:
	SpinOptionView(const std::string &name, const std::string &tooltip, ZLSpinOptionEntry *option, ZLQtDialogContent *tab, int row, int fromColumn, int toColumn) : ZLQtOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), mySpinBox(0) {}

protected:
	void _createItem();
	void _onAccept() const;

private:
	QSpinBox *mySpinBox;
};

class ComboOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	ComboOptionView(const std::string &name, const std::string &tooltip, ZLComboOptionEntry *option, ZLQtDialogContent *tab, int row, int fromColumn, int toColumn) : ZLQtOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myComboBox(0) {}

private:
	void _createItem();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();

private Q_SLOTS:
	void onValueSelected(int index);
	void onValueEdited(const QString &value);
	
private:
	QComboBox *myComboBox;
};

class KeyOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	KeyOptionView(const std::string &name, const std::string &tooltip, ZLKeyOptionEntry *option, ZLQtDialogContent *tab, int row, int fromColumn, int toColumn) : ZLQtOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myComboBox(0), myKeyEditor(0) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;
	void reset();

private Q_SLOTS:
	void onValueChanged(int);

private:
	QComboBox *myComboBox;
	QLineEdit *myKeyEditor;
	std::string myCurrentKey;

friend class KeyLineEdit;
};

class ColorOptionView : public QObject, public ZLQtOptionView {

Q_OBJECT

public:
	ColorOptionView(const std::string &name, const std::string &tooltip, ZLColorOptionEntry *option, ZLQtDialogContent *tab, int row, int fromColumn, int toColumn) : ZLQtOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myRSlider(0), myGSlider(0), myBSlider(0), myColorBar(0) {}

private:
	void _createItem();
	void _onAccept() const;
	void reset();

	QSlider *createColorSlider(QGridLayout *layout, int index, const ZLResource &resource, int value);

private Q_SLOTS:
	void onSliderMove(int);

private:
	QSlider *myRSlider, *myGSlider, *myBSlider;
	QLabel *myColorBar;
};

#endif /* __ZLQTOPTIONVIEW_H__ */
