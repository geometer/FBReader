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

#include "W32Element.h"

class W32ControlListener {

protected:
	W32ControlListener();

public:
	virtual ~W32ControlListener();
	virtual void onEvent(const std::string &event) = 0;
};

class W32Control : public W32Element {

protected:
	W32Control(DWORD style);

	void allocate(WORD *&p, short &id) const;
	int allocationSize() const;
	int controlNumber() const;
	Size minimumSize() const;
	void setPosition(int x, int y, Size size);
	void init(HWND parent, W32ControlCollection *collection);

	virtual WORD classId() const = 0;

	void fireEvent(const std::string &event) const;

public:
	void setEnabled(bool enabled);
	bool isEnabled() const;
	virtual void setVisible(bool visible);
	bool isVisible() const;

	void setListener(W32ControlListener *listener);

public:
	virtual void callback(DWORD hiWParam);

protected:
	DWORD myStyle;
	int myX, myY;
	Size mySize;

	bool myEnabled;

	W32ControlCollection *myOwner;
	HWND myWindow;

	W32ControlListener *myListener;
};

class W32PushButton : public W32Control {

public:
	W32PushButton(const std::string &text);
	void setDimensions(Size charDimension);

	WORD classId() const;
	void init(HWND parent, W32ControlCollection *collection);

private:
	std::string myText;
};

class W32Label : public W32Control {

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

class W32CheckBox : public W32Control {

public:
	static const std::string STATE_CHANGED_EVENT;

public:
	W32CheckBox(const std::string &text);
	void setDimensions(Size charDimension);

	WORD classId() const;
	void init(HWND parent, W32ControlCollection *collection);

	void setChecked(bool checked);
	bool isChecked() const;

	void callback(DWORD hiWParam);

private:
	std::string myText;
	bool myChecked;
};

class W32AbstractEditor : public W32Control {

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

	void callback(DWORD hiWParam);

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

	void callback(DWORD hiWParam);

	unsigned short value() const;

private:
	WORD myMin, myMax, myValue;
	HWND myControlWindow;
};

class W32ComboBox : public W32Control {

public:
	static const std::string SELECTION_CHANGED_EVENT;
	static const std::string VALUE_EDITED_EVENT;

public:
	W32ComboBox(const std::vector<std::string> &list, int initialIndex);
	void setDimensions(Size charDimension);

	WORD classId() const;
	void init(HWND parent, W32ControlCollection *collection);

	void setEditable(bool editable);

	void callback(DWORD hiWParam);

	std::string text() const;

private:
	const std::vector<std::string> &myList;
	int myIndex;

	ZLUnicodeUtil::Ucs2String myBuffer;
	HWND myEditorWindow;
};

#endif /* __W32CONTROL_H__ */
