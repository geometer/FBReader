/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include "ZLWin32Time.h"
#include "../application/ZLWin32ApplicationWindow.h"

ZLWin32TimeManager::ZLWin32TimeManager() : myMaxId(0) {
}

void ZLWin32TimeManager::execute(UINT taskId) {
	shared_ptr<ZLRunnable> task = myTasks[taskId];
	if (!task.isNull()) {
		task->run();
	}
}

void ZLWin32TimeManager::addTask(shared_ptr<ZLRunnable> task, int interval) {
	if (ZLWin32ApplicationWindow::ourApplicationWindow != 0) {
		HWND window = ZLWin32ApplicationWindow::ourApplicationWindow->myMainWindow;
		if (window != 0) {
			SetTimer(window, ++myMaxId, interval, 0);
			myTasks[myMaxId] = task;
			myIds[task] = myMaxId;
		}
	}
}

void ZLWin32TimeManager::removeTask(shared_ptr<ZLRunnable> task) {
	if (ZLWin32ApplicationWindow::ourApplicationWindow != 0) {
		HWND window = ZLWin32ApplicationWindow::ourApplicationWindow->myMainWindow;
		if (window != 0) {
			UINT id = myIds[task];
			KillTimer(window, id);
			myIds.erase(task);
			myTasks.erase(id);
		}
	}
}
