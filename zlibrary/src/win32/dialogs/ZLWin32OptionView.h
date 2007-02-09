/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLWIN32OPTIONVIEW_H__
#define __ZLWIN32OPTIONVIEW_H__

#include "../../abstract/dialogs/ZLOptionsDialog.h"
#include "../../abstract/dialogs/ZLOptionEntry.h"
#include "../../abstract/dialogs/ZLOptionView.h"

#include "../w32widgets/W32Control.h"
#include "../w32widgets/W32Container.h"

class ZLWin32DialogContent;

class ZLWin32OptionView : public ZLOptionView {

protected:
	ZLWin32OptionView(ZLOptionEntry *option) : ZLOptionView(option) {
	}

protected:
	void _createItem();
};

/*
class ChoiceOptionView : public ZLWin32OptionView {

public:
	ChoiceOptionView(ZLChoiceOptionEntry *option, ZLWin32DialogContent *tab, int row, int fromColumn, int toColumn) : ZLWin32OptionView(option, tab, row, fromColumn, toColumn) {
		myFrame = 0;
	}
	~ChoiceOptionView() { if (myFrame != 0) delete[] myButtons; }

protected:
	void _onAccept() const;

private:
	Win32Frame *myFrame;
	Win32Box *myVBox;
	Win32RadioButton **myButtons;
};
*/

class BooleanOptionView : public ZLWin32OptionView, public W32Listener {

public:
	BooleanOptionView(ZLBooleanOptionEntry *option, ZLWin32DialogContent &tab, int from, int to);

protected:
	void _onAccept() const;
	void _show();
	void _hide();

private:
	void onEvent(const std::string &event, W32EventSender &sender);

private:
	W32CheckBox *myCheckBox;
};

class StringOptionView : public ZLWin32OptionView, public W32Listener {

public:
	StringOptionView(ZLStringOptionEntry *option, ZLWin32DialogContent &tab, int from, int to);

private:
	void _setActive(bool active);
	void _onAccept() const;
	void reset();
	void _show();
	void _hide();

private:
	void onEvent(const std::string &event, W32EventSender &sender);

private:
	W32LineEditor *myLineEditor;
	W32Label *myLabel;
};

class SpinOptionView : public ZLWin32OptionView {

public:
	SpinOptionView(ZLSpinOptionEntry *option, ZLWin32DialogContent &tab, int from, int to);

protected:
	void _onAccept() const;
	void _show();
	void _hide();

private:
	W32SpinBox *mySpinBox;
	W32Label *myLabel;
};

class ComboOptionView : public ZLWin32OptionView, public W32Listener {

public:
	ComboOptionView(ZLComboOptionEntry *option, ZLWin32DialogContent &tab, int from, int to);

private:
	void _setActive(bool active);
	void _onAccept() const;
	//void reset();
	void _show();
	void _hide();
	
private:
	void onEvent(const std::string &event, W32EventSender &sender);

private:
	W32ComboBox *myComboBox;
	W32Label *myLabel;
};

/*
class ColorOptionView : public ZLWin32OptionView {

public:
	ColorOptionView(ZLColorOptionEntry *option, ZLWin32DialogContent *tab, int row, int fromColumn, int toColumn) : ZLWin32OptionView(option, tab, row, fromColumn, toColumn), myWidget(0), myDrawingArea(0), myRSlider(0), myGSlider(0), myBSlider(0) {}

private:
	void _onAccept() const;
	void reset();

	void onSliderMove();
	static void _onSliderMove(Win32Range*, gpointer);

private:
	Win32Widget *myWidget, *myDrawingArea;
	Win32Widget *myRSlider, *myGSlider, *myBSlider;
	GdkColor myColor;
};

class KeyOptionView : public ZLWin32OptionView {

public:
	KeyOptionView(ZLKeyOptionEntry *option, ZLWin32DialogContent *tab, int row, int fromColumn, int toColumn) : ZLWin32OptionView(option, tab, row, fromColumn, toColumn), myWidget(0), myKeyButton(0), myLabel(0), myComboBox(0) {}

	void setKey(const std::string &key);

private:
	void _onAccept() const;
	void onValueChanged();
	void reset();

private:
	Win32Widget *myWidget, *myKeyButton, *myLabel;
	Win32ComboBox *myComboBox;
	std::string myCurrentKey;
};
*/

#endif /* __ZLWIN32OPTIONVIEW_H__ */
