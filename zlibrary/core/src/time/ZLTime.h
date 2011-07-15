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

#ifndef __ZLTIME_H__
#define __ZLTIME_H__

class ZLTime {

public:
	ZLTime();
	ZLTime(long seconds, long milliseconds);
	~ZLTime();
	long millisecondsTo(const ZLTime &time) const;
	long millisecondsFrom(const ZLTime &time) const;
	short hours() const;
	short minutes() const;

	short dayOfMonth() const; // from 1 to 31
	short month() const; // from 1 to 12
	short year() const;

private:
	long mySeconds;
	long myMilliseconds;
};

#endif /* __ZLTIME_H__ */
