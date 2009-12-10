/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLSLOWPROCESS_H__
#define __ZLSLOWPROCESS_H__

#include <shared_ptr.h>

class ZLSlowProcessListener;

class ZLSlowProcess {

protected:
	ZLSlowProcess();

public:
	virtual ~ZLSlowProcess();
	void setListener(shared_ptr<ZLSlowProcessListener> listener);
	virtual void onCancel() = 0;
	void setPercent(int ready, int full);

private:
	shared_ptr<ZLSlowProcessListener> myListener;

friend class ZLSlowProcessListener;
};

class ZLSlowProcessListener {

protected:
	ZLSlowProcessListener();

public:
	virtual ~ZLSlowProcessListener();

	void cancelProcess();
	virtual void showPercent(int ready, int full) = 0;

private:
	ZLSlowProcess *myProcess;

friend class ZLSlowProcess;
};

#endif /* __ZLSLOWPROCESS_H__ */
