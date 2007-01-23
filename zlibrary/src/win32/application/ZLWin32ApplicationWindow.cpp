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

#include <iostream>

#include <ZLOptionEntry.h>
#include <ZLDialog.h>

//#include "../util/ZLWin32KeyUtil.h"
//#include "../util/ZLWin32SignalUtil.h"

#include "ZLWin32ApplicationWindow.h"
#include "../dialogs/ZLWin32DialogManager.h"
#include "../view/ZLWin32ViewWidget.h"

void ZLWin32DialogManager::createApplicationWindow(ZLApplication *application) const {
	//myWindow = (new ZLWin32ApplicationWindow(application))->getMainWindow();
	new ZLWin32ApplicationWindow(application);
}

/*
static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	((ZLWin32ApplicationWindow*)data)->application().closeView();
	return true;
}

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	((ZLWin32ApplicationWindow*)data)->application().refreshWindow();
}

static void onButtonClicked(GtkWidget *button, gpointer data) {
	((ZLWin32ApplicationWindow*)data)->onGtkButtonPress(button);
}

static void handleKeyEvent(GtkWidget*, GdkEventKey *event, gpointer data) {
	((ZLWin32ApplicationWindow*)data)->handleKeyEventSlot(event);
}

static void handleScrollEvent(GtkWidget*, GdkEventScroll *event, gpointer data) {
	((ZLWin32ApplicationWindow*)data)->handleScrollEventSlot(event);
}
*/

static ZLWin32ViewWidget *VIEW_WIDGET;
static ZLWin32ApplicationWindow *APPLICATION_WINDOW;
static HWND TOOLBAR = 0;

LRESULT CALLBACK ZLWin32ApplicationWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_SIZE:
			if (TOOLBAR != 0) {
				MoveWindow(TOOLBAR, 0, 0, LOWORD(lParam), 33, true);
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		case WM_PAINT:
			//std::cerr << "WM_PAINT received\n";
			VIEW_WIDGET->doPaint();
			return 0;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_COMMAND:
			APPLICATION_WINDOW->onToolbarButtonPress(LOWORD(wParam));
			return 0;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

static const std::string OPTIONS = "Options";

ZLWin32ApplicationWindow::ZLWin32ApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 2000, 800),
	myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 2000, 600),
	myToolbar(0) {
	//myFullScreen(false) {
	
	APPLICATION_WINDOW = this;

/*
	ZLWin32SignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	myVBox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), myVBox);

	gtk_window_resize(myMainWindow, myWidthOption.value(), myHeightOption.value());
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	ZLWin32SignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKeyEvent), this);
	ZLWin32SignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "scroll_event", G_CALLBACK(handleScrollEvent), this);
*/
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(0);
	wc.hIcon = LoadIcon(wc.hInstance, TEXT("MAIN_ICON"));
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = ZLApplication::ApplicationName().c_str();
	wc.hIconSm = LoadIcon(wc.hInstance, TEXT("MAIN_ICON_SMALL"));

	RegisterClassEx(&wc);

	myMainWindow = CreateWindow(wc.lpszClassName, ZLApplication::ApplicationName().c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, myWidthOption.value(), myHeightOption.value(), (HWND)0, (HMENU)0, wc.hInstance, 0);

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);
}

void ZLWin32ApplicationWindow::init() {
	ZLApplicationWindow::init();

	if (myToolbar != 0) {
		ShowWindow(myToolbar, SW_SHOW);
	}

	// TODO: Hmm, replace SW_SHOWDEFAULT by nCmdShow?
	ShowWindow(myMainWindow, SW_SHOWDEFAULT);
}

ZLWin32ApplicationWindow::~ZLWin32ApplicationWindow() {
	//DestroyMenu(myToolbar);
/*
	if (!myFullScreen) {
		int width, height;
		gtk_window_get_size(myMainWindow, &width, &height);
		myWidthOption.setValue(width);
		myHeightOption.setValue(height);
	}
	myButtonToWidget.clear();
	myWidgetToButton.clear();
*/
}

/*
void ZLWin32ApplicationWindow::handleKeyEventSlot(GdkEventKey *event) {
	application().doActionByKey(ZLWin32KeyUtil::keyName(event));
}

void ZLWin32ApplicationWindow::handleScrollEventSlot(GdkEventScroll *event) {
	switch (event->direction) {
		case GDK_SCROLL_UP:
			application().doActionByKey(ZLApplication::MouseScrollUpKey);
			break;
		case GDK_SCROLL_DOWN:
			application().doActionByKey(ZLApplication::MouseScrollDownKey);
			break;
		default:
			break;
	}
}
*/

void ZLWin32ApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	PostMessage(myToolbar, TB_CHECKBUTTON, button.actionId(), button.isPressed());
}

