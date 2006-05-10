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

#ifndef __RTFREADER_H__
#define __RTFREADER_H__

#include <string>
#include <map>
#include <stack>

#include <abstract/ZLFSManager.h>
#include <abstract/ZLEncodingConverter.h>
#include <stdio.h>
//#define DPRINT(x...) {FILE *flog; flog=fopen("/tmp/reader_log", "a+"); fprintf(flog, ## x); fclose(flog);}
//#define DPRINT(x...) {fprintf(stderr, ## x);}
#define DPRINT(x...)

typedef struct char_prop
{
    bool fBold;
    bool fUnderline;
    bool fItalic;
} CHP;                  // CHaracter Properties

typedef enum {justL, justR, justC, justF } JUST;
typedef struct para_prop
{
    int xaLeft;                 // left indent in twips
    int xaRight;                // right indent in twips
    int xaFirst;                // first line indent in twips
    int just;                  // justification
} PAP;                  // PAragraph Properties

typedef enum {sbkNon, sbkCol, sbkEvn, sbkOdd, sbkPg} SBK;
typedef enum {pgDec, pgURom, pgLRom, pgULtr, pgLLtr} PGN;
typedef struct sect_prop
{
    int cCols;                  // number of columns
    int sbk;                    // section break type
    int xaPgn;                  // x position of page number in twips
    int yaPgn;                  // y position of page number in twips
    int pgnFormat;              // how the page number is formatted
} SEP;                  // SEction Properties

typedef struct doc_prop
{
    int xaPage;                 // page width in twips
    int yaPage;                 // page height in twips
    int xaLeft;                 // left margin in twips
    int yaTop;                  // top margin in twips
    int xaRight;                // right margin in twips
    int yaBottom;               // bottom margin in twips
    int pgnStart;               // starting page number in twips
    bool fFacingp;              // facing pages enabled?
    bool fLandscape;            // landscape or portrait??
} DOP;                  // DOcument Properties

typedef enum { rdsTitleInfo, rdsTitle, rdsAuthor, rdsContent, rdsImage, 
	       rdsStyleSheet, rdsFootnote, rdsSkip } RDS;              // Rtf Destination State

// property save structure
struct RtfReaderState
{
//    struct save *pNext;         // next save
    CHP chp;
    PAP pap;
    SEP sep;
    DOP dop;
    RDS rds;

		bool ReadDataAsHex;
    
    int style;
};


class ZLInputStream;

class RtfReader {

public:
	bool readDocument(const std::string &fileName);

protected:
	RtfReader(const std::string &encoding);
	virtual ~RtfReader();

protected:
	std::string encoding;
	
	virtual void startDocumentHandler() = 0;
	virtual void endDocumentHandler() = 0;

	virtual void startElementHandler(int tag, const char **attributes = NULL) = 0;
	virtual void endElementHandler(int tag) = 0;

	virtual bool characterDataHandler(std::string &str) = 0;

	virtual void addChar(const char ch) = 0;
	virtual void addCharData(const char *data, size_t len) = 0;

	virtual void insertImage(const std::string &fileName, size_t startOffset, size_t size) = 0;

	void interrupt(void);
	shared_ptr<ZLEncodingConverter> myConverter;

	enum TagCode {
		_P,
		_P_RESET,
		
		_ITALIC,
		_BOLD,
		_UNDERLINE,
		
		_FOOTNOTE,
		
		_STYLE_SHEET,
		_STYLE_INFO,
		_STYLE_SET,
//		_SUBTITLE,
//		_CITE,
//		_TEXT_AUTHOR,
//		_DATE,
//		_SECTION,
//		_V,
//		_TITLE,
//		_POEM,

//		_EPIGRAPH,
//		_ANNOTATION,
//		_SUB,
//		_SUP,
//		_CODE,
//		_STRIKETHROUGH,


		_IMAGE,
		_IMAGE_TYPE,
		_BINARY,
		
		_BODY,
//		_EMPTY_LINE,
		_TITLE_INFO,
		_AUTHOR,
		_BOOK_TITLE,
		_ENCODING,
		_UNKNOWN
	};

private:
    
  int ecApplyPropChange(int iprop, int val);
	int ecParseSpecialProperty(int iprop);
	int ecApplyPictPropChange(int pprop);
	void ecChangeDest(int idest);
	int ecEndGroupAction(int rds);
	int ecStyleChange(int st, int val);

	int ecParseSpecialKeyword(int ipfn, int param);
	int ecTranslateKeyword(const std::string &keyword, int param, bool fParam);
	void ecParseChar(char ch);
	void ecParseCharData(const char *data, size_t len);

	int ecRtfParse();

	int is_interrupted;	
	bool fSkipDestIfUnk;

	RtfReaderState state;

  enum ParserState {
    READ_NORMAL_DATA,
    READ_BINARY_DATA,
    READ_HEX_SYMBOL,
    READ_KEYWORD,
    READ_KEYWORD_PARAMETER,
  };

private:
	std::string myFileName;
	shared_ptr<ZLInputStream> myStream;
	char *myStreamBuffer;

	std::stack<RtfReaderState> myStateStack;

	ParserState myParserState;
	int myBinaryDataSize;
};

#endif /* __RTFREADER_H__ */
