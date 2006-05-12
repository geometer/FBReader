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

#include <abstract/ZLInputStream.h>

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
  } else {
    myConverter = NULL;
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

typedef enum {istyleIndex} SPROP;
typedef enum {ipfnParagraph, ipfnHex, ipfnBin, ipfnCodePage, ipfnSkipDest,
    ipfnParagraphReset } IPFN;
typedef enum {idestInfo, idestTitle, idestAuthor, idestPict, idestStyleSheet,
    idestFootnote, idestSkip } IDEST;
typedef enum {kwdChar, kwdDest, kwdProp, kwdPictProp, kwdStyle, kwdSpec} KWD;

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
  int DefaultValue;        // default value to use
  bool UseDefaultValue;     // true to use default value from this table
  KWD BaseAction;         // base action to take
  int Index;         // index into property table if kwd == kwdProp
              // index into destination table if kwd == kwdDest
              // character to print if kwd == kwdChar
  
  RtfKeywordInfo(int _DefaultValue, bool _UseDefaultValue, KWD _BaseAction, int _Index) {
    DefaultValue = _DefaultValue;
    UseDefaultValue = _UseDefaultValue;
    BaseAction = _BaseAction;
    Index = _Index;
  }
	virtual ~RtfKeywordInfo() {}

	virtual void run(RtfReader &reader) const {}
};

class RtfKeywordCharInfo : public RtfKeywordInfo {

public:
  RtfKeywordCharInfo(char chr) : RtfKeywordInfo(0, false, kwdChar, 0), myChar(chr) {}
	void run(RtfReader &reader) const {
    reader.ecParseChar(myChar);
	}
  
private:
  char myChar;
};

class RtfKeywordDestinationInfo : public RtfKeywordInfo {

public:
  RtfKeywordDestinationInfo(IDEST dest) : RtfKeywordInfo(0, false, kwdDest, 0), myDest(dest) {}
	void run(RtfReader &reader) const {
    reader.ecChangeDest(myDest);
	}
  
private:
  IDEST myDest;
};

class RtfKeywordPictureInfo : public RtfKeywordInfo {

public:
  RtfKeywordPictureInfo(const std::string &mimeType) : RtfKeywordInfo(0, false, kwdPictProp, 0), myMimeType(mimeType) {}
	void run(RtfReader &reader) const {
    reader.ecApplyPictPropChange(myMimeType);
	}
  
private:
  const std::string myMimeType;
};

static std::map<std::string, RtfKeywordInfo*> myKeywordMap;

typedef struct symbol
{
  char *szKeyword;    // RTF keyword
  int  dflt;        // default value to use
  bool fPassDflt;     // true to use default value from this table
  KWD  kwd;         // base action to take
  int  idx;         // index into property table if kwd == kwdProp
              // index into destination table if kwd == kwdDest
              // character to print if kwd == kwdChar
} SYM;


// Keyword descriptions
SYM rgsymRtf[] = {
//  keyword   dflt  fPassDflt   kwd     idx
  { "\x0a",     0,    false,   kwdSpec,  ipfnParagraph },
  { "\x0d",     0,    false,   kwdSpec,  ipfnParagraph },
  { "'",        0,    false,   kwdSpec,  ipfnHex },
  { "*",        0,    false,   kwdSpec,  ipfnSkipDest },
  { "ansicpg",  0,    false,   kwdSpec,  ipfnCodePage },
  { "bin",      0,    false,   kwdSpec,  ipfnBin },
  { "par",      0,    false,   kwdSpec,  ipfnParagraph },
//  {   "pard",   0,    false,   kwdSpec,  ipfnParagraphReset },

  { "b",             1,    false,   kwdProp,  ipropBold },
  { "cols",          1,    false,   kwdProp,  ipropCols },
  { "facingp",       1,    true,    kwdProp,  ipropFacingp },
  { "fi",            0,    false,   kwdProp,  ipropFirstInd },
  { "i",             1,    false,   kwdProp,  ipropItalic },
  { "landscape",     1,    true,    kwdProp,  ipropLandscape },
  { "li",            0,    false,   kwdProp,  ipropLeftInd },
  { "margb",      1440,    false,   kwdProp,  ipropYaBottom },
  { "margl",      1800,    false,   kwdProp,  ipropXaLeft },
  { "margr",      1800,    false,   kwdProp,  ipropXaRight },
  { "margt",      1440,    false,   kwdProp,  ipropYaTop },
  { "paperh",    15480,    false,   kwdProp,  ipropYaPage },
  { "paperw",    12240,    false,   kwdProp,  ipropXaPage },
  { "pgndec",    pgDec,    true,    kwdProp,  ipropPgnFormat },
  { "pgnlcltr", pgLLtr,    true,    kwdProp,  ipropPgnFormat },
  { "pgnlcrm",  pgLRom,    true,    kwdProp,  ipropPgnFormat },
  { "pgnstart",      1,    true,    kwdProp,  ipropPgnStart },
  { "pgnucltr", pgULtr,    true,    kwdProp,  ipropPgnFormat },
  { "pgnucrm",  pgURom,    true,    kwdProp,  ipropPgnFormat },
  { "pgnx",          0,    false,   kwdProp,  ipropPgnX },
  { "pgny",          0,    false,   kwdProp,  ipropPgnY },
  { "qc",        justC,    true,    kwdProp,  ipropJust },
  { "ql",        justL,    true,    kwdProp,  ipropJust },
  { "qr",        justR,    true,    kwdProp,  ipropJust },
  { "qj",        justF,    true,    kwdProp,  ipropJust },
  { "sbkcol",   sbkCol,    true,    kwdProp,  ipropSbk },
  { "sbkeven",  sbkEvn,    true,    kwdProp,  ipropSbk },
  { "sbknone",  sbkNon,    true,    kwdProp,  ipropSbk },
  { "sbkodd",   sbkOdd,    true,    kwdProp,  ipropSbk },
  { "sbkpage",   sbkPg,    true,    kwdProp,  ipropSbk },
  { "ri",            0,    false,   kwdProp,  ipropRightInd },
  { "u",             1,    false,   kwdProp,  ipropUnderline },

  { "s",             0,    false,   kwdStyle,   istyleIndex },

  };

