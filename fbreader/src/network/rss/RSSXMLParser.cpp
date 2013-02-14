#include "RSSXMLParser.h"

#include <iostream> //udmv

static const std::string TAG_RSS = "rss";
static const std::string TAG_CHANNEL = "channel";
static const std::string TAG_ITEM = "item";
static const std::string TAG_TITLE = "title";
static const std::string TAG_LINK = "link";
static const std::string TAG_GUID = "guid";
static const std::string TAG_DESCRIPTION = "description";
static const std::string TAG_PUBDATE = "pubDate";

RSSXMLParser::RSSXMLParser(shared_ptr<RSSChannelReader> channelReader) : myState(START), myChannelReader(channelReader) {
}

void RSSXMLParser::startElementHandler(const char *tag, const char **attributes){
    switch (myState) {
        case START:
            if (testTag(ZLXMLNamespace::Atom, TAG_RSS, tag)) {
                myState = RSS;
            }
            break;
        case RSS:
            if (testTag(ZLXMLNamespace::Atom, TAG_CHANNEL, tag)) {
                myState = CHANNEL;
            }
            break;
        case CHANNEL:
            if (testTag(ZLXMLNamespace::Atom, TAG_TITLE, tag)) {
                myState = C_TITLE;
            }
            if (testTag(ZLXMLNamespace::Atom, TAG_LINK, tag)) {                
                myState = C_LINK;
            }
            if (testTag(ZLXMLNamespace::Atom, TAG_ITEM, tag)) {
                myRSSItem = new RSSItem();
                myState = ITEM;
            }
            break;
        case ITEM:
            if (testTag(ZLXMLNamespace::Atom, TAG_TITLE, tag)) {
                myState = TITLE;
            }
            if (testTag(ZLXMLNamespace::Atom, TAG_LINK, tag)) {
                myState = LINK;
            }
            if (testTag(ZLXMLNamespace::Atom, TAG_DESCRIPTION, tag)) {
                myState = DESCRIPTION;
            }
            if (testTag(ZLXMLNamespace::Atom, TAG_GUID, tag)) {                
                myState = GUID;
            }
            if (testTag(ZLXMLNamespace::Atom, TAG_PUBDATE, tag)) {
                myState = PUBDATE;
            }
    }
}

void RSSXMLParser::endElementHandler(const char *tag){
    ZLUnicodeUtil::utf8Trim(myBuffer);    
    switch (myState) {
        case START:
            break;
        case RSS:
            if (testTag(ZLXMLNamespace::Atom, TAG_RSS, tag)) {
                myState = START;
            }
            break;
        case CHANNEL:
            if (testTag(ZLXMLNamespace::Atom, TAG_CHANNEL, tag)) {
                myState = RSS;
            }
            break;
        case C_TITLE:
            if (testTag(ZLXMLNamespace::Atom, TAG_TITLE, tag)) {
                myState = CHANNEL;
            }
            break;
        case C_LINK:
            if (testTag(ZLXMLNamespace::Atom, TAG_LINK, tag)) {                
                myState = CHANNEL;
            }
            break;
        case ITEM:
            if (testTag(ZLXMLNamespace::Atom, TAG_ITEM, tag)) {
                myChannelReader->processChannelItem(myRSSItem);
                myRSSItem.reset();
                myState = CHANNEL;
            }
        case TITLE:
            if (testTag(ZLXMLNamespace::Atom, TAG_TITLE, tag)) {
                const std::string mark = "~ by:";
                unsigned found = myBuffer.find(mark);
                if (found != std::string::npos){
                    std::string title = myBuffer.substr(0, found);
                    myRSSItem->setTitle(title);

                    std::string authorName = myBuffer.substr(found+mark.length());
                    myAuthor = new ATOMAuthor(authorName);
                    myRSSItem->authors().push_back(myAuthor);
                    myAuthor.reset();
                }else{
                    myRSSItem->setTitle(myBuffer);
                }
                myState = ITEM;
            }
            break;
        case GUID:
            if (testTag(ZLXMLNamespace::Atom, TAG_GUID, tag)) {
                myId = new ATOMId(myBuffer);
                myRSSItem->setId(myId);
                myState = ITEM;
            }
            break;
        case DESCRIPTION:
            if (testTag(ZLXMLNamespace::Atom, TAG_DESCRIPTION, tag)) {
                myRSSItem->setSummary(myBuffer);
                myState = ITEM;
            }
            break;
        case PUBDATE:
            if (testTag(ZLXMLNamespace::Atom, TAG_PUBDATE, tag)) {
                myRSSItem->setUserData("pubdate", myBuffer);
                myState = ITEM;
            }
            break;
        case LINK:
            if (testTag(ZLXMLNamespace::Atom, TAG_LINK, tag)) {
                myLink = new ATOMLink(myBuffer, OPDSConstants::REL_ACQUISITION_DOWNLOAD_IN_BROWSER);
                myRSSItem->links().push_back(myLink);
                myLink.reset();
                myState = ITEM;
            }
            break;
    }
    myBuffer.clear();
}

void RSSXMLParser::characterDataHandler(const char *text, std::size_t len){
    myBuffer.append(text, len);
}

bool RSSXMLParser::processNamespaces() const{
    return true;
}
