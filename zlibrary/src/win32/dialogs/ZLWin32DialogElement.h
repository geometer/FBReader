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

#ifndef __ZLWIN32DIALOGELEMENT_H__
#define __ZLWIN32DIALOGELEMENT_H__

#include <vector>
#include <string>

#include <windows.h>

#include <shared_ptr.h>

class ZLWin32DialogUtil {

public:
	static int allocateString(WORD *p, const std::string &text);

private:
	ZLWin32DialogUtil();
};

class ZLWin32DialogElement {

protected:
	ZLWin32DialogElement();

public:
	virtual ~ZLWin32DialogElement();

	virtual void allocate(WORD *p) const = 0;
	virtual int allocationSize() const = 0;
	virtual void setVisible(bool visible) = 0;
	virtual int controlNumber() const = 0;

private:
	ZLWin32DialogElement(const ZLWin32DialogElement&);
	const ZLWin32DialogElement &operator = (const ZLWin32DialogElement&);
};

typedef shared_ptr<ZLWin32DialogElement> ZLWin32DialogElementPtr;
typedef std::vector<ZLWin32DialogElementPtr> ZLWin32DialogElementList;

class ZLWin32DialogBox : public ZLWin32DialogElement {

public:
	ZLWin32DialogBox();
	void addElement(ZLWin32DialogElementPtr element);

	void allocate(WORD *p) const;
	int allocationSize() const;
	void setVisible(bool visible);
	int controlNumber() const;

private:
	ZLWin32DialogElementList myElements;
};

class ZLWin32DialogHBox : public ZLWin32DialogBox {

public:
	ZLWin32DialogHBox();
};

class ZLWin32DialogVBox : public ZLWin32DialogBox {

public:
	ZLWin32DialogVBox();
};

class ZLWin32DialogControl : public ZLWin32DialogElement {

protected:
	ZLWin32DialogControl(DWORD style, int x, int y, int width, int height, WORD id, const std::string &className, const std::string &text);

private:
	void allocate(WORD *p) const;
	int allocationSize() const;
	void setVisible(bool visible);
	int controlNumber() const;

private:
	DWORD myStyle;
	int myX, myY;
	int myWidth, myHeight;
	WORD myId;
	const std::string &myClassName;
	std::string myText;
};

class ZLWin32PushButton : public ZLWin32DialogControl {

public:
	ZLWin32PushButton(int x, int y, int width, int height, WORD id, const std::string &text);
};

class ZLWin32CheckBox : public ZLWin32DialogControl {

public:
	ZLWin32CheckBox(int x, int y, int width, int height, WORD id, const std::string &text);
};

class ZLWin32LineEditor : public ZLWin32DialogControl {

public:
	ZLWin32LineEditor(int x, int y, int width, int height, WORD id, const std::string &text);
};

class ZLWin32DialogPanel {

public:
	ZLWin32DialogPanel(DWORD style, int x, int y, int width, int height, const std::string &text);
	~ZLWin32DialogPanel();
	DLGTEMPLATE *dialogTemplate() const;
	void setElement(ZLWin32DialogElementPtr element);

private:
	DWORD myStyle;
	int myX, myY;
	int myWidth, myHeight;
	std::string myText;

	ZLWin32DialogElementPtr myElement;

	mutable WORD *myAddress;
};

#endif /* __ZLWIN32DIALOGELEMENT_H__ */
