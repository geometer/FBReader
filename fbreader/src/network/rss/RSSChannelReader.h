#ifndef RSSCHANNELREADER_H
#define RSSCHANNELREADER_H

#include "RSSMetadata.h"
#include <shared_ptr.h>

class RSSChannelReader {

public:
    RSSChannelReader() {}
    virtual ~RSSChannelReader() {}

public:
    virtual void processChannelItem(shared_ptr<RSSItem> item) = 0;

};
#endif // RSSCHANNELREADER_H
