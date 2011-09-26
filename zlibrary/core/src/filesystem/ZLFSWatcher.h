/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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

#ifndef ZLFSWATCHER_H
#define ZLFSWATCHER_H

#include <string>
#include <shared_ptr.h>

class ZLFSWatcher {
public:
	virtual ~ZLFSWatcher();
	virtual void onPathChanged(const std::string &path) = 0;
	
	static void addWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher);
	static void removeWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher);
};

#endif // ZLFSWATCHER_H
