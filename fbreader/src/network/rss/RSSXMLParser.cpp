#include "RSSXMLParser.h"

#include <iostream> //udmv

RSSXMLParser::RSSXMLParser(const NetworkLink &link, NetworkOperationData &result){
}

void RSSXMLParser::startElementHandler(const char *tag, const char **attributes){
    std::cout << "[RSSXMLParser] start TAG = " << tag << std::endl;
}

void RSSXMLParser::endElementHandler(const char *tag){
    std::cout << "[RSSXMLParser] end TAG = " << tag << ", myBuffer = " << myBuffer << std::endl;
    myBuffer.clear();
}

void RSSXMLParser::characterDataHandler(const char *text, std::size_t len){
    myBuffer.append(text, len);
}

bool RSSXMLParser::processNamespaces() const{

}
