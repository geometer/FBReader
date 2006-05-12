/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <iostream>
#include <cctype>

#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLXMLReader.h>

#include "../../Files.h"
#include "RtfReader.h"

static const int rtfStreamBufferSize = 4096;

enum ErrorCode {
  ecOK,                    // Everything's fine!
  ecStackUnderflow,        // Unmatched '}'
  ecUnmatchedBrace,        // RTF ended during an open group.
};

RtfReader::RtfReader(const std::string &encoding) {
  this->encoding = encoding;
  
  if (!encoding.empty()) {
    myConverter = ZLEncodingConverter::createConverter(encoding);
  }
}

RtfReader::~RtfReader() {
}

// What types of properties are there?
typedef enum {ipropBold, ipropItalic, ipropUnderline, ipropLeftInd,
        ipropRightInd, ipropFirstInd, ipropCols, ipropPgnX,
        ipropPgnY, ipropXaPage, ipropYaPage, ipropXaLeft,
        ipropXaRight, ipropYaTop, ipropYaBottom, ipropPgnStart,
        ipropSbk, ipropPgnFormat, ipropFacingp, ipropLandscape,
        ipropJust, ipropPard, ipropPlain, ipropSectd,
        ipropMax } IPROP;
typedef enum {ppropPng, ppropJpeg } PPROP;

typedef enum {actnSpec, actnByte, actnWord} ACTN;
typedef enum {propChp, propPap, propSep, propDop} PROPTYPE;

typedef enum {ipfnParagraph, ipfnHex, ipfnBin, ipfnCodePage, ipfnSkipDest,
    ipfnParagraphReset } IPFN;

typedef struct propmod
{
  ACTN actn;        // size of value
  PROPTYPE prop;      // structure containing value
  int  offset;      // offset of value from base of structure
} PROP;

// RTF parser tables

// Property descriptions
PROP rgprop [ipropMax] = {
  { actnByte,   propChp,  offsetof(CHP, fBold) },     // ipropBold
  { actnByte,   propChp,  offsetof(CHP, fItalic) },   // ipropItalic
  { actnByte,   propChp,  offsetof(CHP, fUnderline) },  // ipropUnderline
  { actnWord,   propPap,  offsetof(PAP, xaLeft) },    // ipropLeftInd
  { actnWord,   propPap,  offsetof(PAP, xaRight) },   // ipropRightInd
  { actnWord,   propPap,  offsetof(PAP, xaFirst) },   // ipropFirstInd
  { actnWord,   propSep,  offsetof(SEP, cCols) },     // ipropCols
  { actnWord,   propSep,  offsetof(SEP, xaPgn) },     // ipropPgnX
  { actnWord,   propSep,  offsetof(SEP, yaPgn) },     // ipropPgnY
  { actnWord,   propDop,  offsetof(DOP, xaPage) },    // ipropXaPage
  { actnWord,   propDop,  offsetof(DOP, yaPage) },    // ipropYaPage
  { actnWord,   propDop,  offsetof(DOP, xaLeft) },    // ipropXaLeft
  { actnWord,   propDop,  offsetof(DOP, xaRight) },   // ipropXaRight
  { actnWord,   propDop,  offsetof(DOP, yaTop) },     // ipropYaTop
  { actnWord,   propDop,  offsetof(DOP, yaBottom) },  // ipropYaBottom
  { actnWord,   propDop,  offsetof(DOP, pgnStart) },  // ipropPgnStart
  { actnWord,   propSep,  offsetof(SEP, sbk) },     // ipropSbk
  { actnWord,   propSep,  offsetof(SEP, pgnFormat) },   // ipropPgnFormat
  { actnByte,   propDop,  offsetof(DOP, fFacingp) },  // ipropFacingp
  { actnByte,   propDop,  offsetof(DOP, fLandscape) },  // ipropLandscape
  { actnWord,   propPap,  offsetof(PAP, just) },    // ipropJust
  { actnSpec,   propPap,  0 },              // ipropPard
  { actnSpec,   propChp,  0 },              // ipropPlain
  { actnSpec,   propSep,  0 },              // ipropSectd
};

