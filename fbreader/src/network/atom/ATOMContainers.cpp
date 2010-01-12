/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include "ATOMContainers.h"

ATOMEntry::ATOMEntry() {
}

ATOMEntry::ATOMEntry(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated) : 
	myId(id), myTitle(title), myUpdated(updated) {
}



ATOMFeedMetadata::ATOMFeedMetadata() {
}

ATOMFeedMetadata::ATOMFeedMetadata(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated) : 
	myId(id), myTitle(title), myUpdated(updated) {
}



/*
ATOMFeed::ATOMFeed() {
}

ATOMFeed::ATOMFeed(const ATOMId &id, const std::string &title, shared_ptr<ATOMUpdated> updated) : 
	ATOMFeedMatadata(id, title, updated) {
}
*/


