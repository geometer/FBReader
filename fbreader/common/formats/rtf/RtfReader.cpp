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

#include <cctype>

#include <abstract/ZLInputStream.h>

#include "RtfReader.h"

// RTF parser error codes

static const int rtfStreamBufferSize = 4096;

#define ecOK 0                      // Everything's fine!
#define ecStackUnderflow    1       // Unmatched '}'
#define ecStackOverflow     2       // Too many '{' -- memory exhausted
#define ecUnmatchedBrace    3       // RTF ended during an open group.
#define ecInvalidHex        4       // invalid hex character found in data
#define ecBadTable          5       // RTF table (sym or prop) invalid
#define ecAssertion         6       // Assertion failure
#define ecEndOfFile         7       // End of file reached while reading RTF
#define ecNoEncoding            8

RtfReader::RtfReader(const std::string &encoding) {
  this->encoding = encoding;
  
  if (!encoding.empty())
  {
    myConverter = ZLEncodingConverter::createConverter(encoding);
  }
  else
  {
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
typedef enum {ipfnParagraph, ipfnBin, ipfnHex, ipfnCodePage, ipfnSkipDest,
	      ipfnParagraphReset } IPFN;
typedef enum {idestInfo, idestTitle, idestAuthor, idestPict, idestStyleSheet,
	      idestFootnote, idestSkip } IDEST;
typedef enum {kwdChar, kwdDest, kwdProp, kwdPictProp, kwdStyle, kwdSpec} KWD;

typedef struct propmod
{
    ACTN actn;              // size of value
    PROPTYPE prop;          // structure containing value
    int  offset;            // offset of value from base of structure
} PROP;

// RTF parser tables

// Property descriptions
PROP rgprop [ipropMax] = {
    { actnByte,   propChp,    offsetof(CHP, fBold) },       // ipropBold
    { actnByte,   propChp,    offsetof(CHP, fItalic) },     // ipropItalic
    { actnByte,   propChp,    offsetof(CHP, fUnderline) },  // ipropUnderline
    { actnWord,   propPap,    offsetof(PAP, xaLeft) },      // ipropLeftInd
    { actnWord,   propPap,    offsetof(PAP, xaRight) },     // ipropRightInd
    { actnWord,   propPap,    offsetof(PAP, xaFirst) },     // ipropFirstInd
    { actnWord,   propSep,    offsetof(SEP, cCols) },       // ipropCols
    { actnWord,   propSep,    offsetof(SEP, xaPgn) },       // ipropPgnX
    { actnWord,   propSep,    offsetof(SEP, yaPgn) },       // ipropPgnY
    { actnWord,   propDop,    offsetof(DOP, xaPage) },      // ipropXaPage
    { actnWord,   propDop,    offsetof(DOP, yaPage) },      // ipropYaPage
    { actnWord,   propDop,    offsetof(DOP, xaLeft) },      // ipropXaLeft
    { actnWord,   propDop,    offsetof(DOP, xaRight) },     // ipropXaRight
    { actnWord,   propDop,    offsetof(DOP, yaTop) },       // ipropYaTop
    { actnWord,   propDop,    offsetof(DOP, yaBottom) },    // ipropYaBottom
    { actnWord,   propDop,    offsetof(DOP, pgnStart) },    // ipropPgnStart
    { actnWord,   propSep,    offsetof(SEP, sbk) },         // ipropSbk
    { actnWord,   propSep,    offsetof(SEP, pgnFormat) },   // ipropPgnFormat
    { actnByte,   propDop,    offsetof(DOP, fFacingp) },    // ipropFacingp
    { actnByte,   propDop,    offsetof(DOP, fLandscape) },  // ipropLandscape
    { actnWord,   propPap,    offsetof(PAP, just) },        // ipropJust
    { actnSpec,   propPap,    0 },                          // ipropPard
    { actnSpec,   propChp,    0 },                          // ipropPlain
    { actnSpec,   propSep,    0 },                          // ipropSectd
};

struct RtfKeyword {
  int DefaultValue;              // default value to use
  bool UseDefaultValue;         // true to use default value from this table
  KWD BaseAction;               // base action to take
  int Index;               // index into property table if kwd == kwdProp
                            // index into destination table if kwd == kwdDest
                            // character to print if kwd == kwdChar
	
	RtfKeyword(int _DefaultValue, bool _UseDefaultValue, KWD _BaseAction, int _Index) {
		DefaultValue = _DefaultValue;
		UseDefaultValue = _UseDefaultValue;
		BaseAction = _BaseAction;
		Index = _Index;
	}
};

static std::map<std::string, RtfKeyword> myKeywordMap;

typedef struct symbol
{
    char *szKeyword;        // RTF keyword
    int  dflt;              // default value to use
    bool fPassDflt;         // true to use default value from this table
    KWD  kwd;               // base action to take
    int  idx;               // index into property table if kwd == kwdProp
                            // index into destination table if kwd == kwdDest
                            // character to print if kwd == kwdChar
} SYM;


// Keyword descriptions
SYM rgsymRtf[] = {
//  keyword     dflt    fPassDflt   kwd         idx
    { "\x0a",     0,      false,     kwdSpec,    ipfnParagraph },
    { "\x0d",     0,      false,     kwdSpec,    ipfnParagraph },
    { "'",        0,      false,     kwdSpec,    ipfnHex },
    { "*",        0,      false,     kwdSpec,    ipfnSkipDest },
    { "-",        0,      false,     kwdChar,    '-' },
    { "\\",       0,      false,     kwdChar,    '\\' },
    { "_",        0,      false,     kwdChar,    '-' },
    { "ansicpg",  0,      false,     kwdSpec,    ipfnCodePage },
    { "author",   0,      false,     kwdDest,    idestAuthor },
    { "b",        1,      false,     kwdProp,    ipropBold },
    { "bin",      0,      false,     kwdSpec,    ipfnBin },
    { "bullet",   0,      false,     kwdChar,    '\x95' },
    { "buptim",   0,      false,     kwdDest,    idestSkip },
    { "colortbl", 0,      false,     kwdDest,    idestSkip },
    { "cols",     1,      false,     kwdProp,    ipropCols },
    { "comment",  0,      false,     kwdDest,    idestSkip },
    { "creatim",  0,      false,     kwdDest,    idestSkip },
    { "doccomm",  0,      false,     kwdDest,    idestSkip },
    { "emdash",   0,      false,     kwdChar,    '\x97' },
    { "endash",   0,      false,     kwdChar,    '\x96' },
    { "facingp",  1,      true,      kwdProp,    ipropFacingp },
    { "fi",       0,      false,     kwdProp,    ipropFirstInd },
    { "fonttbl",  0,      false,     kwdDest,    idestSkip },
    { "footer",   0,      false,     kwdDest,    idestSkip },
    { "footerf",  0,      false,     kwdDest,    idestSkip },
    { "footerl",  0,      false,     kwdDest,    idestSkip },
    { "footerr",  0,      false,     kwdDest,    idestSkip },
    { "footnote", 0,      false,     kwdDest,    idestFootnote },
    { "ftncn",    0,      false,     kwdDest,    idestSkip },
    { "ftnsep",   0,      false,     kwdDest,    idestSkip },
    { "ftnsepc",  0,      false,     kwdDest,    idestSkip },
    { "header",   0,      false,     kwdDest,    idestSkip },
    { "headerf",  0,      false,     kwdDest,    idestSkip },
    { "headerl",  0,      false,     kwdDest,    idestSkip },
    { "headerr",  0,      false,     kwdDest,    idestSkip },
    { "i",        1,      false,     kwdProp,    ipropItalic },
    { "info",     0,      false,     kwdDest,    idestInfo },
    { "jpegblip", 0,         false,            kwdPictProp, ppropJpeg },
    { "keywords", 0,      false,     kwdDest,    idestSkip },
    { "landscape",1,      true,      kwdProp,    ipropLandscape },
    { "ldblquote",    0,      false,     kwdChar,     '\x93' },
    { "li",       0,      false,     kwdProp,    ipropLeftInd },
    { "lquote",    0,      false,     kwdChar,     '\x91' },
    { "margb",    1440,   false,     kwdProp,    ipropYaBottom },
    { "margl",    1800,   false,     kwdProp,    ipropXaLeft },
    { "margr",    1800,   false,     kwdProp,    ipropXaRight },
    { "margt",    1440,   false,     kwdProp,    ipropYaTop },
    { "operator", 0,      false,     kwdDest,    idestSkip },
    { "paperh",   15480,  false,     kwdProp,    ipropYaPage },
    { "paperw",   12240,  false,     kwdProp,    ipropXaPage },
    { "par",      0,      false,     kwdSpec,    ipfnParagraph },
//  {   "pard",     0,      false,     kwdSpec,    ipfnParagraphReset },
    { "pgndec",   pgDec,  true,      kwdProp,    ipropPgnFormat },
    { "pgnlcltr", pgLLtr, true,      kwdProp,    ipropPgnFormat },
    { "pgnlcrm",  pgLRom, true,      kwdProp,    ipropPgnFormat },
    { "pgnstart", 1,      true,      kwdProp,    ipropPgnStart },
    { "pgnucltr", pgULtr, true,      kwdProp,    ipropPgnFormat },
    { "pgnucrm",  pgURom, true,      kwdProp,    ipropPgnFormat },
    { "pgnx",     0,      false,     kwdProp,    ipropPgnX },
    { "pgny",     0,      false,     kwdProp,    ipropPgnY },
    { "pict",     0,      false,     kwdDest,    idestPict },
    { "pngblip",         0,         false,            kwdPictProp, ppropPng },
    { "printim",  0,      false,     kwdDest,    idestSkip },
    { "private1", 0,      false,     kwdDest,    idestSkip },
    { "qc",       justC,  true,      kwdProp,    ipropJust },
    { "ql",       justL,  true,      kwdProp,    ipropJust },
    { "qr",       justR,  true,      kwdProp,    ipropJust },
    { "qj",       justF,  true,      kwdProp,    ipropJust },
    { "rdblquote",    0,      false,     kwdChar,     '\x94' },
    { "revtim",   0,      false,     kwdDest,    idestSkip },
    { "ri",       0,      false,     kwdProp,    ipropRightInd },
    { "rquote",    0,      false,     kwdChar,     '\x92' },
    { "rxe",      0,      false,     kwdDest,    idestSkip },

    { "s",        0,      false,     kwdStyle,   istyleIndex },
    { "sbkcol",   sbkCol, true,      kwdProp,    ipropSbk },
    { "sbkeven",  sbkEvn, true,      kwdProp,    ipropSbk },
    { "sbknone",  sbkNon, true,      kwdProp,    ipropSbk },
    { "sbkodd",   sbkOdd, true,      kwdProp,    ipropSbk },
    { "sbkpage",  sbkPg,  true,      kwdProp,    ipropSbk },
    { "stylesheet",   0,  false,     kwdDest,    idestStyleSheet },
    { "subject",  0,      false,     kwdDest,    idestSkip },
    { "tab",      0,      false,     kwdChar,    0x09 },
    { "tc",       0,      false,     kwdDest,    idestSkip },
    { "title",    0,      false,     kwdDest,    idestTitle },
    { "txe",      0,      false,     kwdDest,    idestSkip },
    { "u",        1,      false,     kwdProp,    ipropUnderline },
    { "xe",       0,      false,     kwdDest,    idestSkip },

    { "{",        0,      false,     kwdChar,    '{' },
    { "}",        0,      false,     kwdChar,    '}' },
    { "~",        0,      false,     kwdChar,    ' ' },
    };

static void fillKeywordMap() {
	if (myKeywordMap.empty()) {
		for (unsigned int i = 0; i < sizeof(rgsymRtf) / sizeof(SYM); i++) {
			const symbol &s = rgsymRtf[i];
			myKeywordMap.insert(
				std::pair<std::string,RtfKeyword>(s.szKeyword, RtfKeyword(s.dflt, s.fPassDflt, s.kwd, s.idx))
			);
		}
	}
}

//
// %%Function: ecApplyPropChange
//
// Set the property identified by _iprop_ to the value _val_.
//
//

int RtfReader::ecApplyPropChange(int iprop, int val)
{
  char *pb = NULL;
  CHP oldChp = state.chp;
  
//  DPRINT("Aply prop change: %i %i\n", iprop, val);

  if (state.rds == rdsSkip)                 // If we're skipping text,
      return ecOK;                    // don't do anything.

  switch (rgprop[iprop].prop)
  {
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
      if (rgprop[iprop].actn != actnSpec)
    {
          DPRINT("parse failed: bad table\n");
          return ecBadTable;
    }
      break;
  }
  
  switch (rgprop[iprop].actn)
  {
  case actnByte:
      (*(bool *) (pb + rgprop[iprop].offset)) = (val != 0);
      break;
  case actnWord:
      (*(int *) (pb+rgprop[iprop].offset)) = val;
      break;
  case actnSpec:
      return ecParseSpecialProperty(iprop, val);
      break;
  default:
      {
      DPRINT("parse failed: bad table\n");
      return ecBadTable;
    }
  }
  
  if (state.chp.fItalic != oldChp.fItalic)
  {
      if (state.chp.fItalic)
      {
        startElementHandler(_ITALIC);
    }
      else
      {
        endElementHandler(_ITALIC);
    }
  }

  if (state.chp.fBold != oldChp.fBold)
  {
      if (state.chp.fBold)
      {
        startElementHandler(_BOLD);
      }
      else
      {
        endElementHandler(_BOLD);
    }
  }
  
  return ecOK;
}

char style_attributes[1][256];
int RtfReader::ecStyleChange(int st, int val)
{
    if (st != istyleIndex)
    {
        return ecOK;
    }

    if (state.rds == rdsStyleSheet)
    {
        DPRINT ("Add style index: %i\n", val);
        
        sprintf(style_attributes[0], "%i", val);
        startElementHandler(_STYLE_INFO, (const char **) style_attributes);
    }
    else /*if (state.rds == rdsContent)*/
    {
        DPRINT ("Change style index: %i\n", val);

        sprintf(style_attributes[0], "%i", val);
        startElementHandler(_STYLE_SET, (const char **) style_attributes);
    }
    
    return ecOK;
}

//
// %%Function: ecParseSpecialProperty
//
// Set a property that requires code to evaluate.
//

int RtfReader::ecParseSpecialProperty(int iprop, int val)
{
    switch (iprop)
    {
    case ipropPard:
        memset(&state.pap, 0, sizeof(state.pap));
        return ecOK;
    case ipropPlain:
        if (state.chp.fItalic)
        {
            endElementHandler(_ITALIC);
        }

        if (state.chp.fBold)
        {
            endElementHandler(_BOLD);
        }
    
        memset(&state.chp, 0, sizeof(state.chp));
        return ecOK;
    case ipropSectd:
        memset(&state.sep, 0, sizeof(state.sep));
        return ecOK;
    default:
        return ecBadTable;
    }
    
    DPRINT("parse failed: bad table\n");

    return ecBadTable;
}

static const char *image_png_type[] = {"image/png"};
static const char *image_jpeg_type[] = {"image/jpeg"};
int RtfReader::ecApplyPictPropChange(int pprop)
{
    switch (pprop)
    {
    case ppropPng:
        startElementHandler(_IMAGE_TYPE, image_png_type);
        return ecOK;
    case ppropJpeg:
        startElementHandler(_IMAGE_TYPE, image_jpeg_type);
        return ecOK;
    default:
        DPRINT("parse failed: bad table\n");
        return ecBadTable;
    }

    return ecBadTable;
} 
//
// %%Function: ecTranslateKeyword.
//
// Step 3.
// Search rgsymRtf for szKeyword and evaluate it appropriately.
//
// Inputs:
// szKeyword:   The RTF control to evaluate.
// param:       The parameter of the RTF control.
// fParam:      true if the control had a parameter; (that is, if param is valid)
//              false if it did not.
//

int RtfReader::ecTranslateKeyword(char *szKeyword, int param, bool fParam)
{
    DPRINT("keyword: %s[%x, %i], param: %i, fparam: %i\n", 
       szKeyword, szKeyword[0], strlen(szKeyword), param, fParam);

		fillKeywordMap();

		std::map<std::string, RtfKeyword>::const_iterator it = myKeywordMap.find(szKeyword);
    
		if (it == myKeywordMap.end()) {
        if (fSkipDestIfUnk)         // if this is a new destination
            state.rds = rdsSkip;          // skip the destination
                                    // else just discard it
        fSkipDestIfUnk = false;

//        DPRINT("unknown keyword\n");        
        return ecOK;
    }

    // found it!  use kwd and idx to determine what to do with it.

    fSkipDestIfUnk = false;
		const RtfKeyword &keyword = it->second;
    switch (keyword.BaseAction) {
        case kwdProp:
            if (keyword.UseDefaultValue || !fParam)
                param = keyword.DefaultValue;
            return ecApplyPropChange(keyword.Index, param);
        case kwdChar:
            return ecParseChar(keyword.Index);
        case kwdDest:
            return ecChangeDest(keyword.Index);
        case kwdPictProp:
            return ecApplyPictPropChange(keyword.Index);
        case kwdStyle:
            return ecStyleChange(keyword.Index, param);
        case kwdSpec:
            return ecParseSpecialKeyword(keyword.Index, param);
        default:
            DPRINT("parse failed: bad table\n");

            return ecBadTable;
    }

    DPRINT("parse failed: bad table\n");

    return ecBadTable;
}

//
// %%Function: ecChangeDest
//
// Change to the destination specified by idest.
// There's usually more to do here than this...
//

int RtfReader::ecChangeDest(int idest)
{
    if (state.rds == rdsSkip)             // if we're skipping text,
    {
//        DPRINT("skipping...\n");
        return ecOK;                // don't do anything
    }

    switch (idest)
    {
        case idestInfo:
        {
                DPRINT("title info\n");
            state.rds = rdsTitleInfo;
            startElementHandler(_TITLE_INFO);
            break;
        }
        case idestTitle:
        {
                DPRINT("title\n");
            state.rds = rdsTitle;
            startElementHandler(_BOOK_TITLE);
            break;
        }
        case idestAuthor:
        {
                DPRINT("author\n");
            state.rds = rdsAuthor;
            startElementHandler(_AUTHOR);
            break;
        }
        case idestPict:
        {
                DPRINT("picture\n");
            state.ris = risHexStream;
            state.rds = rdsImage;
            startElementHandler(_IMAGE);
            break;
        }
        case idestFootnote:
        {
            DPRINT("footnote\n");
            state.rds = rdsFootnote;
            startElementHandler(_FOOTNOTE);
            break;
        }
        case idestStyleSheet:
        {
            DPRINT("style sheet\n");
            state.rds = rdsStyleSheet;
            startElementHandler(_STYLE_SHEET);
            break;
        }
        default:
        {
            state.rds = rdsSkip;              // when in doubt, skip it...
            break;
        }
    }
    return ecOK;
}

//
// %%Function: ecEndGroupAction
//
// The destination specified by rds is coming to a close.
// If there's any cleanup that needs to be done, do it now.
//

int RtfReader::ecEndGroupAction(int rds)
{
    if (rds == rdsSkip)             // if we're skipping text,
    {
        return ecOK;                // don't do anything
    }

    switch (rds)
    {
        case rdsTitleInfo:
        {
            DPRINT("info end\n");
            endElementHandler(_TITLE_INFO);
            break;
        }
        case rdsTitle:
        {
            DPRINT("title end\n");
            endElementHandler(_BOOK_TITLE);
            break;
        }
        case rdsAuthor:
        {
            DPRINT("author end\n");
            endElementHandler(_AUTHOR);
            break;
        }
        case rdsImage:
        {
            DPRINT("image end\n");
            endElementHandler(_IMAGE);
            break;
        }
        case rdsFootnote:
        {
            DPRINT("footnote end\n");
            endElementHandler(_FOOTNOTE);
            break;
        }
        case rdsStyleSheet:
        {
            DPRINT("style sheet end\n");
            endElementHandler(_STYLE_INFO);
            break;
        }
        default:
        {
            break;
        }
    }
    return ecOK;
}

//
// %%Function: ecParseSpecialKeyword
//
// Evaluate an RTF control that needs special processing.
//
static const char *encoding1251 = "windows-1251";

int RtfReader::ecParseSpecialKeyword(int ipfn, int param)
{
    if (state.rds == rdsSkip && ipfn != ipfnBin)  // if we're skipping, and it's not
        return ecOK;                        // the \bin keyword, ignore it.
    switch (ipfn)
    {
    case ipfnParagraph:
        startElementHandler(_P);
        break;
    case ipfnParagraphReset:
        startElementHandler(_P_RESET);
        break;
    case ipfnBin:
        state.ris = risBin;
        cbBin = param;
        break;
    case ipfnCodePage:
        startElementHandler(_ENCODING);
        if ((param == 1251) && (encoding != encoding1251))
        {
            encoding = encoding1251;
            myConverter = ZLEncodingConverter::createConverter(encoding);
        }
        else
        {
        }
        break;
    case ipfnSkipDest:
        fSkipDestIfUnk = true;
        break;
    case ipfnHex:
        state.ris = risHexSingle;
        break;
    default:
        DPRINT("parse failed: bad table\n");

        return ecBadTable;
    }
    return ecOK;
}

int RtfReader::getChar(void)
{
    int ch;
    
//    DPRINT("get char at: %i{%i}\n", startCounter, endCounter);
    
    if (startCounter == endCounter)
    {
        if (startCounter != rtfStreamBufferSize)
        {
            DPRINT("EOF\n");
            return -1;
        }

        startCounter = 0;
        endCounter = myStream->read(myStreamBuffer, rtfStreamBufferSize);
        
        if (endCounter == 0)
        {
            DPRINT("EOF\n");
            return -1;
        }
    }
    
    ch = (unsigned char)myStreamBuffer[startCounter];
    startCounter++;
    
//    DPRINT("result %c{%i}\n", ch, startCounter);
    
    return ch;
}

void RtfReader::unGetChar(int ch)
{
//    DPRINT("un get char at: %i{%i}\n", startCounter, endCounter);
    
    startCounter--;
    myStreamBuffer[startCounter] = ch;
}

int RtfReader::ecRtfParse() {
    int ch;
    int ec;
    int cNibble = 2;
    int b = 0;

    while (((ch = getChar()) != -1) && !is_interrupted)
    {
//        DPRINT("main loop char: %c\n", ch);
        
        if (cGroup < 0)
        {
            DPRINT("parse failed: StackUnderflow\n");

            return ecStackUnderflow;
        }
        if (state.ris == risBin)                      // if we're parsing binary data, handle it directly
        {
            if ((ec = ecParseChar(ch)) != ecOK)
                return ec;
        }
        else
        {
            switch (ch)
            {
            case '{':
                DPRINT("{");
                stack.push_back(state);
                state.ris = risNorm;
                cGroup++;
                break;
            case '}':
                int ec;
                CHP oldChp;

                DPRINT("}");

                if (cGroup == 0)
                {
                    DPRINT("parse failed: stack underflow\n");

                    return ecStackUnderflow;
                }
                
                oldChp = state.chp;
                DPRINT("old chp: %i, %i\n", oldChp.fItalic, oldChp.fBold);
                
                if (state.rds != stack.back().rds)
                {
                    if ((ec = ecEndGroupAction(state.rds)) != ecOK)
                    return ec;
                }
                
                state = stack.back();
                stack.pop_back();

                cGroup--;

                if (state.chp.fItalic != oldChp.fItalic)
                {
                    DPRINT("new italic: %i\n", state.chp.fItalic);
                    if (state.chp.fItalic)
                    {
                            startElementHandler(_ITALIC);
                    }
                    else
                    {
                        endElementHandler(_ITALIC);
                    }
                }

                if (state.chp.fBold != oldChp.fBold)
                {
                    DPRINT("new bold: %i\n", state.chp.fBold);
                    if (state.chp.fBold)
                    {
                            startElementHandler(_BOLD);
                    }
                    else
                    {
                        endElementHandler(_BOLD);
                    }
                }
                
                break;
            case '\\':
                if ((ec = ecParseRtfKeyword()) != ecOK)
                    return ec;
                break;
            case 0x0d:
            case 0x0a:          // cr and lf are noise characters...
                break;
            default:
                if (state.ris == risNorm) {
                    if ((ec = ecParseChar(ch)) != ecOK) {
                        return ec;
										}
                }
                else {               // parsing hex data
                    b = b << 4;

                    if (isdigit(ch)) {
                        b += (char) ch - '0';
                    } else {
                        if (islower(ch)) {
                            if (ch < 'a' || ch > 'f') {
                                DPRINT("parse failed: invalid hex\n");

                                return ecInvalidHex;
                            }
                            b += (char) ch - 'a' + 10;
                        }
                        else {
                            if (ch < 'A' || ch > 'F') {
                                DPRINT("parse failed: invalid hex\n");

                                return ecInvalidHex;
                            }
                            b += (char) ch - 'A' + 10;
                        }
                    }

                    cNibble--;
                    if (!cNibble)
                    {

                        if ((ec = ecParseChar(b)) != ecOK)
                            return ec;
                        cNibble = 2;
                        b = 0;
                        
                        if (state.ris == risHexSingle)
                        {
                            state.ris = risNorm;
                        }
                    }
                }                   // end else (ris != risNorm)
                break;
            }       // switch
        }           // else (ris != risBin)
    }               // while
    
    if (is_interrupted)
    {
        return ecOK;
    }
    
    if (cGroup < 0)
    {
        DPRINT("parse failed: stack underflow\n");

        return ecStackUnderflow;
    }
    
    if (cGroup > 0)
    {
        DPRINT("parse failed: unmatched brace\n");

        return ecUnmatchedBrace;
    }
    return ecOK;
}

//
// %%Function: ecParseRtfKeyword
//
// Step 2:
// get a control word (and its associated value) and
// call ecTranslateKeyword to dispatch the control.
//

int RtfReader::ecParseRtfKeyword(void)
{
    int ch;
    char fParam = false;
    char fNeg = false;
    int param = 0;
    char *pch;
    char szKeyword[30];
    char szParameter[20];

//    DPRINT("parse keyword\n");

    szKeyword[0] = '\0';
    szParameter[0] = '\0';
    
    if ((ch = getChar()) == -1)
    {
        DPRINT("parse failed: end of file\n");

        return ecEndOfFile;
    }
        
    if (!isalpha(ch))           // a control symbol; no delimiter.
    {
        szKeyword[0] = (char) ch;
        szKeyword[1] = '\0';

        return ecTranslateKeyword(szKeyword, 0, fParam);
    }
    
    for (pch = szKeyword; isalpha(ch); ch = getChar(), pch++)
    {
        pch[0] = (char) ch;
    }

    pch[0] = '\0';

//    DPRINT("keyword: %s, next char: %c\n", szKeyword, ch);

    if (ch == '-')
    {
        fNeg  = true;
        if ((ch = getChar()) == -1)
        {
            DPRINT("parse failed: end of file\n");
            return ecEndOfFile;
        }
    }
    
    if (isdigit(ch))
    {
        fParam = true;         // a digit after the control means we have a parameter
        for (pch = szParameter; isdigit(ch); ch = getChar(), pch++)
        {
            pch[0] = (char) ch;
        }
        pch[0] = '\0';
        param = atoi(szParameter);
        if (fNeg)
            param = -param;
        lParam = atol(szParameter);
        if (fNeg)
            param = -param;
    }

//    DPRINT("parameter: %s\n", szParameter);
    
    if (ch != ' ')
        unGetChar(ch);
        
    return ecTranslateKeyword(szKeyword, param, fParam);
}

//
// %%Function: ecParseChar
//
// Route the character to the appropriate destination stream.
//

int RtfReader::ecParseChar(int ch)
{
//    DPRINT("%c", ch);
    
    if (state.ris == risBin && --cbBin <= 0)
        state.ris = risNorm;
    switch (state.rds)
    {
    case rdsSkip:
//        DPRINT("%c", ch);
        // Toss this character.
        return ecOK;
    case rdsTitle:
    case rdsAuthor:
    case rdsContent:
    case rdsFootnote:
    case rdsImage:
        return characterPrint(ch) ? ecOK : ecNoEncoding;
    default:
    // handle other destinations....
        return ecOK;
    }
}

void RtfReader::interrupt(void)
{
//    flushBuffer();
    is_interrupted = true;
}

bool RtfReader::readDocument(shared_ptr<ZLInputStream> stream) {
    DPRINT("readDocument\n");
    if (stream.isNull() || !stream->open()) {
            return false;
    }

		myStream = stream;
    myStreamBuffer = new char[rtfStreamBufferSize];
    
    startCounter = endCounter = rtfStreamBufferSize;

    is_interrupted = false;
    startDocumentHandler();

    cGroup = 0;
    fSkipDestIfUnk = false;
    cbBin = 0;
    lParam = 0;

    stack.reserve(30);

    memset(&state, 0, sizeof(state));
    state.rds = rdsContent;
    state.ris = risNorm;

    int ret = ecRtfParse();
    bool code = ret != ecOK;
    if (!code) {
        DPRINT("parse failed: %i\n", ret);
		}
    endDocumentHandler();
    stack.reserve(0);
		
    delete[] myStreamBuffer;
    myStream->close();
    
		return code;
}
