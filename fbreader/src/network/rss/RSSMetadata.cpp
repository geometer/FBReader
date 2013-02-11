#include "RSSMetadata.h"

RSSItem::RSSItem() : ATOMEntry(){
}

RSSItem::RSSItem(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated) :
    ATOMEntry(id, title, updated) {
}

