#ifndef RSSBOOKITEM_H
#define RSSBOOKITEM_H

#include "../NetworkItems.h"
#include "RSSMetadata.h"

class RSSBookItem : public NetworkBookItem
{
public:
    RSSBookItem(const NetworkLink &link, RSSItem &item, unsigned int index);
    NetworkItem::UrlInfoCollection getUrls(RSSItem &entry);
};

#endif // RSSBOOKITEM_H