struct RtfKeywordInfo {

protected:
  virtual ~RtfKeywordInfo() {}

public:
  virtual RtfReader::ParserState run(RtfReader &reader, int *parameter) const = 0;
};

class RtfKeywordPropInfo : public RtfKeywordInfo {

public:
  RtfKeywordPropInfo(int defaultParameter, bool alwaysUseDefaultParameter, int index) : myDefaultParameter(defaultParameter), myAlwaysUseDefaultParameter(alwaysUseDefaultParameter), myIndex(index) {}
  RtfReader::ParserState run(RtfReader &reader, int *parameter) const {
    reader.ecApplyPropChange(myIndex,
        (parameter && !myAlwaysUseDefaultParameter) ? *parameter : myDefaultParameter);
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
  int myDefaultParameter;
  bool myAlwaysUseDefaultParameter;
  int myIndex;
};

class RtfKeywordCharInfo : public RtfKeywordInfo {

public:
  RtfKeywordCharInfo(const std::string &chr) : myChar(chr) {}
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    reader.ecParseCharData(myChar.data(), myChar.length(), false);
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
  std::string myChar;
};

class RtfKeywordDestinationInfo : public RtfKeywordInfo {

public:
  RtfKeywordDestinationInfo(Destination dest) : myDest(dest) {}
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    reader.ecChangeDest(myDest);
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
  Destination myDest;
};

class RtfKeywordStyleInfo : public RtfKeywordInfo {

public:
  RtfKeywordStyleInfo() {}
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    reader.ecStyleChange();
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
};

class RtfKeywordSpecInfo : public RtfKeywordInfo {

public:
  RtfKeywordSpecInfo(IPFN ipfn) : myIpfn(ipfn) {}
  RtfReader::ParserState run(RtfReader &reader, int *parameter) const {
    return reader.ecParseSpecialKeyword(myIpfn, parameter ? *parameter : 0);
  }
  
private:
  IPFN myIpfn;
};

class RtfKeywordPictureInfo : public RtfKeywordInfo {

public:
  RtfKeywordPictureInfo(const std::string &mimeType) : myMimeType(mimeType) {}
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    reader.ecApplyPictPropChange(myMimeType);
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
  const std::string myMimeType;
};

static std::map<std::string, RtfKeywordInfo*> myKeywordMap;

struct pkw {
  char *kw;
  int  dflt;
  bool fPassDflt;
  int  idx;
} propKeyWords[] = {
  { "b",             1,    false,   ipropBold },
  { "cols",          1,    false,   ipropCols },
  { "facingp",       1,    true,    ipropFacingp },
  { "fi",            0,    false,   ipropFirstInd },
  { "i",             1,    false,   ipropItalic },
  { "landscape",     1,    true,    ipropLandscape },
  { "li",            0,    false,   ipropLeftInd },
  { "margb",      1440,    false,   ipropYaBottom },
  { "margl",      1800,    false,   ipropXaLeft },
  { "margr",      1800,    false,   ipropXaRight },
  { "margt",      1440,    false,   ipropYaTop },
  { "paperh",    15480,    false,   ipropYaPage },
  { "paperw",    12240,    false,   ipropXaPage },
  { "pgndec",    pgDec,    true,    ipropPgnFormat },
  { "pgnlcltr", pgLLtr,    true,    ipropPgnFormat },
  { "pgnlcrm",  pgLRom,    true,    ipropPgnFormat },
  { "pgnstart",      1,    true,    ipropPgnStart },
  { "pgnucltr", pgULtr,    true,    ipropPgnFormat },
  { "pgnucrm",  pgURom,    true,    ipropPgnFormat },
  { "pgnx",          0,    false,   ipropPgnX },
  { "pgny",          0,    false,   ipropPgnY },
  { "qc",        justC,    true,    ipropJust },
  { "ql",        justL,    true,    ipropJust },
  { "qr",        justR,    true,    ipropJust },
  { "qj",        justF,    true,    ipropJust },
  { "sbkcol",   sbkCol,    true,    ipropSbk },
  { "sbkeven",  sbkEvn,    true,    ipropSbk },
  { "sbknone",  sbkNon,    true,    ipropSbk },
  { "sbkodd",   sbkOdd,    true,    ipropSbk },
  { "sbkpage",   sbkPg,    true,    ipropSbk },
  { "ri",            0,    false,   ipropRightInd },
  { "u",             1,    false,   ipropUnderline },
};

struct skw {
  const char *kw;
  IPFN ipfn;
} specKeyWords[] = {
  { "\x0a",     ipfnParagraph },
  { "\x0d",     ipfnParagraph },
  { "'",        ipfnHex },
  { "*",        ipfnSkipDest },
  { "ansicpg",  ipfnCodePage },
  { "bin",      ipfnBin },
  { "par",      ipfnParagraph },
//  {   "pard",   ipfnParagraphReset },
};

static const std::string charTag = "char";
static const std::string destinationTag = "destination";
  
class RtfKeywordsReader : public ZLXMLReader {

public:
  RtfKeywordsReader(std::map<std::string, RtfKeywordInfo*> &keywordMap) : myKeywordMap(keywordMap) {
  }

