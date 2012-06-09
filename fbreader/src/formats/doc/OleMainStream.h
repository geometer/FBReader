/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OLEMAINSTREAM_H__
#define __OLEMAINSTREAM_H__

#include <vector>

#include "OleStream.h"

class OleMainStream : public OleStream {
public:
	struct Piece {
		int offset;
		int length;
		bool isANSI;
	};
	typedef std::vector<Piece> Pieces;

	struct CharInfo {

		enum Font {
			REGULAR = 0x0000,
			BOLD = 0x0001,
			ITALIC = 0x0002,
			UNDERLINE = 0x0004,
			CAPITALS = 0x0008,
			SMALL_CAPITALS = 0x0010,
			STRIKE = 0x0020,
			HIDDEN = 0x0040,
			MARKDEL = 0x0080,
			SUPERSCRIPT = 0x0100,
			SUBSCRIPT = 0x0200
		};

		unsigned int offset;
		unsigned int fontStyle;

		CharInfo();
	};
	typedef std::vector<CharInfo> CharInfoList;

	struct StyleInfo {
		unsigned int offset; // The style start with this character
		bool numPause;
		bool noRestart; // Don't restart by more significant levels
		unsigned int istd; // Current style
		unsigned int istdNext; // Next style unless overruled
		unsigned int startAt;	 // Number at the start of a list
		unsigned int beforeIndent; // Vertical indent before paragraph in twips
		unsigned int afterIndent; // Vertical indent after paragraph in twips
		unsigned int listIndex; // Before Word 8 this field was not filled
		unsigned int listChar; // Character for an itemized list (Unicode)
		int	leftIndent; // Left indentation in twips
		int	leftIndent1; // First line left indentation in twips
		int	rightIndent; // Right indentation in twips
		unsigned int alignment;
		unsigned int numberFormatCode; // Number format code
		unsigned int numLevel;
		unsigned int listLevel; // Before Word 8 this field was not filled
		StyleInfo();
	};
	typedef std::vector<StyleInfo> StyleInfoList;

	enum Alignment {
		LEFT = 0x00,
		CENTER = 0x01,
		RIGHT = 0x02,
		JUSTIFY = 0x03
	};

	enum StyleID {
		STI_USER = 0xFFE,
		STI_NIL = 0xFFF,
		ISTD_INVALID = 0xFFFF
	};

	enum PrlFlag {
		UNSET = 0,
		SET = 1,
		UNCHANGED = 128,
		NEGATION = 129
	};

	struct SectionInfo {
		unsigned int charPos;
		bool newPage;
		SectionInfo();
	};

	typedef std::vector<SectionInfo> SectionInfoList;

public:
	OleMainStream(shared_ptr<OleStorage> storage, OleEntry oleEntry, shared_ptr<ZLInputStream> stream);

public:
	bool open();
	const Pieces& getPieces() const;
	const CharInfoList& getCharInfoList() const;
	const StyleInfoList& getStyleInfoList() const;
	const SectionInfoList& getSectionInfoList() const;

private:
	bool readFIB(const char* headerBuffer);
	bool readPieceTable(const char* headerBuffer, const OleEntry& tableEntry);
	bool readStylesheet(const char* headerBuffer, const OleEntry& tableEntry);
	bool readSectionsInfoTable(const char* headerBuffer, const OleEntry& tableEntry);
	bool readParagraphStyleTable(const char* headerBuffer, const OleEntry& tableEntry);
	bool readCharInfoTable(const char* headerBuffer, const OleEntry& tableEntry);

private: //readPieceTable helpers methods
	static std::string getPiecesTableBuffer(const char* headerBuffer, OleStream& tableStream);
	static int getLastCP(const char* buffer);

private: //formatting reader helpers methods
	static unsigned int getInfoLength(const char *grpprlBuffer, unsigned int byteNumber);
	static void getCharInfo(unsigned int chpxOffset, unsigned int istd, const char *grpprlBuffer, unsigned int iBytes, CharInfo& charInfo);
	static void getStyleInfo(unsigned int papxOffset, const char *grpprlBuffer, unsigned int bytes, StyleInfo& styleInfo);
	static void getSectionInfo(const char* grpprlBuffer, size_t bytes, SectionInfo& sectionInfo);
	static StyleInfo getStyleInfoFromStylesheet(unsigned int istd, const OleMainStream::StyleInfoList &styleInfoList);
	static CharInfo getCharInfoFromStylesheet(unsigned int istd, const OleMainStream::StyleInfoList &styleInfoList, const OleMainStream::CharInfoList &charInfoList);
	static int getStyleIndex(unsigned int istd, const std::vector<bool>& isFilled, const StyleInfoList& styleInfoList);
	static unsigned int getIstdByOffset(unsigned int offset, const StyleInfoList& styleInfoList);

private:
	int myStartOfText;
	int myEndOfText;

	Pieces myPieces;

	CharInfoList myStyleSheetCharInfo;
	StyleInfoList myStyleSheetStyleInfo;

	CharInfoList myCharInfoList;
	StyleInfoList myStyleInfoList;
	SectionInfoList mySectionInfoList;
};

#endif /* __OLEMAINSTREAM_H__ */
