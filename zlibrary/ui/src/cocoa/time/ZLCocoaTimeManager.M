/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#import <Foundation/Foundation.h>

#include "ZLCocoaTimeManager.h"

@interface CocoaRunner : NSObject {
}

- (void)run:(NSTimer*)theTimer;

@end

@implementation CocoaRunner

- (void)run:(NSTimer*)theTimer {
	shared_ptr<ZLRunnable> task =
		((ZLCocoaTimeManager&)ZLTimeManager::Instance()).taskByTimer(theTimer);
	if (!task.isNull()) {
		task->run();
	}
}

@end

ZLCocoaTimeManager::ZLCocoaTimeManager() {
	myRunner = [CocoaRunner new];
}

ZLCocoaTimeManager::~ZLCocoaTimeManager() {
	for (std::map<shared_ptr<ZLRunnable>,void*>::iterator it = myTimers.begin(); it != myTimers.end(); ++it) {
		[(NSTimer*)it->second invalidate];
	}
}

void ZLCocoaTimeManager::addTask(shared_ptr<ZLRunnable> task, int interval) {
	removeTask(task);
	if ((interval > 0) && !task.isNull()) {
		NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:interval/1000.0
																											target:(CocoaRunner*)myRunner
																										selector:@selector(run:)
																										userInfo:nil
																										 repeats:YES];
		myTimers[task] = timer;
		myTasks[timer] = task;
	}
}

void ZLCocoaTimeManager::removeTaskInternal(shared_ptr<ZLRunnable> task) {
	std::map<shared_ptr<ZLRunnable>,void*>::iterator it = myTimers.find(task);
	if (it != myTimers.end()) {
		[(NSTimer*)it->second invalidate];
		myTasks.erase(it->second);
		myTimers.erase(it);
	}
}

shared_ptr<ZLRunnable> ZLCocoaTimeManager::taskByTimer(void *timer) const {
	std::map<void*,shared_ptr<ZLRunnable> >::const_iterator it = myTasks.find(timer);
	return (it != myTasks.end()) ? it->second : 0;
}
