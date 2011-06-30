/*
 * qqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
 */

#ifndef __OpenSearchXMLReader_H__
#define __OpenSearchXMLReader_H__

#include <string>
#include <ZLXMLReader.h>

class OpenSearchXMLReader : public ZLXMLReader {

public:
	OpenSearchXMLReader() {}
	std::string templateURL() {return myTemplateURL;}

	static std::string convertOpenSearchURL(const std::string& raws);

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, size_t len);
	std::string myTemplateURL;

};















#endif /* __OpenSearchXMLReader_H__ */
