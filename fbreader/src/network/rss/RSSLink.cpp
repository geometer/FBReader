#include "RSSLink.h"

#include <ZLNetworkUtil.h>
#include <ZLNetworkManager.h>
#include <iostream> //udmv

RSSLink::RSSLink(const std::string &siteName) : OPDSLink(siteName){
}

RSSLink::~RSSLink() {
}

shared_ptr<NetworkItem> RSSLink::libraryItem() const {
    NetworkItem::UrlInfoCollection urlMap;
    urlMap[NetworkItem::URL_COVER] = getIcon();
    urlMap[NetworkItem::URL_CATALOG] = url(URL_MAIN);
    std::cout << "!RSSLink::libraryItem() url " << urlMap[NetworkItem::URL_CATALOG] << std::endl;
    return new RSSCatalogItem(*this, getTitle(), getSummary(), urlMap);
}

shared_ptr<ZLNetworkRequest> RSSLink::createNetworkRequest(const std::string &url, NetworkOperationData &result) const{
    if (url.empty()) {
        return 0;
    }
    std::cout << "-> RSSLink::createNetworkRequest url: " << url << std::endl;
    return ZLNetworkManager::Instance().createXMLParserRequest(url, new RSSXMLParser(*this, result));
}
