/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ZLTIME_H__
#define __ZLTIME_H__

class ZLTime {

public:
	ZLTime() UTIL_SECTION;
	ZLTime(long seconds, long milliseconds) UTIL_SECTION;
	~ZLTime() UTIL_SECTION;
	long millisecondsTo(const ZLTime &time) const UTIL_SECTION;
	long millisecondsFrom(const ZLTime &time) const UTIL_SECTION;

private:
	long mySeconds;
	long myMilliseconds;

friend class ZLTimeManager;
};

class ZLTimeManager {

private:
	static ZLTimeManager &instance() UTIL_SECTION;

public:
	static void deleteInstance() UTIL_SECTION;
	
protected:
	ZLTimeManager() UTIL_SECTION;
	virtual ~ZLTimeManager() UTIL_SECTION;
	virtual ZLTime currentTime() const UTIL_SECTION = 0;
	
protected:
	static ZLTimeManager *ourInstance;

friend class ZLTime;
};

#endif /* __ZLTIME_H__ */
