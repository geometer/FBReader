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

#include "W32Event.h"

W32Listener::W32Listener() {
}

W32Listener::~W32Listener() {
}

W32EventSender::W32EventSender() {
}

void W32EventSender::addListener(W32Listener *listener) {
	if (listener != 0) {
		std::list<W32Listener*>::const_iterator it = std::find(myListeners.begin(), myListeners.end(), listener);
		if (it == myListeners.end()) {
			myListeners.push_back(listener);
		}
	}
}

void W32EventSender::removeListener(W32Listener *listener) {
	myListeners.remove(listener);
}

void W32EventSender::fireEvent(const std::string &event) {
	for (std::list<W32Listener*>::iterator it = myListeners.begin(); it != myListeners.end(); ++it) {
		(*it)->onEvent(event, *this);
	}
}
