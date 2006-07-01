/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLDEVICEINFO_H__
#define __ZLDEVICEINFO_H__

class ZLDeviceInfo {

public:
	enum ScreenSize {
		SIZE_DESKTOP,
		SIZE_160x160,
		SIZE_240x320,
		SIZE_320x320,
		SIZE_640x480,
		SIZE_800x480,
	};

	static ScreenSize screenSize();

	static bool isKeyboardPresented();
	static bool isMousePresented();

	static void deleteInstance();

private:
	static unsigned int screenWidth();
	static unsigned int screenHeight();

protected:
	static ZLDeviceInfo *ourInstance;

	ZLDeviceInfo();
	virtual ~ZLDeviceInfo();
	virtual unsigned int screenWidthInternal() const = 0;
	virtual unsigned int screenHeightInternal() const = 0;
	virtual bool isKeyboardPresentedInternal() const = 0;
};

inline unsigned int ZLDeviceInfo::screenWidth() { return ourInstance->screenWidthInternal(); }
inline unsigned int ZLDeviceInfo::screenHeight() { return ourInstance->screenHeightInternal(); }
inline bool ZLDeviceInfo::isKeyboardPresented() { return ourInstance->isKeyboardPresentedInternal(); }

#endif /* __ZLDEVICEINFO_H__ */
