#include "RSSBookItem.h"
#include <iostream> //udmv

RSSBookItem::RSSBookItem(const NetworkLink &link, RSSItem &item, unsigned int index) :
    NetworkBookItem(
        link,
        item.id()->uri(),
        index,
        item.title(),
        item.summary(),
        item.lang(),
        "11-11-11",
        std::vector<AuthorData>(),
        std::vector<std::string>(),
        "ser_"+index,
        0,
        getUrls(item),
        std::vector<shared_ptr<BookReference> >()
    ){
}

NetworkItem::UrlInfoCollection RSSBookItem::getUrls(RSSItem &entry) {
    //TODO split urls and references in UrlInfoCollection, like it's implemented in FBReaderJ

    //Testing
    NetworkItem::UrlInfoCollection urlMap;
    for (std::size_t i = 0; i < entry.links().size(); ++i) {
        ATOMLink &link = *(entry.links()[i]);
        urlMap[NetworkItem::URL_SINGLE_ENTRY] = link.href();
    }
    return urlMap;
}