struct ckw {
  const char *kw;
  char chr;
} charKeyWords[] = {
  { "-",    '-' },
  { "\\",     '\\' },
  { "_",    '-' },
  { "bullet",   '\x95' },
  { "emdash",   '\x97' },
  { "endash",   '\x96' },
  { "ldblquote", '\x93' },
  { "lquote",  '\x91' },
  { "{",    '{' },
  { "}",    '}' },
  { "~",    ' ' },
  { "rdblquote", '\x94' },
  { "rquote",  '\x92' },
  { "tab",    0x09 },
};
  
struct dkw {
	const char *kw;
	IDEST dest;
} destKeyWords[] = {
  { "author",     idestAuthor },
  { "footnote",   idestFootnote },
  { "stylesheet", idestStyleSheet },
  { "info",       idestInfo },
  { "pict",       idestPict },
  { "title",      idestTitle },
  { "buptim",     idestSkip },
  { "colortbl",   idestSkip },
  { "comment",    idestSkip },
  { "creatim",    idestSkip },
  { "doccomm",    idestSkip },
  { "fonttbl",    idestSkip },
  { "footer",     idestSkip },
  { "footerf",    idestSkip },
  { "footerl",    idestSkip },
  { "footerr",    idestSkip },
  { "ftncn",      idestSkip },
  { "ftnsep",     idestSkip },
  { "ftnsepc",    idestSkip },
  { "header",     idestSkip },
  { "headerf",    idestSkip },
  { "headerl",    idestSkip },
  { "headerr",    idestSkip },
  { "keywords",   idestSkip },
  { "operator",   idestSkip },
  { "printim",    idestSkip },
  { "private1",   idestSkip },
  { "revtim",     idestSkip },
  { "rxe",        idestSkip },
  { "subject",    idestSkip },
  { "tc",         idestSkip },
  { "txe",        idestSkip },
  { "xe",         idestSkip },
};
  
