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

#include "ZLTime.h"
#include "ZLTimeManager.h"

ZLTime::ZLTime() {
	*this = ZLTimeManager::Instance().currentTime();
}

ZLTime::ZLTime(long seconds, long milliseconds) : mySeconds(seconds), myMilliseconds(milliseconds) {
}

ZLTime::~ZLTime() {
}

long ZLTime::millisecondsTo(const ZLTime &time) const {
	return 1000 * (time.mySeconds - mySeconds) + time.myMilliseconds - myMilliseconds;
}

long ZLTime::millisecondsFrom(const ZLTime &time) const {
	return - millisecondsTo(time);
}

short ZLTime::hours() const {
	return ZLTimeManager::Instance().hoursBySeconds(mySeconds);
}

short ZLTime::minutes() const {
	return ZLTimeManager::Instance().minutesBySeconds(mySeconds);
}

short ZLTime::dayOfMonth() const {
	return ZLTimeManager::Instance().dayOfMonthBySeconds(mySeconds);
}

short ZLTime::month() const {
	return ZLTimeManager::Instance().monthBySeconds(mySeconds);
}

short ZLTime::year() const {
	return ZLTimeManager::Instance().yearBySeconds(mySeconds);
}
