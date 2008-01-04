/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include "ZLGtkSignalUtil.h"

std::vector<std::pair<GtkObject*,int> > ZLGtkSignalUtil::ourConnectedSignals;

void ZLGtkSignalUtil::connectSignal(GtkObject *object, const char *name, GtkSignalFunc function, void *data) {
	int handlerId = gtk_signal_connect(object, name, function, data);
	ourConnectedSignals.push_back(std::pair<GtkObject*,int>(object, handlerId));
}

void ZLGtkSignalUtil::removeAllSignals() {
	for (std::vector<std::pair<GtkObject*,int> >::const_iterator it = ourConnectedSignals.begin(); it != ourConnectedSignals.end(); ++it) {
		gtk_signal_disconnect(it->first, it->second);
	}
}
