#ifndef NETWORKRSSFEEDREADER_H
#define NETWORKRSSFEEDREADER_H

#include "RSSChannelReader.h"
#include "RSSLink.h"
#include "RSSBookItem.h"

class NetworkRSSChannelReader : public RSSChannelReader
{
public:
    NetworkRSSChannelReader(
            const RSSLink &link,
            const std::string &baseURL,
            NetworkOperationData &result);

public:
    void processChannelItem(shared_ptr<RSSItem> item);

private:
    NetworkOperationData &myData;
    const RSSLink &myLink;
    const std::string myURL;
    unsigned int myIndex;
};

#endif // NETWORKRSSFEEDREADER_H
