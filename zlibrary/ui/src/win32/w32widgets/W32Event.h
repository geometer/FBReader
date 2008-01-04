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

#ifndef __W32EVENT_H__
#define __W32EVENT_H__

#include <string>
#include <list>

class W32EventSender;

class W32Listener {

protected:
	W32Listener();

public:
	virtual ~W32Listener();
	virtual void onEvent(const std::string &event, W32EventSender &sender) = 0;
};

class W32EventSender {

public:
	W32EventSender();
	void addListener(W32Listener *listener);
	void removeListener(W32Listener *listener);

protected:
	void fireEvent(const std::string &event);

private:
	std::list<W32Listener*> myListeners;
};

#endif /* __W32EVENT_H__ */
