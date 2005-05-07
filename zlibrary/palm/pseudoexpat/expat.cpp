#include "expat.h"

XMLPARSEAPI(enum XML_Status)
XML_Parse(XML_Parser parser, const char *s, int len, int isFinal) {
	return XML_STATUS_OK;
}

XMLPARSEAPI(XML_Parser)
XML_ParserCreate(const XML_Char *encoding) {
	return 0;
}

XMLPARSEAPI(void)
XML_ParserFree(XML_Parser parser) {}

XMLPARSEAPI(enum XML_Error)
XML_UseForeignDTD(XML_Parser parser, XML_Bool useDTD) {
	return XML_ERROR_NONE;
}

XMLPARSEAPI(XML_Parser)
XML_ExternalEntityParserCreate(XML_Parser parser,
                               const XML_Char *context,
                               const XML_Char *encoding) {
	return 0;
}

XMLPARSEAPI(void)
XML_SetUnknownEncodingHandler(XML_Parser parser,
                              XML_UnknownEncodingHandler handler,
                              void *encodingHandlerData) {}

XMLPARSEAPI(void)
XML_SetUserData(XML_Parser parser, void *userData) {}

XMLPARSEAPI(void)
XML_SetStartElementHandler(XML_Parser parser,
                           XML_StartElementHandler handler) {}

XMLPARSEAPI(void)
XML_SetEndElementHandler(XML_Parser parser,
                         XML_EndElementHandler handler) {}

XMLPARSEAPI(void)
XML_SetCharacterDataHandler(XML_Parser parser,
                            XML_CharacterDataHandler handler) {}
