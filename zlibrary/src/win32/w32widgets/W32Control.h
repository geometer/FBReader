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

#ifndef __W32CONTROL_H__
#define __W32CONTROL_H__

#include <vector>
#include <string>

#include <windows.h>

#include <ZLUnicodeUtil.h>

#include "W32Widget.h"
#include "W32Event.h"

class W32Control : public W32Widget, public W32EventSender {

protected:
	W32Control(DWORD style);

	Size minimumSize() const;
	void setPosition(int x, int y, Size size);
	void init(HWND parent, W32ControlCollection *collection);

public:
	void setEnabled(bool enabled);
	bool isEnabled() const;
	virtual void setVisible(bool visible);
	bool isVisible() const;

public:
	virtual void commandCallback(DWORD hiWParam);
	virtual void notificationCallback(LPARAM lParam);

protected:
	DWORD myStyle;
	int myX, myY;
	Size mySize;

	bool myEnabled;

	W32ControlCollection *myOwner;
	HWND myWindow;
};

class W32StandardControl : public W32Control {

protected:
	W32StandardControl(DWORD style);

	int controlNumber() const;
	void allocate(WORD *&p, short &id) const;
	int allocationSize() const;
	virtual WORD classId() const = 0;
};

class W32PushButton : public W32StandardControl {

public:
	static const std::string RELEASED_EVENT;

public:
	enum ButtonType {
		NORMAL_BUTTON,
		OK_BUTTON,
		CANCEL_BUTTON
	};

public:
	W32PushButton(const std::string &text, ButtonType type = NORMAL_BUTTON);
	void setDimensions(Size charDimension);

	void allocate(WORD *&p, short &id) const;
	WORD classId() const;
	void init(HWND parent, W32ControlCollection *collection);

	void commandCallback(DWORD hiWParam);

private:
	std::string myText;
	ButtonType myType;
};

class W32Label : public W32StandardControl {

public:
	W32Label(const std::string &text);
	void setDimensions(Size charDimension);

	WORD classId() const;
	void setPosition(int x, int y, Size size);
	void init(HWND parent, W32ControlCollection *collection);

private:
	std::string myText;
	int myVShift;
};

class W32CheckBox : public W32StandardControl {

public:
	static const std::string STATE_CHANGED_EVENT;

public:
	W32CheckBox(const std::string &text);
	void setDimensions(Size charDimension);

	WORD classId() const;
	void init(HWND parent, W32ControlCollection *collection);

	void setChecked(bool checked);
	bool isChecked() const;

	void commandCallback(DWORD hiWParam);

private:
	std::string myText;
	bool myChecked;
};

class W32AbstractEditor : public W32StandardControl {

public:
	W32AbstractEditor(DWORD style);

	WORD classId() const;
	void init(HWND parent, W32ControlCollection *collection);
};

class W32LineEditor : public W32AbstractEditor {

public:
	static const std::string VALUE_EDITED_EVENT;

public:
	W32LineEditor(const std::string &text);
	void setDimensions(Size charDimension);
	void init(HWND parent, W32ControlCollection *collection);

	void setEditable(bool editable);

	void commandCallback(DWORD hiWParam);

	void setText(const std::string &text);
	std::string text() const;

private:
	ZLUnicodeUtil::Ucs2String myBuffer;
	bool myBlocked;
};

class W32SpinBox : public W32AbstractEditor {

public:
	W32SpinBox(WORD min, WORD max, WORD initial);
	void setDimensions(Size charDimension);

	void allocate(WORD *&p, short &id) const;
	int allocationSize() const;
	int controlNumber() const;
	void setPosition(int x, int y, Size size);
	void init(HWND parent, W32ControlCollection *collection);

	void setVisible(bool visible);

	void commandCallback(DWORD hiWParam);

	unsigned short value() const;

private:
	WORD myMin, myMax, myValue;
	HWND myControlWindow;
};

class W32ComboBox : public W32StandardControl {

public:
	static const std::string SELECTION_CHANGED_EVENT;
	static const std::string VALUE_EDITED_EVENT;

public:
	W32ComboBox(const std::vector<std::string> &list, int initialIndex);
	void setDimensions(Size charDimension);

	WORD classId() const;
	void init(HWND parent, W32ControlCollection *collection);

	void setEditable(bool editable);

	void commandCallback(DWORD hiWParam);

	std::string text() const;

	void setList(const std::vector<std::string> &list);
	void setSelection(int index);

private:
	HWND editorWindow();

private:
	std::vector<std::string> myList;
	int myIndex;

	ZLUnicodeUtil::Ucs2String myBuffer;
	HWND myEditorWindow;
};

class W32RadioButtonGroup;

class W32RadioButton : public W32StandardControl {

public:
	W32RadioButton(W32RadioButtonGroup &group, const std::string &text);
	WORD classId() const;

private:
	void setDimensions(Size charDimension);
	void init(HWND parent, W32ControlCollection *collection);
	void commandCallback(DWORD hiWParam);

	void setChecked(bool checked);

private:
	W32RadioButtonGroup &myGroup;
	std::string myText;

friend class W32RadioButtonGroup;
};

class W32RadioButtonGroup : public W32StandardControl {

public:
	W32RadioButtonGroup(const std::string &caption, const std::vector<std::string> &buttonTexts);

	void setVisible(bool visible);
	void setChecked(int index);
	int checkedIndex() const;

private:
	WORD classId() const;
	void allocate(WORD *&p, short &id) const;
	int allocationSize() const;
	int controlNumber() const;
	void setPosition(int x, int y, Size size);
	void setDimensions(Size charDimension);
	void init(HWND parent, W32ControlCollection *collection);

	void setChecked(W32RadioButton &button);

private:
	std::string myCaption;
	W32WidgetList myButtons;
	int myCheckedIndex;

	int myLeftMargin;

friend class W32RadioButton;
};

#endif /* __W32CONTROL_H__ */