static void fillKeywordMap() {
  if (myKeywordMap.empty()) {
    for (unsigned int i = 0; i < sizeof(rgsymRtf) / sizeof(SYM); i++) {
      const symbol &s = rgsymRtf[i];
      myKeywordMap[s.szKeyword] = new RtfKeywordInfo(s.dflt, s.fPassDflt, s.kwd, s.idx);
    }
    for (unsigned int i = 0; i < sizeof(charKeyWords) / sizeof(struct ckw); i++) {
      myKeywordMap[charKeyWords[i].kw] = new RtfKeywordCharInfo(charKeyWords[i].chr);
    }
    for (unsigned int i = 0; i < sizeof(destKeyWords) / sizeof(struct dkw); i++) {
      myKeywordMap[destKeyWords[i].kw] = new RtfKeywordDestinationInfo(destKeyWords[i].dest);
    }
		myKeywordMap["jpegblip"] = new RtfKeywordPictureInfo("image/jpeg");
		myKeywordMap["pngblip"] = new RtfKeywordPictureInfo("image/png");
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

  if (state.rds == rdsSkip)         // If we're skipping text,
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

char style_attributes[1][256];
void RtfReader::ecStyleChange(int st, int val) {
  if (st != istyleIndex) {
    return;
  }

  if (state.rds == rdsStyleSheet) {
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

//
// %%Function: ecChangeDest
//
// Change to the destination specified by idest.
// There's usually more to do here than this...
//

void RtfReader::ecChangeDest(int idest) {
  if (state.rds == rdsSkip) {
    return;
  }

  switch (idest) {
    case idestInfo:
      DPRINT("title info\n");
      state.rds = rdsTitleInfo;
      startElementHandler(_TITLE_INFO);
      break;
    case idestTitle:
      DPRINT("title\n");
      state.rds = rdsTitle;
      startElementHandler(_BOOK_TITLE);
      break;
    case idestAuthor:
      DPRINT("author\n");
      state.rds = rdsAuthor;
      startElementHandler(_AUTHOR);
      break;
    case idestPict:
      DPRINT("picture\n");
      state.ReadDataAsHex = true;
      state.rds = rdsImage;
      startElementHandler(_IMAGE);
      break;
    case idestFootnote:
      DPRINT("footnote\n");
      state.rds = rdsFootnote;
      startElementHandler(_FOOTNOTE);
      break;
    case idestStyleSheet:
      DPRINT("style sheet\n");
      state.rds = rdsStyleSheet;
      startElementHandler(_STYLE_SHEET);
      break;
    default:
      state.rds = rdsSkip;        // when in doubt, skip it...
      break;
  }
}

//
// %%Function: ecEndGroupAction
//
// The destination specified by rds is coming to a close.
// If there's any cleanup that needs to be done, do it now.
//

void RtfReader::ecEndGroupAction(int rds) {
  switch (rds) {
    case rdsTitleInfo:
      DPRINT("info end\n");
      endElementHandler(_TITLE_INFO);
      break;
    case rdsTitle:
      DPRINT("title end\n");
      endElementHandler(_BOOK_TITLE);
      break;
    case rdsAuthor:
      DPRINT("author end\n");
      endElementHandler(_AUTHOR);
      break;
    case rdsImage:
      DPRINT("image end\n");
      endElementHandler(_IMAGE);
      break;
    case rdsFootnote:
      DPRINT("footnote end\n");
      endElementHandler(_FOOTNOTE);
      break;
    case rdsStyleSheet:
      DPRINT("style sheet end\n");
      endElementHandler(_STYLE_INFO);
      break;
    default:
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
  if (state.rds == rdsSkip && ipfn != ipfnBin)  // if we're skipping, and it's not
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
          ecParseChar(*ptr);
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
            ecParseChar(ch);
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
  ParserState parserState = READ_NORMAL_DATA;

  std::map<std::string, RtfKeywordInfo*>::const_iterator it = myKeywordMap.find(keyword);
  
  if (it == myKeywordMap.end()) {
    if (fSkipDestIfUnk)     // if this is a new destination
      state.rds = rdsSkip;      // skip the destination
                  // else just discard it
    fSkipDestIfUnk = false;

//    DPRINT("unknown keyword\n");    
    return parserState;
  }

  // found it!  use kwd and idx to determine what to do with it.

  fSkipDestIfUnk = false;
  const RtfKeywordInfo &keywordInfo = *it->second;
  switch (keywordInfo.BaseAction) {
    case kwdProp:
      if (keywordInfo.UseDefaultValue || !fParam)
        param = keywordInfo.DefaultValue;
      ecApplyPropChange(keywordInfo.Index, param);
			break;
    case kwdChar:
    case kwdPictProp:
    case kwdDest:
			keywordInfo.run(*this);
			break;
    case kwdStyle:
      ecStyleChange(keywordInfo.Index, param);
			break;
    case kwdSpec:
      parserState = ecParseSpecialKeyword(keywordInfo.Index, param);
			break;
		default:
	    std::cerr << "parse failed: bad table 5\n";
			break;
  }
	return parserState;
}


//
// %%Function: ecParseChar
//
// Route the character to the appropriate destination stream.
//

void RtfReader::ecParseChar(char ch) {
  switch (state.rds) {
    case rdsSkip:
    default:
      return;
    case rdsTitle:
    case rdsAuthor:
    case rdsContent:
    case rdsFootnote:
    case rdsImage:
      addChar(ch);
  }
}

void RtfReader::ecParseCharData(const char *data, size_t len) {
  switch (state.rds) {
    case rdsSkip:
    default:
      return;
    case rdsTitle:
    case rdsAuthor:
    case rdsContent:
    case rdsFootnote:
    case rdsImage:
      addCharData(data, len);
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
  state.rds = rdsContent;
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
