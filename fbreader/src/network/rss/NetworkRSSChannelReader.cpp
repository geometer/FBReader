/*
 * Copyright (C) 2009-2013 Geometer Plus <contact@geometerplus.com>
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

#include "NetworkRSSChannelReader.h"

NetworkRSSChannelReader::NetworkRSSChannelReader(const RSSLink &link, const std::string &baseURL, NetworkOperationData &result) :
    myLink(link), myURL(baseURL), myData(result), myIndex(0){
}

void NetworkRSSChannelReader::processChannelItem(shared_ptr<RSSItem> item){
    if(item.isNull())
        return;

    RSSItem &myItem = *item;

    shared_ptr<NetworkItem> book;
    book = new RSSBookItem(myLink, myItem, myIndex++);
    myData.Items.push_back(book);
}
