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

#ifndef __ZLGTKOPTIONVIEW_H__
#define __ZLGTKOPTIONVIEW_H__

#include <gtk/gtkwidget.h>
#include <gtk/gtkrange.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkcombobox.h>
#include <gtk/gtkentry.h>
#include <hildon-controlbar.h>

#include "../../abstract/dialogs/ZLOptionsDialog.h"
#include "../../abstract/dialogs/ZLOptionEntry.h"

class ZLGtkOptionsDialogTab;

class ZLGtkOptionView : public ZLOptionView {

protected:
	ZLGtkOptionView(ZLOptionEntry *option, ZLGtkOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : ZLOptionView(option), myTab(tab), myRow(row), myFromColumn(fromColumn), myToColumn(toColumn), myInitialized(false) {}

public:
	virtual ~ZLGtkOptionView() {}
	void setPosition(ZLGtkOptionsDialogTab *tab, int row, int fromColumn, int toColumn) {
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
	static void _onValueChanged(GtkWidget*, gpointer self);
	virtual void onValueChanged() {}

protected:
	ZLGtkOptionsDialogTab *myTab;
	int myRow, myFromColumn, myToColumn;

private:
	bool myInitialized;
};

class ChoiceOptionView : public ZLGtkOptionView {

public:
	ChoiceOptionView(ZLChoiceOptionEntry *option, ZLGtkOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(option, tab, row, fromColumn, toColumn) {
		myFrame = 0;
	}
	~ChoiceOptionView() { if (myFrame != 0) delete[] myButtons; }

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private:
	GtkFrame *myFrame;
	GtkBox *myVBox;
	GtkRadioButton **myButtons;
};

class BooleanOptionView : public ZLGtkOptionView {

public:
	BooleanOptionView(ZLBooleanOptionEntry *option, ZLGtkOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private:
	void onValueChanged();

private:
	GtkWidget *myCheckBox;
};

class StringOptionView : public ZLGtkOptionView {

public:
	StringOptionView(ZLStringOptionEntry *option, ZLGtkOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(option, tab, row, fromColumn, toColumn), myLabel(0), myLineEdit(0) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();
	void onValueChanged();

private:
	GtkWidget *myLabel;
	GtkEntry *myLineEdit;
};

class SpinOptionView : public ZLGtkOptionView {

public:
	SpinOptionView(ZLSpinOptionEntry *option, ZLGtkOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(option, tab, row, fromColumn, toColumn) {}

protected:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;

private:
	GtkWidget *myLabel, *mySpinBox;
};

class ComboOptionView : public ZLGtkOptionView {

public:
	ComboOptionView(ZLComboOptionEntry *option, ZLGtkOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(option, tab, row, fromColumn, toColumn), myLabel(0), myComboBox(0), myListSize(0) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _setActive(bool active);
	void _onAccept() const;
	void reset();
	void onValueChanged();
	
private:
	GtkWidget *myLabel;
	GtkComboBox *myComboBox;
	int mySelectedIndex;
	int myListSize;
};

class ColorOptionView : public ZLGtkOptionView {

public:
	ColorOptionView(ZLColorOptionEntry *option, ZLGtkOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(option, tab, row, fromColumn, toColumn), myWidget(0), myDrawingArea(0), myRSlider(0), myGSlider(0), myBSlider(0) {}

private:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;
	void reset();

	void onSliderMove();
	static void _onSliderMove(GtkRange *, gpointer);

private:
	GtkWidget *myWidget, *myDrawingArea;
	HildonControlbar *myRSlider, *myGSlider, *myBSlider;
	GdkColor myColor;
};

class KeyOptionView : public ZLGtkOptionView {

public:
	KeyOptionView(ZLKeyOptionEntry *option, ZLGtkOptionsDialogTab *tab, int row, int fromColumn, int toColumn) : ZLGtkOptionView(option, tab, row, fromColumn, toColumn), myWidget(0), myKeyButton(0), myLabel(0), myComboBox(0) {}

	void setKey(const std::string &key);

private:
	void _createItem();
	void _show();
	void _hide();
	void _onAccept() const;
	void onValueChanged();
	void reset();

public:
	GtkWidget *comboBox() const { return GTK_WIDGET(myComboBox); }

private:
	GtkWidget *myWidget, *myKeyButton, *myLabel;
	GtkComboBox *myComboBox;
	std::string myCurrentKey;
};

#endif /* __ZLGTKOPTIONVIEW_H__ */
