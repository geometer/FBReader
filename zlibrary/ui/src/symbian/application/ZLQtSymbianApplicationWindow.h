/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTSYMBIANAPPLICATIONWINDOW_H__
#define __ZLQTSYMBIANAPPLICATIONWINDOW_H__


#include <ZLRunnable.h>
#include <unistd.h>

#include "ZLQtApplicationWindow.h"

class VolumeKeysCapturer;

class ZLQtSymbianApplicationWindow : public ZLQtApplicationWindow {
        Q_OBJECT

public:

        ZLQtSymbianApplicationWindow(ZLApplication *application);
        ~ZLQtSymbianApplicationWindow();

public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };
    void setOrientation(ScreenOrientation orientation);

private:
    ZLViewWidget *createViewWidget();


private:
        VolumeKeysCapturer* myVolumeKeyCapture;

};

#include <ZLExecutionData.h>
class TestRunnable : public ZLRunnable {

public:
        void run() {
            for (int i=0; i<10;++i) {
                usleep(10000);
            }

        }      
};

#endif /* __ZLQTSYMBIANAPPLICATIONWINDOW_H__ */
