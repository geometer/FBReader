#include <abstract/ZLOptions.h>
#include <palm/ZLPalmFSManager.h>
#include <abstract/ZLInputStream.h>

#include "PalmFBReader.h"
#include "PalmPaintContext.h"
#include "PalmFBReader-resources.h"

static Boolean MainFBReaderFormHandleEvent(EventPtr event) {
	switch (event->eType) {
  	case menuCmdBarOpenEvent:	
			MenuCmdBarAddButton(menuCmdBarOnRight, OpenBook, menuCmdBarResultMenuItem, OpenBook, "");
			MenuCmdBarAddButton(menuCmdBarOnRight, AddBook, menuCmdBarResultMenuItem, AddBook, "");
			return false;

   	case penDownEvent:
			{
				char txt[4];
				int x = event->screenX;
				txt[0] = '0' + x / 100;
				txt[1] = '0' + x / 10 % 10;
				txt[2] = '0' + x % 10;
				txt[3] = '\0';
				FrmCustomAlert(GoodnightMoonAlert, txt, 0, 0);
			}
			return true;
  	
  	case frmOpenEvent:	
			{
				FrmDrawForm(FrmGetActiveForm());
				WinSetCoordinateSystem(kCoordinatesNative);
				PalmPaintContext context;
				context.setSize(320, 320);
				ZLColorOption foreColorOption("Color", "Foreground", ZLColor(0, 0, 255));
				context.setColor(foreColorOption.value());
				PaintContext::BackgroundColorOption.setValue(ZLColor(255, 255, 0));
				context.clear();

				context.setFillColor(ZLColor(0, 255, 0));
				context.fillRectangle(100, 100, 110, 110);

				std::string fileName = "/test1.zip:test1";
				//std::string fileName = "/test1";
				ZLInputStream *istream = ZLPalmFSManager::instance().createInputStream(fileName);
				if (istream != 0) {
					if (istream->open()) {
						char txt[10];
						int size = istream->read(txt, 6);
						context.drawString(10, 10, txt, 0, size);
						istream->close();
					}
					delete istream;
				}

				context.fillRectangle(110, 110, 120, 120);

				int barLeft = 0;
				int barRight = context.width() - 1;
				int barBottom = context.height() - 1;
				int barTop = barBottom - context.height() / 20;
				context.drawLine(barLeft, barTop, barLeft, barBottom);
				context.drawLine(barRight, barTop, barRight, barBottom);
				context.drawLine(barLeft, barTop, barRight, barTop);
				context.drawLine(barLeft, barBottom, barRight, barBottom);
				WinSetCoordinateSystem(kCoordinatesStandard);
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
