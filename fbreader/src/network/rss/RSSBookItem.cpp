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

#include "RSSBookItem.h"
#include "../NetworkLink.h"

RSSBookItem::RSSBookItem(const RSSLink &link, RSSItem &item, unsigned int index) :
    NetworkBookItem(
        link,
        item.id()->uri(),
        index,
        item.title(),
        item.summary(),
        item.lang(),
        item.userData("pubdate"),
        getAuthors(item),
        std::vector<std::string>(),
        "none_"+index,
        0,
        getUrls(item),
        getReferences(item)
    ){
}

std::vector<shared_ptr<BookReference> >  RSSBookItem::getReferences(RSSItem &item) {

    std::vector<shared_ptr<BookReference> > references;
    for (std::size_t i = 0; i < item.links().size(); ++i) {
        ATOMLink &link = *(item.links()[i]);
        const std::string href = link.href();
        shared_ptr<ZLMimeType> type = ZLMimeType::get(link.type());
        const std::string &rel = link.rel();
        if(type == ZLMimeType::EMPTY && rel == OPDSConstants::REL_ACQUISITION_DOWNLOAD_IN_BROWSER){
            references.push_back(new BuyBookReference(
                                 href, BookReference::NONE, BookReference::DOWNLOAD_IN_BROWSER, ""
                                 ));
        }
    }
    return references;
}

NetworkItem::UrlInfoCollection RSSBookItem::getUrls(RSSItem &item) {
    //TODO: Refactor it

    NetworkItem::UrlInfoCollection urlMap;
    if(item.links().size() > 0){
        for (std::size_t i = 0; i < item.links().size(); ++i) {
            ATOMLink &link = *(item.links()[i]);
            urlMap[NetworkItem::URL_SINGLE_ENTRY] = link.href();
        }
    }
    return urlMap;
}

std::vector<NetworkBookItem::AuthorData> RSSBookItem::getAuthors(RSSItem &item) {
    std::vector<NetworkBookItem::AuthorData> authors;

    for (std::size_t i = 0; i < item.authors().size(); ++i) {
        NetworkBookItem::AuthorData authorData;
        ATOMAuthor &author = *(item.authors()[i]);
        std::string name = author.name();
        authorData.DisplayName = name;
        authors.push_back(authorData);
    }
    return authors;
}
