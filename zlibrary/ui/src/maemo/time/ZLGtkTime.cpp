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

#include <glib.h>

#include "ZLGtkTime.h"
#include "../dialogs/GtkDialogManager.h"

static bool taskFunction(gpointer *data) {
	if (!((GtkDialogManager&)GtkDialogManager::instance()).isWaiting()) {
		((ZLRunnable*)data)->run();
	}
	return true;
}

void ZLGtkTimeManager::addTask(shared_ptr<ZLRunnable> task, int interval) {
	if ((interval > 0) && !task.isNull()) {
		myHandlers[task] = g_timeout_add(interval, (GSourceFunc)taskFunction, &*task);
	}
}

void ZLGtkTimeManager::removeTask(shared_ptr<ZLRunnable> task) {
	std::map<shared_ptr<ZLRunnable>,int>::iterator it = myHandlers.find(task);
	if (it != myHandlers.end()) {
		g_source_remove(it->second);
		myHandlers.erase(it);
	}
}