  void startElementHandler(const char *tag, const char **attributes) {
    const char *keyword = attributeValue(attributes, "keyword");
    const char *value = attributeValue(attributes, "value");
    if (keyword != 0) {
      if (charTag == tag) {
        if (value != 0) {
          myKeywordMap[keyword] = new RtfKeywordCharInfo(value);
        }
      } else if (destinationTag == tag) {
        if (value != 0) {
          myKeywordMap[keyword] = new RtfKeywordDestinationInfo((Destination)atoi(value));
        }
      }
    }
  }

private:
  std::map<std::string, RtfKeywordInfo*> &myKeywordMap;
};

static void fillKeywordMap() {
  if (myKeywordMap.empty()) {
    RtfKeywordsReader(myKeywordMap).readDocument(
      ZLFile(
        Files::PathPrefix + "formats" + Files::PathDelimiter + "rtf" + Files::PathDelimiter + "keywords.xml"
      ).inputStream()
    );

    for (unsigned int i = 0; i < sizeof(propKeyWords) / sizeof(struct pkw); i++) {
      const struct pkw &s = propKeyWords[i];
      myKeywordMap[s.kw] = new RtfKeywordPropInfo(s.dflt, s.fPassDflt, s.idx);
    }
    for (unsigned int i = 0; i < sizeof(specKeyWords) / sizeof(struct skw); i++) {
      myKeywordMap[specKeyWords[i].kw] = new RtfKeywordSpecInfo(specKeyWords[i].ipfn);
    }
    myKeywordMap["jpegblip"] = new RtfKeywordPictureInfo("image/jpeg");
    myKeywordMap["pngblip"] = new RtfKeywordPictureInfo("image/png");
    myKeywordMap["s"] = new RtfKeywordStyleInfo();
  }
}

//
// %%Function: ecApplyPropChange
//
// Set the property identified by _iprop_ to the value _val_.
//
//

void RtfReader::ecApplyPropChange(int iprop, int val) {
  char *pb = NULL;
  bool oldItalic = state.chp.fItalic;
  bool oldBold = state.chp.fBold;
  
//  DPRINT("Aply prop change: %i %i\n", iprop, val);

  if (state.rds == DESTINATION_SKIP)         // If we're skipping text,
    return;          // don't do anything.

  switch (rgprop[iprop].prop) {
    case propDop:
      pb = (char *) &state.dop;
      break;
    case propSep:
      pb = (char *) &state.sep;
      break;
    case propPap:
      pb = (char *) &state.pap;
      break;
    case propChp:
      pb = (char *) &state.chp;
      break;
    default:
      if (rgprop[iprop].actn != actnSpec) {
        std::cerr << "parse failed: bad table 0\n";
        return;
      }
  }
  
  switch (rgprop[iprop].actn) {
    case actnByte:
      (*(bool *) (pb + rgprop[iprop].offset)) = (val != 0);
      break;
    case actnWord:
      (*(int *) (pb+rgprop[iprop].offset)) = val;
      break;
    case actnSpec:
      ecParseSpecialProperty(iprop);
      return;
    default:
      std::cerr << "parse failed: bad table 1\n";
      return;
  }
  
  if (state.chp.fItalic != oldItalic) {
    if (state.chp.fItalic) {
      startElementHandler(_ITALIC);
    } else {
      endElementHandler(_ITALIC);
    }
  }

  if (state.chp.fBold != oldBold) {
    if (state.chp.fBold) {
      startElementHandler(_BOLD);
    } else {
      endElementHandler(_BOLD);
    }
  }
}

//char style_attributes[1][256];
void RtfReader::ecStyleChange() {
  if (state.rds == DESTINATION_STYLESHEET) {
    //std::cerr << "Add style index: " << val << "\n";
    
    //sprintf(style_attributes[0], "%i", val);
    startElementHandler(_STYLE_INFO);
  } else /*if (state.rds == rdsContent)*/ {
    //std::cerr << "Set style index: " << val << "\n";

    //sprintf(style_attributes[0], "%i", val);
    startElementHandler(_STYLE_SET);
  }
}

//
// %%Function: ecParseSpecialProperty
//
// Set a property that requires code to evaluate.
//

void RtfReader::ecParseSpecialProperty(int iprop) {
  switch (iprop) {
    case ipropPard:
      memset(&state.pap, 0, sizeof(state.pap));
      break;
    case ipropPlain:
      if (state.chp.fItalic) {
        endElementHandler(_ITALIC);
      }

      if (state.chp.fBold) {
        endElementHandler(_BOLD);
      }
    
      memset(&state.chp, 0, sizeof(state.chp));
      break;
    case ipropSectd:
      memset(&state.sep, 0, sizeof(state.sep));
      break;
    default:
      std::cerr << "parse failed: bad table 2\n";
      break;
  }
}

void RtfReader::ecApplyPictPropChange(const std::string &mimeType) {
  startElementHandler(_IMAGE_TYPE);
  myNextImageMimeType = mimeType;
} 

void RtfReader::ecChangeDest(Destination destination) {
  if (state.rds == DESTINATION_SKIP) {
    return;
  }

  state.rds = destination;
  switch (destination) {
    case DESTINATION_INFO:
      DPRINT("title info\n");
      startElementHandler(_TITLE_INFO);
      break;
    case DESTINATION_TITLE:
      DPRINT("title\n");
      startElementHandler(_BOOK_TITLE);
      break;
    case DESTINATION_AUTHOR:
      DPRINT("author\n");
      startElementHandler(_AUTHOR);
      break;
    case DESTINATION_PICTURE:
      DPRINT("picture\n");
      state.ReadDataAsHex = true;
      startElementHandler(_IMAGE);
      break;
    case DESTINATION_FOOTNOTE:
      DPRINT("footnote\n");
      startElementHandler(_FOOTNOTE);
      break;
    case DESTINATION_STYLESHEET:
      DPRINT("style sheet\n");
      startElementHandler(_STYLE_SHEET);
      break;
    case DESTINATION_NONE:
    case DESTINATION_SKIP:
      break;
  }
}

//
// %%Function: ecEndGroupAction
//
// The destination specified by rds is coming to a close.
// If there's any cleanup that needs to be done, do it now.
//

void RtfReader::ecEndGroupAction(Destination destination) {
  switch (destination) {
    case DESTINATION_INFO:
      DPRINT("info end\n");
      endElementHandler(_TITLE_INFO);
      break;
    case DESTINATION_TITLE:
      DPRINT("title end\n");
      endElementHandler(_BOOK_TITLE);
      break;
    case DESTINATION_AUTHOR:
      DPRINT("author end\n");
      endElementHandler(_AUTHOR);
      break;
    case DESTINATION_PICTURE:
      DPRINT("image end\n");
      endElementHandler(_IMAGE);
      break;
    case DESTINATION_FOOTNOTE:
      DPRINT("footnote end\n");
      endElementHandler(_FOOTNOTE);
      break;
    case DESTINATION_STYLESHEET:
      DPRINT("style sheet end\n");
      endElementHandler(_STYLE_INFO);
      break;
    case DESTINATION_SKIP:
    case DESTINATION_NONE:
      break;
  }
}

//
// %%Function: ecParseSpecialKeyword
//
// Evaluate an RTF control that needs special processing.
//
static const char *encoding1251 = "windows-1251";

RtfReader::ParserState RtfReader::ecParseSpecialKeyword(int ipfn, int param) {
  ParserState parserState = READ_NORMAL_DATA;
  if (state.rds == DESTINATION_SKIP && ipfn != ipfnBin)  // if we're skipping, and it's not
    return parserState;            // the \bin keyword, ignore it.
  switch (ipfn) {
    case ipfnParagraph:
      startElementHandler(_P);
      break;
    case ipfnParagraphReset:
      startElementHandler(_P_RESET);
      break;
    case ipfnBin:
      if (param > 0) {
        parserState = READ_BINARY_DATA;
        myBinaryDataSize = param;
      }
      break;
    case ipfnHex:
      parserState = READ_HEX_SYMBOL;
      break;
    case ipfnCodePage:
      startElementHandler(_ENCODING);
      if ((param == 1251) && (encoding != encoding1251)) {
        encoding = encoding1251;
        myConverter = ZLEncodingConverter::createConverter(encoding);
      } else {
        // ???
      }
      break;
    case ipfnSkipDest:
      fSkipDestIfUnk = true;
      break;
    default:
      std::cerr << "parse failed: bad table 4\n";
  }
  return parserState;
}

int RtfReader::ecRtfParse() {
  ParserState parserState = READ_NORMAL_DATA;

  std::string keyword;
  std::string parameterString;
  std::string hexString;
  int imageStartOffset = -1;

  while (!is_interrupted) {
    const char *ptr = myStreamBuffer;
    const char *end = myStreamBuffer + myStream->read(myStreamBuffer, rtfStreamBufferSize);
    if (ptr == end) {
      break;
    }
    const char *dataStart = ptr;
    bool readNextChar = true;
    while (ptr != end) {
      switch (parserState) {
        case READ_BINARY_DATA:
          // TODO: optimize
          ecParseCharData(ptr, 1);
          myBinaryDataSize--;
          if (myBinaryDataSize == 0) {
            parserState = READ_NORMAL_DATA;
          }
          break;
        case READ_NORMAL_DATA:
          switch (*ptr) {
            case '{':
              if (ptr > dataStart) {
                ecParseCharData(dataStart, ptr - dataStart);
              }
              dataStart = ptr + 1;
              myStateStack.push(state);
              state.ReadDataAsHex = false;
              break;
            case '}':
            {
              if (ptr > dataStart) {
                ecParseCharData(dataStart, ptr - dataStart);
              }
              dataStart = ptr + 1;

              if (imageStartOffset >= 0) {
                int imageSize = myStream->offset() + (ptr - end) - imageStartOffset;
                insertImage(myNextImageMimeType, myFileName, imageStartOffset, imageSize);
                imageStartOffset = -1;
              }

              if (myStateStack.empty()) {
                return ecStackUnderflow;
              }
              
              if (state.rds != myStateStack.top().rds) {
                ecEndGroupAction(state.rds);
              }
              
              bool oldItalic = state.chp.fItalic;
              bool oldBold = state.chp.fBold;
              state = myStateStack.top();
              myStateStack.pop();
          
              if (state.chp.fItalic != oldItalic) {
                if (state.chp.fItalic) {
                  startElementHandler(_ITALIC);
                } else {
                  endElementHandler(_ITALIC);
                }
              }
          
              if (state.chp.fBold != oldBold) {
                if (state.chp.fBold) {
                  startElementHandler(_BOLD);
                } else {
                  endElementHandler(_BOLD);
                }
              }
              
              break;
            }
            case '\\':
              if (ptr > dataStart) {
                ecParseCharData(dataStart, ptr - dataStart);
              }
              dataStart = ptr + 1;
              keyword.erase();
              parserState = READ_KEYWORD;
              break;
            case 0x0d:
            case 0x0a:      // cr and lf are noise characters...
              if (ptr > dataStart) {
                ecParseCharData(dataStart, ptr - dataStart);
              }
              dataStart = ptr + 1;
              break;
            default:
              if (state.ReadDataAsHex) {
                if (imageStartOffset == -1) {
                  imageStartOffset = myStream->offset() + (ptr - end);
                }
              }
              break;
          }
          break;
        case READ_HEX_SYMBOL:
          hexString += *ptr;
          if (hexString.size() == 2) {
            char ch = strtol(hexString.c_str(), 0, 16); 
            hexString.erase();
            ecParseCharData(&ch, 1);
            parserState = READ_NORMAL_DATA;
          }
          break;
        case READ_KEYWORD:
          if (!isalpha(*ptr)) {
            if (ptr == dataStart) {
              keyword = *ptr;
              parserState = ecTranslateKeyword(keyword, 0, false);
              dataStart = ptr + 1;
            } else {
              keyword.append(dataStart, ptr - dataStart);
              if ((*ptr == '-') || isdigit(*ptr)) {
                dataStart = ptr;
                parserState = READ_KEYWORD_PARAMETER;
              } else {
                readNextChar = *ptr == ' ';
                parserState = ecTranslateKeyword(keyword, 0, false);
                dataStart = readNextChar ? ptr + 1 : ptr;
              }
            }
          }
          break;
        case READ_KEYWORD_PARAMETER:
          if (!isdigit(*ptr)) {
            parameterString.append(dataStart, ptr - dataStart);
            int param = atoi(parameterString.c_str());
            parameterString.erase();
            readNextChar = *ptr == ' ';
            parserState = ecTranslateKeyword(keyword, param, true);
            dataStart = readNextChar ? ptr + 1 : ptr;
          }
          break;
      }
      if (readNextChar) {
        ptr++;
      } else {
        readNextChar = true;
      }
    }
    if (dataStart < end) {
      switch (parserState) {
        case READ_NORMAL_DATA:
          ecParseCharData(dataStart, end - dataStart);
        case READ_KEYWORD:
          keyword.append(dataStart, end - dataStart);
          break;
        case READ_KEYWORD_PARAMETER:
          parameterString.append(dataStart, end - dataStart);
          break;
        default:
          break;
      }
    }
  }
  
  return (is_interrupted || myStateStack.empty()) ? ecOK : ecUnmatchedBrace;
}

RtfReader::ParserState RtfReader::ecTranslateKeyword(const std::string &keyword, int param, bool fParam) {
  std::map<std::string, RtfKeywordInfo*>::const_iterator it = myKeywordMap.find(keyword);
  
  if (it == myKeywordMap.end()) {
    if (fSkipDestIfUnk)     // if this is a new destination
      state.rds = DESTINATION_SKIP;      // skip the destination
                  // else just discard it
    fSkipDestIfUnk = false;

//    DPRINT("unknown keyword\n");    
    return READ_NORMAL_DATA;
  }

  fSkipDestIfUnk = false;

  return it->second->run(*this, fParam ? &param : 0);
}


void RtfReader::ecParseCharData(const char *data, size_t len, bool convert) {
  if (state.rds != DESTINATION_SKIP) {
    addCharData(data, len, convert);
  }
}

void RtfReader::interrupt() {
  is_interrupted = true;
}

bool RtfReader::readDocument(const std::string &fileName) {
  myFileName = fileName;
  myStream = ZLFile(fileName).inputStream();
  if (myStream.isNull() || !myStream->open()) {
      return false;
  }

  fillKeywordMap();

  myStreamBuffer = new char[rtfStreamBufferSize];
  
  is_interrupted = false;
  startDocumentHandler();

  fSkipDestIfUnk = false;

  memset(&state, 0, sizeof(state));
  state.rds = DESTINATION_NONE;
  state.ReadDataAsHex = false;

  int ret = ecRtfParse();
  bool code = ret == ecOK;
  if (!code) {
    std::cerr << "parse failed: " << ret << "\n";
  }
  endDocumentHandler();

  while (!myStateStack.empty()) {
    myStateStack.pop();
  }
  
  delete[] myStreamBuffer;
  myStream->close();
  
  return code;
}
