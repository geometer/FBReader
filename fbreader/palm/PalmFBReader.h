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

#ifndef __PALMFBREADER_H__
#define __PALMFBREADER_H__

#include <PalmOS.h>				

#include "../common/fbreader/FBReader.h"

void EventLoop() FB_SECTION;

class PalmPaintContext;

class PalmFBReader : public FBReader {

public:
	PalmFBReader() FB_SECTION;
	~PalmFBReader() FB_SECTION;
	void setWindowCaption(const std::string &caption) FB_SECTION;
	void addButton(ActionCode id, const std::string &name) FB_SECTION;
	void addButtonSeparator() FB_SECTION;
	void setButtonVisible(ActionCode id, bool visible) FB_SECTION;
	void setButtonEnabled(ActionCode id, bool enable) FB_SECTION;
	void searchSlot() FB_SECTION;
	void toggleFullscreenSlot() FB_SECTION;
	bool isFullscreen() const FB_SECTION;
	void quitSlot() FB_SECTION;
	bool isRotationSupported() const FB_SECTION;
	void stylusPressEvent(int x, int y) FB_SECTION;
	void paintEvent() FB_SECTION;
};

extern PalmFBReader *READER;

#endif /* __PALMFBREADER_H__ */
