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

std::map<std::string, RtfCommand*> RtfReader::ourKeywordMap;

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

typedef enum {ipfnCodePage, ipfnSkipDest } IPFN;

struct RtfCommand {

protected:
  virtual ~RtfCommand() {}

public:
  virtual RtfReader::ParserState run(RtfReader &reader, int *parameter) const = 0;
};

class RtfNewParagraphCommand : public RtfCommand {

public:
  RtfNewParagraphCommand() {}
  RtfReader::ParserState run(RtfReader &reader, int *parameter) const {
    reader.newParagraph();
    return RtfReader::READ_NORMAL_DATA;
  }
};

class RtfFontPropertyCommand : public RtfCommand {

public:
  RtfFontPropertyCommand(RtfReader::FontProperty property) : myProperty(property) {}
  RtfReader::ParserState run(RtfReader &reader, int *parameter) const {
    reader.ecApplyPropChange(myProperty, (parameter == 0) || (*parameter == 1));
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
  RtfReader::FontProperty myProperty;
};

class RtfAlignmentCommand : public RtfCommand {

public:
  RtfAlignmentCommand(AlignmentType alignment) : myAlignment(alignment) {}
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    //std::cerr << "Alignment = " << myAlignment << "\n";
    reader.setAlignment(myAlignment);
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
  AlignmentType myAlignment;
};

class RtfCharCommand : public RtfCommand {

public:
  RtfCharCommand(const std::string &chr) : myChar(chr) {}
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    reader.ecParseCharData(myChar.data(), myChar.length(), false);
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
  std::string myChar;
};

class RtfDestinationCommand : public RtfCommand {

public:
  RtfDestinationCommand(Destination dest) : myDest(dest) {}
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    reader.ecChangeDest(myDest);
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
  Destination myDest;
};

class RtfStyleCommand : public RtfCommand {

public:
  RtfStyleCommand() {}
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    reader.ecStyleChange();
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
};

class RtfSpecCommand : public RtfCommand {

public:
  RtfSpecCommand(IPFN ipfn) : myIpfn(ipfn) {}
  RtfReader::ParserState run(RtfReader &reader, int *parameter) const {
    return reader.ecParseSpecialKeyword(myIpfn, parameter ? *parameter : 0);
  }
  
private:
  IPFN myIpfn;
};

class RtfPictureCommand : public RtfCommand {

public:
  RtfPictureCommand(const std::string &mimeType) : myMimeType(mimeType) {}
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    reader.ecApplyPictPropChange(myMimeType);
    return RtfReader::READ_NORMAL_DATA;
  }
  
private:
  const std::string myMimeType;
};

class RtfFontResetCommand : public RtfCommand {

public:
  RtfReader::ParserState run(RtfReader &reader, int*) const {
    reader.ecApplyPropChange(RtfReader::FONT_BOLD, false);
    reader.ecApplyPropChange(RtfReader::FONT_ITALIC, false);
    reader.ecApplyPropChange(RtfReader::FONT_UNDERLINED, false);
    return RtfReader::READ_NORMAL_DATA;
  }
};

struct skw {
  const char *kw;
  IPFN ipfn;
} specKeyWords[] = {
  { "*",        ipfnSkipDest },
  { "ansicpg",  ipfnCodePage },
};

