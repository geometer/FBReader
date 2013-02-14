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
        item.userData("pubdate"),
        getAuthors(item),
        std::vector<std::string>(),
        "ser_"+index,
        0,
        getUrls(item),
        getReferences(item)
    ){
}

std::vector<shared_ptr<BookReference> >  RSSBookItem::getReferences(RSSItem &item) {
    //TODO: Refactor it

    std::vector<shared_ptr<BookReference> > references;
    if(item.links().size() > 0){
        ATOMLink &link = *(item.links()[0]);
        const std::string href = link.href();
        references.push_back(new BuyBookReference(
                                 href, BookReference::NONE, BookReference::BUY_IN_BROWSER, "0"
                                 ));
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
    /*for (std::size_t i = 0; i < entry.authors().size(); ++i) {
        ATOMAuthor &author = *(entry.authors()[i]);
        NetworkBookItem::AuthorData authorData;
        std::string name = author.name();
        std::string lowerCased = ZLUnicodeUtil::toLower(name);
        static const std::string authorPrefix = "author:";
        std::size_t index = lowerCased.find(authorPrefix);
        if (index != std::string::npos) {
            name = name.substr(index + authorPrefix.size());
        } else {
            static const std::string authorsPrefix = "authors:";
            index = lowerCased.find(authorsPrefix);
            if (index != std::string::npos) {
                name = name.substr(index + authorsPrefix.size());
            }
        }
        index = name.find(',');
        if (index != std::string::npos) {
            std::string before = name.substr(0, index);
            std::string after = name.substr(index + 1);
            ZLUnicodeUtil::utf8Trim(before);
            ZLUnicodeUtil::utf8Trim(after);
            authorData.SortKey = before;
            authorData.DisplayName = after + ' ' + before;
        } else {
            ZLUnicodeUtil::utf8Trim(name);
            index = name.rfind(' ');
            authorData.SortKey = name.substr(index + 1);
            authorData.DisplayName = name;
        }
        authors.push_back(authorData);
    }*/
    return authors;
}
