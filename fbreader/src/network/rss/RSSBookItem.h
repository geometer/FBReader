#ifndef RSSBOOKITEM_H
#define RSSBOOKITEM_H

#include "../NetworkItems.h"
#include "RSSMetadata.h"
#include "RSSLink.h"

class RSSBookItem : public NetworkBookItem
{
public:
    RSSBookItem(const RSSLink &link, RSSItem &item, unsigned int index);

protected:
    NetworkItem::UrlInfoCollection getUrls(RSSItem &item);
    std::vector<shared_ptr<BookReference> >  getReferences(RSSItem &item);
    std::vector<NetworkBookItem::AuthorData> getAuthors(RSSItem &item);
};

#endif // RSSBOOKITEM_H
