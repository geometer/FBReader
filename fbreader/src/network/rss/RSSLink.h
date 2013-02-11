#ifndef RSSLINK_H
#define RSSLINK_H

#include "../opds/OPDSLink.h"
#include "RSSCatalogItem.h"
#include "RSSXMLParser.h"
#include "NetworkRSSChannelReader.h"

class RSSLink : public OPDSLink
{
public:
    RSSLink(const std::string &siteName);

public:
    ~RSSLink();

private:
    shared_ptr<NetworkItem> libraryItem() const;
    shared_ptr<ZLNetworkRequest> createNetworkRequest(const std::string &url, NetworkOperationData &result) const;

friend class RSSCatalogItem;
};

#endif // RSSLINK_H
