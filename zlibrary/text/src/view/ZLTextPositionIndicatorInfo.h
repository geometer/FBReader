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

#ifndef __ZLTEXTPOSITIONINDICATORINFO_H__
#define __ZLTEXTPOSITIONINDICATORINFO_H__

#include <shared_ptr.h>

#include <ZLOptions.h>

class ZLTextPositionIndicatorInfo {

protected:
	ZLTextPositionIndicatorInfo();

public:
	virtual ~ZLTextPositionIndicatorInfo();

	enum Type {
		OS_SCROLLBAR = 0,
		FB_INDICATOR = 1,
		NONE = 2
	};

	virtual Type type() const = 0;
	virtual bool isSensitive() const = 0;
	virtual bool isTextPositionShown() const = 0;
	virtual bool isTimeShown() const = 0;
	virtual ZLColor color() const = 0;
	virtual int height() const = 0;
	virtual int offset() const = 0;
	virtual int fontSize() const = 0;

private:
	ZLTextPositionIndicatorInfo(const ZLTextPositionIndicatorInfo&);
	const ZLTextPositionIndicatorInfo &operator = (const ZLTextPositionIndicatorInfo&);
};

#endif /* __ZLTEXTPOSITIONINDICATORINFO_H__ */
