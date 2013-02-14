#include "RSSBookItem.h"
#include "../NetworkLink.h"

#include <iostream> //udmv

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
