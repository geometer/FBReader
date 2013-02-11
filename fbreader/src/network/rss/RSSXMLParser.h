#ifndef RSSXMLPARSER_H
#define RSSXMLPARSER_H

#include <ZLXMLReader.h>
#include "../NetworkItems.h"
#include "../NetworkOperationData.h"

class RSSXMLParser : public ZLXMLReader{
public:
    RSSXMLParser(const NetworkLink &link, NetworkOperationData &result);

protected:
    void startElementHandler(const char *tag, const char **attributes);
    void endElementHandler(const char *tag);
    void characterDataHandler(const char *text, std::size_t len);
    bool processNamespaces() const;

protected:
    enum State {
        START
    };

protected:
    State myState;

private:
    std::string myBuffer;

};

#endif // RSSXMLPARSER_H