void RtfReader::fillKeywordMap() {
  if (ourKeywordMap.empty()) {
    for (unsigned int i = 0; i < sizeof(specKeyWords) / sizeof(struct skw); i++) {
      ourKeywordMap[specKeyWords[i].kw] = new RtfSpecCommand(specKeyWords[i].ipfn);
    }

    static const char *keywordsToSkip[] = {"buptim", "colortbl", "comment", "creatim", "doccomm", "fonttbl", "footer", "footerf", "footerl", "footerr", "ftncn", "ftnsep", "ftnsepc", "header", "headerf", "headerl", "headerr", "keywords", "operator", "printim", "private1", "revtim", "rxe", "subject", "tc", "txe", "xe", 0};
    RtfCommand *skipCommand = new RtfDestinationCommand(DESTINATION_NONE);
    for (const char **i = keywordsToSkip; *i != 0; i++) {
      ourKeywordMap[*i] = skipCommand;
    }
    ourKeywordMap["info"] = new RtfDestinationCommand(DESTINATION_INFO);
    ourKeywordMap["title"] = new RtfDestinationCommand(DESTINATION_TITLE);
    ourKeywordMap["author"] = new RtfDestinationCommand(DESTINATION_AUTHOR);
    ourKeywordMap["pict"] = new RtfDestinationCommand(DESTINATION_PICTURE);
    ourKeywordMap["stylesheet"] = new RtfDestinationCommand(DESTINATION_STYLESHEET);
    ourKeywordMap["footnote"] = new RtfDestinationCommand(DESTINATION_FOOTNOTE);

    RtfCommand *newParagraphCommand = new RtfNewParagraphCommand();
    ourKeywordMap["\n"] = newParagraphCommand;
    ourKeywordMap["\r"] = newParagraphCommand;
    ourKeywordMap["par"] = newParagraphCommand;

    ourKeywordMap["\x09"] = new RtfCharCommand("\x09");
    ourKeywordMap["\\"] = new RtfCharCommand("\\");
    ourKeywordMap["{"] = new RtfCharCommand("{");
    ourKeywordMap["}"] = new RtfCharCommand("}");
    ourKeywordMap["bullet"] = new RtfCharCommand("\xE2\x80\xA2");     // &bullet;
    ourKeywordMap["endash"] = new RtfCharCommand("\xE2\x80\x93");     // &ndash;
    ourKeywordMap["emdash"] = new RtfCharCommand("\xE2\x80\x94");     // &mdash;
    ourKeywordMap["~"] = new RtfCharCommand("\xC0\xA0");              // &nbsp;
    ourKeywordMap["enspace"] = new RtfCharCommand("\xE2\x80\x82");    // &emsp;
    ourKeywordMap["emspace"] = new RtfCharCommand("\xE2\x80\x83");    // &ensp;
    ourKeywordMap["lquote"] = new RtfCharCommand("\xE2\x80\x98");     // &lsquo;
    ourKeywordMap["rquote"] = new RtfCharCommand("\xE2\x80\x99");     // &rsquo;
    ourKeywordMap["ldblquote"] = new RtfCharCommand("\xE2\x80\x9C");  // &ldquo;
    ourKeywordMap["rdblquote"] = new RtfCharCommand("\xE2\x80\x9D");  // &rdquo;

    ourKeywordMap["jpegblip"] = new RtfPictureCommand("image/jpeg");
    ourKeywordMap["pngblip"] = new RtfPictureCommand("image/png");

    ourKeywordMap["s"] = new RtfStyleCommand();

    ourKeywordMap["qc"] = new RtfAlignmentCommand(ALIGN_CENTER);
    ourKeywordMap["ql"] = new RtfAlignmentCommand(ALIGN_LEFT);
    ourKeywordMap["qr"] = new RtfAlignmentCommand(ALIGN_RIGHT);
    ourKeywordMap["qj"] = new RtfAlignmentCommand(ALIGN_JUSTIFY);
    ourKeywordMap["pard"] = new RtfAlignmentCommand(ALIGN_UNDEFINED);

    ourKeywordMap["b"] = new RtfFontPropertyCommand(FONT_BOLD);
    ourKeywordMap["i"] = new RtfFontPropertyCommand(FONT_ITALIC);
    ourKeywordMap["u"] = new RtfFontPropertyCommand(FONT_UNDERLINED);
    ourKeywordMap["plain"] = new RtfFontResetCommand();
  }
}

