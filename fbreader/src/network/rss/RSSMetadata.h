#ifndef RSSMETADATA_H
#define RSSMETADATA_H

#include "../atom/ATOMContainers.h"

class RSSItem : public ATOMEntry{
public:
    RSSItem();
    RSSItem(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated);
};

#endif // RSSMETADATA_H
