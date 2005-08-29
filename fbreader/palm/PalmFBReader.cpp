/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLOptions.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLDeviceInfo.h>
#include <abstract/ZLTime.h>
#include <abstract/ZLStringUtil.h>

#include <palm/ZLPalmFSManager.h>
#include <palm/PalmViewWidget.h>
#include <palm/PalmPaintContext.h>

#include "PalmFBReader.h"
#include "PalmFBReader-resources.h"

PalmFBReader::PalmFBReader() : FBReader(new PalmPaintContext(), std::string()) {
	myViewWidget = new PalmViewWidget(MainFBReaderForm);
	setMode(BOOK_TEXT_MODE);
}

PalmFBReader::~PalmFBReader() {
	delete (PalmViewWidget*)myViewWidget;
}

void PalmFBReader::setWindowCaption(const std::string &caption) {}
void PalmFBReader::addButton(ActionCode id, const std::string &name) {}
void PalmFBReader::addButtonSeparator() {}
void PalmFBReader::setButtonVisible(ActionCode id, bool visible) {}
void PalmFBReader::setButtonEnabled(ActionCode id, bool enable) {}
void PalmFBReader::searchSlot() {}
void PalmFBReader::quitSlot() {}
void PalmFBReader::toggleFullscreenSlot() {}
bool PalmFBReader::isFullscreen() const { return false; }
bool PalmFBReader::isRotationSupported() const { return false; }
void PalmFBReader::stylusPressEvent(int x, int y) {
	ZLView *view = myViewWidget->view();
	x -= view->context().leftMargin();
	y -= view->context().topMargin();
	view->onStylusPress(x, y);
}
void PalmFBReader::paintEvent() {
	((PalmViewWidget*)myViewWidget)->paintEvent();
}

static Boolean MainFBReaderFormHandleEvent(EventPtr event) FB_SECTION;
static Boolean ApplicationHandleEvent(EventPtr event) FB_SECTION;
static Boolean OptionsDialogFormHandleEvent(EventPtr event) FB_SECTION;

static Boolean MainFBReaderFormHandleEvent(EventPtr event) {
	switch (event->eType) {
  	case menuCmdBarOpenEvent:	
			MenuCmdBarAddButton(menuCmdBarOnRight, OpenBook, menuCmdBarResultMenuItem, OpenBook, "");
			MenuCmdBarAddButton(menuCmdBarOnRight, AddBook, menuCmdBarResultMenuItem, AddBook, "");
			return false;

		case keyDownEvent:
			{
				ZLTime start;
				switch (event->data.keyDown.chr) {
					case pageUpChr:
						DO_PAINT = true;
						READER->doAction(FBReader::ACTION_LARGE_SCROLL_BACKWARD);
						DO_PAINT = false;
						break;
					case pageDownChr:
						DO_PAINT = true;
						READER->doAction(FBReader::ACTION_LARGE_SCROLL_FORWARD);
						DO_PAINT = false;
						break;
				}
				ZLTime end;
				unsigned long timeSpent = end.millisecondsFrom(start);
				std::string timeS;
				ZLStringUtil::appendNumber(timeS, timeSpent);
				FrmCustomAlert(GoodnightMoonAlert, timeS.c_str(), 0, 0);
			}
			return true;

   	case penDownEvent:
			{
				int x = event->screenX;
				int y = event->screenY;
				if (ZLDeviceInfo::screenSize() == ZLDeviceInfo::SIZE_320x320) {
					x *= 2;
					y *= 2;
				}
				DO_PAINT = true;
				READER->stylusPressEvent(x, y);
				DO_PAINT = false;
				/*
				char txt[4];
				//std::string s = "$$TEST";
				//std::string t = s;
				//int x = (s == t) ? 1 : 0;
				//int x = (s == "$$TEST") ? 1 : 0;
				txt[0] = '0' + x / 100 % 10;
				txt[1] = '0' + x / 10 % 10;
				txt[2] = '0' + x % 10;
				txt[3] = '\0';
				FrmCustomAlert(GoodnightMoonAlert, txt, 0, 0);
				*/
			}
			return true;
  	
  	case frmUpdateEvent:	
  	case frmOpenEvent:	
			{
				DO_PAINT = true;
				READER->paintEvent();
				DO_PAINT = false;
			}
			return true;

		case menuEvent:
			switch (event->data.menu.itemID) {
				case OpenBook:
				case AddBook:
					FrmGotoForm(OptionsDialogForm);
					break;
			}
			return false;

		case winDisplayChangedEvent:
			FrmCustomAlert(GoodnightMoonAlert, "WDCE", 0, 0);
			return false;

		default:
			return false;
	}
}

static char *TEXT = "item";

static Boolean OptionsDialogFormHandleEvent(EventPtr event) {
	switch (event->eType) {
  	
  	case frmOpenEvent:	
			{
				FormPtr form = FrmGetActiveForm();
				TablePtr table = (TablePtr)FrmGetObjectPtr(form, FrmGetObjectIndex(form, OptionsTable));
				for (int i = 0; i < 5; i++) {
					for (int j = 0; j < 5; j++) {
						TblSetItemStyle(table, i, j, labelTableItem);
						TblSetItemPtr(table, i, j, TEXT);
					}
					TblSetColumnUsable(table, i, true);
				}
				FrmDrawForm(form);
			}
			return true;

		case penDownEvent:
			FrmGotoForm(MainFBReaderForm);
			return true;

		default:
			return false;
	}
}

static Boolean ApplicationHandleEvent(EventPtr event) {
	FormPtr	frm;
	Int16 formId;

	if (event->eType == frmLoadEvent) {
		// Load the form resource specified in the event then activate the form.
		formId = event->data.frmLoad.formID;
		frm = FrmInitForm(formId);
		FrmSetActiveForm(frm);

		// Set the event handler for the form.  The handler of the currently 
		// active form is called by FrmDispatchEvent each time it receives an event.
		switch (formId) {
			case MainFBReaderForm:
				FrmSetEventHandler(frm, MainFBReaderFormHandleEvent);
				break;
			case OptionsDialogForm:
				FrmSetEventHandler(frm, OptionsDialogFormHandleEvent);
				break;
		}
		return true;
	}
	
	return false;
}

void EventLoop(void) {
	EventType event;
	UInt16 error;
	
	do {
		EvtGetEvent(&event, evtWaitForever);
		if (!SysHandleEvent(&event) && !MenuHandleEvent(0, &event, &error) && !ApplicationHandleEvent(&event)) {
			FrmDispatchEvent(&event);
		}
	} while (event.eType != appStopEvent);
}
