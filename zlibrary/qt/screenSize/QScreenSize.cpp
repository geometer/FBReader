#include <qapplication.h>

#include "QScreenSize.h"

ZLScreenSize::Size QScreenSize::getSizeInternal() const {
	QWidget *desktop = qApp->desktop();
	int w = desktop->width();
	int h = desktop->height();
	if (w * h == 640 * 480) {
		return SIZE_640x480;
	}
	return SIZE_DESKTOP;
}
