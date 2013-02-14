#ifndef RSSXMLPARSER_H
#define RSSXMLPARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ZLStringUtil.h>
#include <ZLXMLReader.h>
#include <ZLUnicodeUtil.h>
#include <ZLXMLNamespace.h>

#include "RSSChannelReader.h"
#include "RSSMetadata.h"

#include "../NetworkItems.h"
#include "../NetworkOperationData.h"

class RSSXMLParser : public ZLXMLReader{
public:
    RSSXMLParser(shared_ptr<RSSChannelReader> channelReader);

protected:
    void startElementHandler(const char *tag, const char **attributes);
    void endElementHandler(const char *tag);
    void characterDataHandler(const char *text, std::size_t len);
    bool processNamespaces() const;

protected:
    enum State {
        START,
        RSS, CHANNEL, C_TITLE, C_LINK,
        ITEM, TITLE, LINK, COMMENTS, PUBDATE, CATEGORY, GUID, DESCRIPTION, CONTENT, COMMENTS_RSS
    };

protected:
    State myState;

private:
    std::string myBuffer;
    shared_ptr<RSSChannelReader> myChannelReader;
    shared_ptr<RSSItem> myRSSItem;

    shared_ptr<ATOMId> myId;
    shared_ptr<ATOMUpdated> myUpdated;
    shared_ptr<ATOMLink> myLink;
    shared_ptr<ATOMAuthor> myAuthor;
};

#endif // RSSXMLPARSER_H
