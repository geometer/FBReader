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

public:
	struct Size {
		short Width;
		short Height;

		Size() : Width(0), Height(0) {}
		Size(short w, short h) : Width(w), Height(h) {}
	};

protected:
	ZLWin32DialogElement();

public:
	virtual ~ZLWin32DialogElement();

	virtual void allocate(WORD *p) const = 0;
	virtual int allocationSize() const = 0;
	virtual void setVisible(bool visible) = 0;
	virtual int controlNumber() const = 0;
	virtual Size minimumSize() const = 0;
	virtual void setPosition(int x, int y, Size size) = 0;
	virtual void setDimensions(Size charDimension) = 0;

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

	void setHomogeneous(bool homogeneous);
	void setMargins(int top, int bottom, int left, int right);
	void setSpacing(int spacing);

protected:
	bool homogeneous() const { return myHomogeneous; }
	int topMargin() const { return myTopMargin; }
	int bottomMargin() const { return myBottomMargin; }
	int leftMargin() const { return myLeftMargin; }
	int rightMargin() const { return myRightMargin; }
	int spacing() const { return mySpacing; }
	void setDimensions(Size charDimension);

protected:
	ZLWin32DialogElementList myElements;

private:
	bool myHomogeneous;
	int myTopMargin, myBottomMargin, myLeftMargin, myRightMargin;
	int mySpacing;
};

class ZLWin32DialogHBox : public ZLWin32DialogBox {

public:
	ZLWin32DialogHBox();

	Size minimumSize() const;
	void setPosition(int x, int y, Size size);
};

class ZLWin32DialogVBox : public ZLWin32DialogBox {

public:
	ZLWin32DialogVBox();

	Size minimumSize() const;
	void setPosition(int x, int y, Size size);
};

class ZLWin32DialogControl : public ZLWin32DialogElement {

protected:
	ZLWin32DialogControl(DWORD style, WORD id, const std::string &className, const std::string &text);

private:
	void allocate(WORD *p) const;
	int allocationSize() const;
	void setVisible(bool visible);
	int controlNumber() const;
	Size minimumSize() const;
	void setPosition(int x, int y, Size size);

private:
	DWORD myStyle;
	int myX, myY;
	WORD myId;
	const std::string &myClassName;

protected:
	Size mySize;
	std::string myText;
};

class ZLWin32PushButton : public ZLWin32DialogControl {

public:
	ZLWin32PushButton(WORD id, const std::string &text);
	void setDimensions(Size charDimension);
};

class ZLWin32CheckBox : public ZLWin32DialogControl {

public:
	ZLWin32CheckBox(WORD id, const std::string &text);
	void setDimensions(Size charDimension);
};

class ZLWin32LineEditor : public ZLWin32DialogControl {

public:
	ZLWin32LineEditor(WORD id, const std::string &text);
	void setDimensions(Size charDimension);
};

class ZLWin32DialogPanel {

public:
	ZLWin32DialogPanel(HWND mainWindow, const std::string &caption);
	~ZLWin32DialogPanel();
	DLGTEMPLATE *dialogTemplate();
	void setElement(ZLWin32DialogElementPtr element);
	ZLWin32DialogElement::Size charDimension() const;

private:
	ZLWin32DialogElement::Size myCharDimension;
	ZLWin32DialogElement::Size mySize;
	std::string myCaption;

	ZLWin32DialogElementPtr myElement;

	mutable WORD *myAddress;
};

#endif /* __ZLWIN32DIALOGELEMENT_H__ */