void ZLWin32ApplicationWindow::onToolbarButtonPress(int actionCode) {
	onButtonPress((ZLApplication::Toolbar::ButtonItem&)*myButtonByActionCode[actionCode]);
}

void ZLWin32ApplicationWindow::setFullscreen(bool fullscreen) {
	/*
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;

	if (myFullScreen) {
		gtk_window_fullscreen(myMainWindow);
		gtk_widget_hide(myToolbar);
	} else if (!myFullScreen) {
		gtk_window_unfullscreen(myMainWindow);
		gtk_widget_show(myToolbar);
	}

	gtk_widget_queue_resize(GTK_WIDGET(myMainWindow));
	*/
}

bool ZLWin32ApplicationWindow::isFullscreen() const {
	//return myFullScreen;
	return false;
}

void ZLWin32ApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (myToolbar == 0) {
  	myToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, 0, WS_CHILD | CCS_NORESIZE | TBSTYLE_FLAT, 0, 0, 0, 0, myMainWindow, (HMENU)0, 0, 0);
		TOOLBAR = myToolbar;
		SendMessage(myToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(myToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(24, 24));
		SendMessage(myToolbar, TB_SETINDENT, 3, 0);
	}

	if (item->isButton()) {
		static int buttonCounter = 0;
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;

		TBADDBITMAP addBitmap;
		addBitmap.hInst = GetModuleHandle(0);
		addBitmap.nID = 101 + buttonCounter;
		SendMessage(myToolbar, TB_ADDBITMAP, 1, (LPARAM)&addBitmap);

		TBBUTTON button;
		button.iBitmap = buttonCounter;
		button.fsState = TBSTATE_ENABLED;
		button.fsStyle = buttonItem.isToggleButton() ? TBSTYLE_CHECK : TBSTYLE_BUTTON;
		button.idCommand = buttonItem.actionId();
		myButtonByActionCode[button.idCommand] = item;
		button.dwData = 0;
		button.iString = 0;
		SendMessage(myToolbar, TB_ADDBUTTONS, 1, (LPARAM)&button);

		++buttonCounter;
	} else {
		TBBUTTON separator;
		separator.iBitmap = 6;
		separator.fsState = TBSTATE_ENABLED;
		separator.fsStyle = TBSTYLE_SEP;
		SendMessage(myToolbar, TB_ADDBUTTONS, 1, (LPARAM)&separator);
	}
}

void ZLWin32ApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	if (item->isButton()) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		LPARAM state = (visible ? 0 : TBSTATE_HIDDEN) | (enabled ? TBSTATE_ENABLED : 0);
		PostMessage(myToolbar, TB_SETSTATE, (WPARAM)buttonItem.actionId(), state);
		if (buttonItem.isToggleButton()) {
			setToggleButtonState(buttonItem);
		}
	}
}

void ZLWin32ApplicationWindow::refresh() {
	ZLApplicationWindow::refresh();
}

ZLViewWidget *ZLWin32ApplicationWindow::createViewWidget() {
	/*
	ZLWin32ViewWidget *viewWidget = new ZLWin32ViewWidget(&application(), (ZLViewWidget::Angle)application().AngleStateOption.value());
	gtk_container_add(GTK_CONTAINER(myVBox), viewWidget->area());
	// Was gtk_signal_connect_after(). Why?
	ZLWin32SignalUtil::connectSignal(GTK_OBJECT(viewWidget->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);
	gtk_widget_show_all(myVBox);
	return viewWidget;
	*/
	myWin32ViewWidget = new ZLWin32ViewWidget(*this);
	VIEW_WIDGET = myWin32ViewWidget;
	return myWin32ViewWidget;
}

void ZLWin32ApplicationWindow::close() {
	//ZLWin32SignalUtil::removeAllSignals();
	//gtk_main_quit();
}

bool ZLWin32ApplicationWindow::isFullKeyboardControlSupported() const {
	return false;
}

void ZLWin32ApplicationWindow::grabAllKeys(bool) {
}

bool ZLWin32ApplicationWindow::isFingerTapEventSupported() const {
	return false;
}

bool ZLWin32ApplicationWindow::isMousePresented() const {
	return true;
}

bool ZLWin32ApplicationWindow::isKeyboardPresented() const {
	return true;
}

void ZLWin32ApplicationWindow::setCaption(const std::string &caption) {
	//gtk_window_set_title (myMainWindow, caption.c_str ());
}

HWND ZLWin32ApplicationWindow::mainWindow() const {
	return myMainWindow;
}

int ZLWin32ApplicationWindow::topOffset() const {
	if (myToolbar != 0) {
		RECT toolbarRectangle;
		GetWindowRect(myToolbar, &toolbarRectangle);
		return toolbarRectangle.bottom - toolbarRectangle.top + 1;
	}
	return 0;
}
