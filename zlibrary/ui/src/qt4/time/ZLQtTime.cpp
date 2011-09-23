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

#include "ZLQtTime.h"
#include <QtCore/QThread>

void ZLQtTimeManager::addTask(shared_ptr<ZLRunnable> task, int interval) {
	if (thread() != QThread::currentThread()) {
		QMetaObject::invokeMethod(this, "addTask", Qt::QueuedConnection,
		                          Q_ARG(shared_ptr<ZLRunnable>, task),
		                          Q_ARG(int, interval));
	} else {
		removeTask(task);
		if ((interval > 0) && !task.isNull()) {
			int id = startTimer(interval);
			myTimers[task] = id;
			myTasks[id] = task;
		}
	}
}

void ZLQtTimeManager::removeTaskInternal(shared_ptr<ZLRunnable> task) {
	if (thread() != QThread::currentThread()) {
		QMetaObject::invokeMethod(this, "removeTaskInternal", Qt::QueuedConnection,
		                          Q_ARG(shared_ptr<ZLRunnable>, task));
	} else {
		QMap<shared_ptr<ZLRunnable>,int>::iterator it = myTimers.find(task);
		if (it != myTimers.end()) {
			killTimer(it.value());
			myTasks.remove(it.value());
			myTimers.erase(it);
		}
	}
}

void ZLQtTimeManager::timerEvent(QTimerEvent *event) {
	myTasks[event->timerId()]->run();
}
