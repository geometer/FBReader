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

#include <windows.h>

#include <ZLOptionEntry.h>
#include <ZLDialog.h>

//#include "../util/ZLWin32KeyUtil.h"
//#include "../util/ZLWin32SignalUtil.h"

#include "ZLWin32ApplicationWindow.h"
#include "../dialogs/ZLWin32DialogManager.h"
//#include "../view-desktop/ZLWin32ViewWidget.h"

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;

	switch (uMsg) {
		case WM_PAINT:
			hDC = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rect);
			DrawText(hDC, "Hello, World!", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

//static const std::string OPTIONS = "Options";

ZLWin32ApplicationWindow::ZLWin32ApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application) {
	//myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 2000, 800),
	//myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 2000, 800),
	//myFullScreen(false) {

/*
	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	ZLWin32SignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	myVBox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), myVBox);

	myToolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(myVBox), myToolbar, false, false, 0);
	gtk_toolbar_set_style(GTK_TOOLBAR(myToolbar), GTK_TOOLBAR_ICONS);

	gtk_window_resize(myMainWindow, myWidthOption.value(), myHeightOption.value());
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	ZLWin32SignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKeyEvent), this);
	ZLWin32SignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "scroll_event", G_CALLBACK(handleScrollEvent), this);
*/
	int nCmdShow = 10;
	HINSTANCE hInstance = GetModuleHandle(0);

	HWND hMainHwnd;
	char szClassName[] = "MyClass";

	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = szClassName;
	wc.hIconSm = LoadIcon(0, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		//std::cerr << "cannot register class\n";
		//return 0;
	}

	hMainHwnd = CreateWindow(szClassName, "Hello Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, (HWND)0, (HMENU)0, hInstance, 0);
	if (hMainHwnd == 0) {
		//std::cerr << "cannot create main window\n";
		//return 0;
	}

	ShowWindow(hMainHwnd, nCmdShow);
}

ZLWin32ApplicationWindow::~ZLWin32ApplicationWindow() {
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

void ZLWin32ApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	GtkToggleButton *gtkButton = GTK_TOGGLE_BUTTON(myButtonToWidget[&(ZLApplication::Toolbar::Item&)button]);
	const bool isPressed = button.isPressed();
	if (gtk_toggle_button_get_active(gtkButton) != isPressed) {
		gtk_toggle_button_set_active(gtkButton, isPressed);
	}
}

void ZLWin32ApplicationWindow::onGtkButtonPress(GtkWidget *gtkButton) {
	onButtonPress((ZLApplication::Toolbar::ButtonItem&)*myWidgetToButton[gtkButton]);
}
*/

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
	/*
	if (item->isButton()) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		static std::string imagePrefix = ZLApplication::ImageDirectory() + ZLApplication::PathDelimiter + ZLApplication::ApplicationName() + ZLApplication::PathDelimiter;
		GtkWidget *image = gtk_image_new_from_file((imagePrefix + buttonItem.iconName() + ".png").c_str());
		GtkWidget *button = buttonItem.isToggleButton() ? gtk_toggle_button_new() : gtk_button_new();
		gtk_button_set_relief((GtkButton*)button, GTK_RELIEF_NONE);
		GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
		gtk_container_add(GTK_CONTAINER(button), image);
		gtk_container_add(GTK_CONTAINER(myToolbar), button);
		ZLWin32SignalUtil::connectSignal(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(onButtonClicked), this);
		myButtonToWidget[&*item] = button;
		myWidgetToButton[button] = item;
		gtk_widget_show_all(button);
	}
	*/
}

/*
void ZLWin32ApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	GtkWidget *gtkButton = myButtonToWidget[&*item];
	if (gtkButton != 0) {
		if (visible) {
			gtk_widget_show(gtkButton);
		} else {
			gtk_widget_hide(gtkButton);
		}
		/ *
		 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
		 * does something strange if WIDGET is already insensitive.
		 * /
		bool alreadyEnabled = GTK_WIDGET_STATE(gtkButton) != GTK_STATE_INSENSITIVE;
		if (enabled != alreadyEnabled) {
			gtk_widget_set_sensitive(gtkButton, enabled);
		}
	}
}
*/

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
	return 0;
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
