/*
 * FBReader -- electronic book reader
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

#ifndef __SCROLLINGOPTIONSPAGE_H__
#define __SCROLLINGOPTIONSPAGE_H__

#include "FBReader.h"

class ZLDialogContent;
class ZLSpinOptionEntry;
class ZLComboOptionEntry;

class ScrollingOptionsPage {

public:
  ScrollingOptionsPage(ZLDialogContent &dialogTab, FBReader &fbreader);
  ~ScrollingOptionsPage();

private:

  struct ScrollingEntries {
    ZLSpinOptionEntry *myDelayEntry;
    ZLComboOptionEntry *myModeEntry;
    ZLSpinOptionEntry *myLinesToKeepEntry;
    ZLSpinOptionEntry *myLinesToScrollEntry;
    ZLSpinOptionEntry *myPercentToScrollEntry;

    void init(FBReader::ScrollingOptions &options);
    void connect(ZLDialogContent &dialogTab);
    void show(bool visible);
  };

  ScrollingEntries myLargeScrollingEntries;
  ScrollingEntries mySmallScrollingEntries;
  ScrollingEntries myMouseScrollingEntries;

friend class MainEntry;
friend class ScrollingModeEntry;
};

inline ScrollingOptionsPage::~ScrollingOptionsPage() {}

#endif /* __SCROLLINGOPTIONSPAGE_H__ */
