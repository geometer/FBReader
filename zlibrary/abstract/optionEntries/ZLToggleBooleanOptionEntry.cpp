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

#include "ZLToggleBooleanOptionEntry.h"

ZLToggleBooleanOptionEntry::ZLToggleBooleanOptionEntry(const std::string &name, ZLBooleanOption &option, const Entries &slaveEntries) : ZLSimpleBooleanOptionEntry(name, option), mySlaveEntries(slaveEntries) {
}

ZLToggleBooleanOptionEntry::ZLToggleBooleanOptionEntry(const std::string &name, ZLBooleanOption &option, ZLOptionEntry *slave0, ZLOptionEntry *slave1, ZLOptionEntry *slave2) : ZLSimpleBooleanOptionEntry(name, option) {
	if (slave0 != 0) mySlaveEntries.push_back(slave0);
	if (slave1 != 0) mySlaveEntries.push_back(slave1);
	if (slave2 != 0) mySlaveEntries.push_back(slave2);
}

void ZLToggleBooleanOptionEntry::onStateChanged(bool state) {
	for (Entries::const_iterator it = mySlaveEntries.begin(); it != mySlaveEntries.end(); ++it) {
		(*it)->setVisible(state);
	}
}
