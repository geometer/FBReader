#include <PalmOS.h>				
#include <PalmTypes.h>				
#include <PalmCompatibility.h>				

#include "PalmFBReader-resources.h"

static Err StartApplication(void) {
	FrmGotoForm(MainFBReaderForm);
	return 0;
}


static void StopApplication(void) {
}


static Boolean MyFormHandleEvent(EventPtr event) {
	switch (event->eType) {
  	case menuCmdBarOpenEvent:	
			MenuCmdBarAddButton(menuCmdBarOnRight, OpenBookBitmap, menuCmdBarResultMenuItem, 5000, NULL);
			MenuCmdBarAddButton(menuCmdBarOnRight, AddBookBitmap, menuCmdBarResultMenuItem, 5001, NULL);
			return false;

   	case penDownEvent:  // A control button was pressed and released.
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
				RGBColorType red = { 0x00, 0xff, 0x00, 0xff };
				RGBColorType yellow = { 0x00, 0xff, 0xff, 0xa0 };
				RGBColorType blue = { 0x00, 0x00, 0x00, 0x80 };
				IndexedColorType iRed = WinRGBToIndex(&red);
				IndexedColorType iYellow = WinRGBToIndex(&yellow);
				IndexedColorType iBlue = WinRGBToIndex(&blue);
				WinSetTextColor(iRed);
				WinSetBackColor(iYellow);
				WinSetForeColor(iBlue);
				RectangleType rectangle;
				WinGetWindowFrameRect(WinGetActiveWindow(), &rectangle);
				WinFillRectangle(&rectangle, 0);
				WinDrawChars("Hello", 5, 10, 10);

				int barLeft = 0;//rectangle.topLeft.x;
				int barRight = 319;//rectangle.extent.x;
				int barBottom = 319;//rectangle.extent.y;
				int barTop = barBottom - 16;
				WinDrawLine(barLeft, barTop, barLeft, barBottom);
				WinDrawLine(barRight, barTop, barRight, barBottom);
				WinDrawLine(barLeft, barTop, barRight, barTop);
				WinDrawLine(barLeft, barBottom, barRight, barBottom);
				WinSetCoordinateSystem(kCoordinatesStandard);
			}
			return true;
	}
	return false;
}


static Boolean ApplicationHandleEvent(EventPtr event) {
	FormPtr	frm;
	Int		formId;

	if (event->eType == frmLoadEvent) {
		// Load the form resource specified in the event then activate the form.
		formId = event->data.frmLoad.formID;
		frm = FrmInitForm(formId);
		FrmSetActiveForm(frm);

		// Set the event handler for the form.  The handler of the currently 
		// active form is called by FrmDispatchEvent each time it receives an event.
		switch (formId) {
			case MainFBReaderForm:
				FrmSetEventHandler(frm, MyFormHandleEvent);
				break;
		}
		return true;
	}
	
	return false;
}


static void EventLoop(void) {
	EventType event;
	Word error;
	
	do {
		EvtGetEvent(&event, evtWaitForever);
		if (!SysHandleEvent(&event) && !MenuHandleEvent(0, &event, &error) && !ApplicationHandleEvent(&event)) {
			FrmDispatchEvent(&event);
		}
	} while (event.eType != appStopEvent);
}


DWord PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags) {
	Err err = 0;
	
	if ((cmd == sysAppLaunchCmdNormalLaunch) && ((err = StartApplication()) == 0)) {
		EventLoop();
		StopApplication();
	}

	return err;
}
