#ifndef __EXPAT_H__
#define __EXPAT_H__

// pseudo expat

typedef char XML_Char;

#define XMLCALL
#define XMLIMPORT

#define XMLPARSEAPI(type) XMLIMPORT type XMLCALL

typedef struct {
  int map[256];
  void *data;
  int (XMLCALL *convert)(void *data, const char *s);
  void (XMLCALL *release)(void *data);
} XML_Encoding;

enum XML_Status {
  XML_STATUS_ERROR = 0,
#define XML_STATUS_ERROR XML_STATUS_ERROR
  XML_STATUS_OK = 1,
#define XML_STATUS_OK XML_STATUS_OK
  XML_STATUS_SUSPENDED = 2,
#define XML_STATUS_SUSPENDED XML_STATUS_SUSPENDED
};

enum XML_Error {
  XML_ERROR_NONE,
  XML_ERROR_NO_MEMORY,
  XML_ERROR_SYNTAX,
  XML_ERROR_NO_ELEMENTS,
  XML_ERROR_INVALID_TOKEN,
  XML_ERROR_UNCLOSED_TOKEN,
  XML_ERROR_PARTIAL_CHAR,
  XML_ERROR_TAG_MISMATCH,
  XML_ERROR_DUPLICATE_ATTRIBUTE,
  XML_ERROR_JUNK_AFTER_DOC_ELEMENT,
  XML_ERROR_PARAM_ENTITY_REF,
  XML_ERROR_UNDEFINED_ENTITY,
  XML_ERROR_RECURSIVE_ENTITY_REF,
  XML_ERROR_ASYNC_ENTITY,
  XML_ERROR_BAD_CHAR_REF,
  XML_ERROR_BINARY_ENTITY_REF,
  XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF,
  XML_ERROR_MISPLACED_XML_PI,
  XML_ERROR_UNKNOWN_ENCODING,
  XML_ERROR_INCORRECT_ENCODING,
  XML_ERROR_UNCLOSED_CDATA_SECTION,
  XML_ERROR_EXTERNAL_ENTITY_HANDLING,
  XML_ERROR_NOT_STANDALONE,
  XML_ERROR_UNEXPECTED_STATE,
  XML_ERROR_ENTITY_DECLARED_IN_PE,
  XML_ERROR_FEATURE_REQUIRES_XML_DTD,
  XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING,
  /* Added in 1.95.7. */
  XML_ERROR_UNBOUND_PREFIX,
  /* Added in 1.95.8. */
  XML_ERROR_UNDECLARING_PREFIX,
  XML_ERROR_INCOMPLETE_PE,
  XML_ERROR_XML_DECL,
  XML_ERROR_TEXT_DECL,
  XML_ERROR_PUBLICID,
  XML_ERROR_SUSPENDED,
  XML_ERROR_NOT_SUSPENDED,
  XML_ERROR_ABORTED,
  XML_ERROR_FINISHED,
  XML_ERROR_SUSPEND_PE
};

struct XML_ParserStruct;
typedef struct XML_ParserStruct *XML_Parser;

typedef unsigned char XML_Bool;
#define XML_TRUE   ((XML_Bool) 1)
#define XML_FALSE  ((XML_Bool) 0)

XMLPARSEAPI(enum XML_Status)
XML_Parse(XML_Parser parser, const char *s, int len, int isFinal) XML_SECTION;

XMLPARSEAPI(XML_Parser)
XML_ParserCreate(const XML_Char *encoding) XML_SECTION;

XMLPARSEAPI(void)
XML_ParserFree(XML_Parser parser) XML_SECTION;

XMLPARSEAPI(enum XML_Error)
XML_UseForeignDTD(XML_Parser parser, XML_Bool useDTD) XML_SECTION;

XMLPARSEAPI(XML_Parser)
XML_ExternalEntityParserCreate(XML_Parser parser,
                               const XML_Char *context,
                               const XML_Char *encoding) XML_SECTION;

typedef int (XMLCALL *XML_UnknownEncodingHandler) (
                                    void *encodingHandlerData,
                                    const XML_Char *name,
                                    XML_Encoding *info);

typedef void (XMLCALL *XML_StartElementHandler) (void *userData,
                                                 const XML_Char *name,
                                                 const XML_Char **atts);

typedef void (XMLCALL *XML_EndElementHandler) (void *userData,
                                               const XML_Char *name);

typedef void (XMLCALL *XML_CharacterDataHandler) (void *userData,
                                                  const XML_Char *s,
                                                  int len);

XMLPARSEAPI(void)
XML_SetUnknownEncodingHandler(XML_Parser parser,
                              XML_UnknownEncodingHandler handler,
                              void *encodingHandlerData) XML_SECTION;

XMLPARSEAPI(void)
XML_SetUserData(XML_Parser parser, void *userData) XML_SECTION;

XMLPARSEAPI(void)
XML_SetStartElementHandler(XML_Parser parser,
                           XML_StartElementHandler handler) XML_SECTION;

XMLPARSEAPI(void)
XML_SetEndElementHandler(XML_Parser parser,
                         XML_EndElementHandler handler) XML_SECTION;

XMLPARSEAPI(void)
XML_SetCharacterDataHandler(XML_Parser parser,
                            XML_CharacterDataHandler handler) XML_SECTION;

#endif /* __EXPAT_H__ */
