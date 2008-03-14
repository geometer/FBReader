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

#ifndef __ZLGTKOPTIONVIEW_H__
#define __ZLGTKOPTIONVIEW_H__

#include <gtk/gtk.h>

#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include "../../../../core/src/dialogs/ZLOptionView.h"

class ZLGtkDialogContent;

class ZLGtkOptionView : public ZLOptionView {

protected:
	ZLGtkOptionView(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, ZLGtkDialogContent *tab, int row, int fromColumn, int toColumn) : ZLOptionView(name, tooltip, option), myTab(tab), myRow(row), myFromColumn(fromColumn), myToColumn(toColumn) {}
	
protected:
	static void _onValueChanged(GtkWidget*, gpointer self);
	// TODO: replace by pure virtual method (?)
	virtual void onValueChanged() {}

protected:
	ZLGtkDialogContent *myTab;
	int myRow, myFromColumn, myToColumn;
};

class ChoiceOptionView : public ZLGtkOptionView {

public:
	ChoiceOptionView(const std::string &name, const std::string &tooltip, ZLChoiceOptionEntry *option, ZLGtkDialogContent *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(name, tooltip, option, tab, row, fromColumn, toColumn) {
		myFrame = 0;
	}
	~ChoiceOptionView() { if (myFrame != 0) delete[] myButtons; }

protected:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;

private:
	GtkFrame *myFrame;
	GtkBox *myVBox;
	GtkRadioButton **myButtons;
};

class BooleanOptionView : public ZLGtkOptionView {

public:
	BooleanOptionView(const std::string &name, const std::string &tooltip, ZLBooleanOptionEntry *option, ZLGtkDialogContent *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(name, tooltip, option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;

private:
	void onValueChanged();

private:
	GtkCheckButton *myCheckBox;
};

class Boolean3OptionView : public ZLGtkOptionView {

private:
	static void _onReleased(GtkButton *button, gpointer self);

public:
	Boolean3OptionView(const std::string &name, const std::string &tooltip, ZLBoolean3OptionEntry *option, ZLGtkDialogContent *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(name, tooltip, option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;

private:
	void setState(ZLBoolean3 state);
	void onValueChanged();

private:
	ZLBoolean3 myState;
	GtkCheckButton *myCheckBox;
};

class StringOptionView : public ZLGtkOptionView {

public:
	StringOptionView(const std::string &name, const std::string &tooltip, ZLStringOptionEntry *option, ZLGtkDialogContent *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myLabel(0), myLineEdit(0) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();
	void onValueChanged();

private:
	GtkLabel *myLabel;
	GtkEntry *myLineEdit;
};

class SpinOptionView : public ZLGtkOptionView {

public:
	SpinOptionView(const std::string &name, const std::string &tooltip, ZLSpinOptionEntry *option, ZLGtkDialogContent *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myLabel(0), mySpinBox(0) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private:
	GtkLabel *myLabel;
	GtkSpinButton *mySpinBox;
};

class ComboOptionView : public ZLGtkOptionView {

public:
	ComboOptionView(const std::string &name, const std::string &tooltip, ZLComboOptionEntry *option, ZLGtkDialogContent *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myLabel(0), myComboBox(0), myListSize(0) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();
	void onValueChanged();
	
private:
	GtkLabel *myLabel;
	GtkComboBox *myComboBox;
	int mySelectedIndex;
	int myListSize;
};

class ColorOptionView : public ZLGtkOptionView {

public:
	ColorOptionView(const std::string &name, const std::string &tooltip, ZLColorOptionEntry *option, ZLGtkDialogContent *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myWidget(0), myDrawingArea(0), myRSlider(0), myGSlider(0), myBSlider(0) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;
	void reset();

	void onSliderMove();
	static void _onSliderMove(GtkRange*, gpointer);

private:
	GtkWidget *myWidget, *myDrawingArea;
	GtkWidget *myRSlider, *myGSlider, *myBSlider;
	GdkColor myColor;
};

class KeyOptionView : public ZLGtkOptionView {

public:
	KeyOptionView(const std::string &name, const std::string &tooltip, ZLKeyOptionEntry *option, ZLGtkDialogContent *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(name, tooltip, option, tab, row, fromColumn, toColumn), myTable(0), myKeyEntry(0), myLabel(0), myComboBox(0) {}

	void setKey(const std::string &key);

private:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;
	void onValueChanged();
	void reset();

private:
	GtkTable *myTable;
	GtkEntry *myKeyEntry;
	GtkLabel *myLabel;
	GtkComboBox *myComboBox;
	std::string myCurrentKey;
};

#endif /* __ZLGTKOPTIONVIEW_H__ */