void RtfReader::ecApplyPropChange(FontProperty property, bool start) {
  if (state.rds == DESTINATION_SKIP)         // If we're skipping text,
    return;          // don't do anything.

  switch (property) {
    case FONT_BOLD:
      if (state.Bold != start) {
        state.Bold = start;
        setFontProperty(FONT_BOLD, state.Bold);
      }
      break;
    case FONT_ITALIC:
      if (state.Italic != start) {
        state.Italic = start;
        setFontProperty(FONT_ITALIC, state.Italic);
      }
      break;
    case FONT_UNDERLINED:
      if (state.Underlined != start) {
        state.Underlined = start;
        setFontProperty(FONT_UNDERLINED, state.Underlined);
      }
      break;
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
      //DPRINT("title info\n");
      startElementHandler(_TITLE_INFO);
      break;
    case DESTINATION_TITLE:
      //DPRINT("title\n");
      startElementHandler(_BOOK_TITLE);
      break;
    case DESTINATION_AUTHOR:
      //DPRINT("author\n");
      startElementHandler(_AUTHOR);
      break;
    case DESTINATION_PICTURE:
      //DPRINT("picture\n");
      state.ReadDataAsHex = true;
      startElementHandler(_IMAGE);
      break;
    case DESTINATION_FOOTNOTE:
      //DPRINT("footnote\n");
      startElementHandler(_FOOTNOTE);
      break;
    case DESTINATION_STYLESHEET:
      //DPRINT("style sheet\n");
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
      //DPRINT("info end\n");
      endElementHandler(_TITLE_INFO);
      break;
    case DESTINATION_TITLE:
      //DPRINT("title end\n");
      endElementHandler(_BOOK_TITLE);
      break;
    case DESTINATION_AUTHOR:
      //DPRINT("author end\n");
      endElementHandler(_AUTHOR);
      break;
    case DESTINATION_PICTURE:
      //DPRINT("image end\n");
      endElementHandler(_IMAGE);
      break;
    case DESTINATION_FOOTNOTE:
      //DPRINT("footnote end\n");
      endElementHandler(_FOOTNOTE);
      break;
    case DESTINATION_STYLESHEET:
      //DPRINT("style sheet end\n");
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
  if (state.rds == DESTINATION_SKIP)  // if we're skipping, and it's not
    return parserState;            // the \bin keyword, ignore it.
  switch (ipfn) {
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

  while (!myIsInterrupted) {
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
              
              bool oldItalic = state.Italic;
              bool oldBold = state.Bold;
              bool oldUnderlined = state.Underlined;
              state = myStateStack.top();
              myStateStack.pop();
          
              if (state.Italic != oldItalic) {
                setFontProperty(FONT_ITALIC, state.Italic);
              }
              if (state.Bold != oldBold) {
                setFontProperty(FONT_BOLD, state.Bold);
              }
              if (state.Underlined != oldUnderlined) {
                setFontProperty(FONT_UNDERLINED, state.Underlined);
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
            dataStart = ptr + 1;
          }
          break;
        case READ_KEYWORD:
          if (!isalpha(*ptr)) {
            if ((ptr == dataStart) && (keyword.empty())) {
              if (*ptr == '\'') {
								parserState = READ_HEX_SYMBOL;
              } else {
                keyword = *ptr;
                parserState = ecTranslateKeyword(keyword, 0, false);
              }
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
						if ((keyword == "bin") && (param > 0)) {
              myBinaryDataSize = param;
              parserState = READ_BINARY_DATA;
						} else {
              parserState = ecTranslateKeyword(keyword, param, true);
						}
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
  
  return (myIsInterrupted || myStateStack.empty()) ? ecOK : ecUnmatchedBrace;
}

RtfReader::ParserState RtfReader::ecTranslateKeyword(const std::string &keyword, int param, bool fParam) {
  std::map<std::string, RtfCommand*>::const_iterator it = ourKeywordMap.find(keyword);
  
  if (it == ourKeywordMap.end()) {
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
  myIsInterrupted = true;
}

bool RtfReader::readDocument(const std::string &fileName) {
  myFileName = fileName;
  myStream = ZLFile(fileName).inputStream();
  if (myStream.isNull() || !myStream->open()) {
      return false;
  }

  fillKeywordMap();

  myStreamBuffer = new char[rtfStreamBufferSize];
  
  myIsInterrupted = false;
  startDocumentHandler();

  fSkipDestIfUnk = false;

  state.alignment = ALIGN_UNDEFINED;
  state.Italic = false;
  state.Bold = false;
  state.Underlined = false;
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
