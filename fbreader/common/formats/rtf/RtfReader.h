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

typedef enum { risNorm, risBin, risHexSingle, risHexStream/*, risSkip*/} RIS;       // Rtf Internal State
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
    RIS ris;
    
    int style;
};


class ZLInputStream;

class RtfReader {

public:
	bool readDocument(shared_ptr<ZLInputStream> stream) FORMATS_SECTION;

protected:
	RtfReader(const std::string &encoding) FORMATS_SECTION;
	virtual ~RtfReader() FORMATS_SECTION;

protected:
	std::string encoding;
	
	virtual void startDocumentHandler() FORMATS_SECTION = 0;
	virtual void endDocumentHandler() FORMATS_SECTION = 0;

	virtual void startElementHandler(int tag, const char **attributes = NULL) FORMATS_SECTION = 0;
	virtual void endElementHandler(int tag) FORMATS_SECTION = 0;

	virtual bool characterDataHandler(std::string &str) FORMATS_SECTION = 0;
	virtual bool characterPrint(char ch) FORMATS_SECTION = 0;
//	virtual void flushBuffer() FORMATS_SECTION = 0;

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
	int ecParseSpecialProperty(int iprop, int val);
	int ecApplyPictPropChange(int pprop);
	int ecChangeDest(int idest);
	int ecEndGroupAction(int rds);
	int ecStyleChange(int st, int val);

	int ecPushRtfState(void);
	int ecPopRtfState(void);
	
	int ecParseSpecialKeyword(int ipfn, int param);
	int ecParseRtfKeyword(void);
	int ecParseChar(int ch);

	int ecRtfParse();

	int is_interrupted;	
	int cGroup;
	bool fSkipDestIfUnk;
	long cbBin;

	RtfReaderState state;

	std::vector<RtfReaderState> stack;


	int startCounter;
	int endCounter;
	int getChar(void);
	void unGetChar(int ch);

private:
	shared_ptr<ZLInputStream> myStream;
	char *myStreamBuffer;
};

#endif /* __RTFREADER_H__ */
