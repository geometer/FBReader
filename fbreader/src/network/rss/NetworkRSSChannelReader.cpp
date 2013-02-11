#include "NetworkRSSChannelReader.h"

#include <iostream> //udmv

NetworkRSSChannelReader::NetworkRSSChannelReader(const RSSLink &link, const std::string &baseURL, NetworkOperationData &result) :
    myLink(link), myURL(baseURL), myData(result), myIndex(0){
}

void NetworkRSSChannelReader::processChannelItem(shared_ptr<RSSItem> item){
    if(item.isNull())
        return;

    RSSItem &myItem = *item;

    shared_ptr<NetworkItem> book;
    book = new RSSBookItem(myLink, myItem, myIndex++);
    myData.Items.push_back(book);
}
