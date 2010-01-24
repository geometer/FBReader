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

#include "ZLTimeManager.h"

ZLTimeManager *ZLTimeManager::ourInstance = 0;

ZLTimeManager &ZLTimeManager::Instance() {
	return *ourInstance;
}

void ZLTimeManager::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
		ourInstance = 0;
	}
}

ZLTimeManager::ZLTimeManager() {
}

ZLTimeManager::~ZLTimeManager() {
}

class ZLTimeManager::AutoRemovableTask : public ZLRunnable {

public:
	AutoRemovableTask(shared_ptr<ZLRunnable> task);

private:
	void run();

private:
	shared_ptr<ZLRunnable> myTask;
};

ZLTimeManager::AutoRemovableTask::AutoRemovableTask(shared_ptr<ZLRunnable> task) : myTask(task) {
}

void ZLTimeManager::AutoRemovableTask::run() {
	ZLTimeManager &timeManager = ZLTimeManager::Instance();
	std::map<shared_ptr<ZLRunnable>,shared_ptr<ZLRunnable> > &taskList = timeManager.myAutoRemovableTasks;
	std::map<shared_ptr<ZLRunnable>,shared_ptr<ZLRunnable> >::iterator it = taskList.begin();
	for (; it != taskList.end(); ++it) {
		if (this == &*it->second) {
			break;
		}
	}
	if (it != taskList.end()) {
		shared_ptr<ZLRunnable> task = myTask;
		timeManager.removeTaskInternal(it->second);
		taskList.erase(it);
		task->run();
	}
}

void ZLTimeManager::addAutoRemovableTask(shared_ptr<ZLRunnable> task, int delay) {
	shared_ptr<ZLRunnable> wrapper = new AutoRemovableTask(task);
	myAutoRemovableTasks[task] = wrapper;
	addTask(wrapper, delay > 0 ? delay : 1);
}

void ZLTimeManager::removeTask(shared_ptr<ZLRunnable> task) {
	std::map<shared_ptr<ZLRunnable>,shared_ptr<ZLRunnable> >::iterator it =
		myAutoRemovableTasks.find(task);
	if (it == myAutoRemovableTasks.end()) {
		removeTaskInternal(task);
	} else {
		removeTaskInternal(it->second);
		myAutoRemovableTasks.erase(it);
	}
}
