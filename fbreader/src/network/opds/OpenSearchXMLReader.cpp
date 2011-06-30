/*
 * qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
 */

#include "OpenSearchXMLReader.h"
#include <ZLMimeType.h>

static const std::string TAG_URL = "Url";

std::string OpenSearchXMLReader::convertOpenSearchURL(const std::string& raws) { //TODO
	size_t pos = raws.find('{');
	return raws.substr(0, pos) + "%s";
}

void OpenSearchXMLReader::startElementHandler(const char *tag, const char **attributes) {
	if (TAG_URL == tag) {
		const char *type = attributeValue(attributes, "type");
		if (std::string(type) == ZLMimeType::APPLICATION_ATOM_XML) {
			const char *templ = attributeValue(attributes, "template");
			if (templ != 0) {
				myTemplateURL = convertOpenSearchURL(templ);
			}
		}
	}
}

void OpenSearchXMLReader::endElementHandler(const char *tag) {
}

void OpenSearchXMLReader::characterDataHandler(const char *text, size_t len) {
}
