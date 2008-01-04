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

#ifndef __ZLTIME_H__
#define __ZLTIME_H__

#include <shared_ptr.h>
#include <ZLRunnable.h>

class ZLTime {

public:
	ZLTime();
	ZLTime(long seconds, long milliseconds);
	~ZLTime();
	long millisecondsTo(const ZLTime &time) const;
	long millisecondsFrom(const ZLTime &time) const;
	short hours() const;
	short minutes() const;

private:
	long mySeconds;
	long myMilliseconds;

friend class ZLTimeManager;
};

class ZLTimeManager {

public:
	static ZLTimeManager &instance();
	static void deleteInstance();
	virtual void addTask(shared_ptr<ZLRunnable> task, int interval) = 0;
	virtual void removeTask(shared_ptr<ZLRunnable> task) = 0;
	
protected:
	ZLTimeManager();
	virtual ~ZLTimeManager();
	virtual ZLTime currentTime() const = 0;
	virtual short hoursBySeconds(long seconds) const = 0;
	virtual short minutesBySeconds(long seconds) const = 0;

protected:
	static ZLTimeManager *ourInstance;

friend class ZLTime;
};

#endif /* __ZLTIME_H__ */
