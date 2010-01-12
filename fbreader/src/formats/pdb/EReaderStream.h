/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __EREADERSTREAM_H__
#define __EREADERSTREAM_H__

#include <map>

#include "PalmDocLikeStream.h"

class ZLFile;

class EReaderStream : public PalmDocLikeStream {

public:
	EReaderStream(ZLFile &file);
	~EReaderStream();

	enum StreamDestination {
		TEXT,
		FOOTNOTE,
	};
	
	struct ImageInfo {
		unsigned long Offset;
		unsigned short Size;
		std::string Type;
	};

	ImageInfo imageLocation(const std::string& id);
	//bool hasExtraSections() const;
	bool switchStreamDestination(StreamDestination destination, const std::string &footnoteId);
	const std::map<std::string, unsigned short>& footnotes() const;
	const std::map<std::string, ImageInfo>& images() const;

private:
	bool processRecord();
	bool processZeroRecord();
	bool processFootnoteIdsRecord();
	bool processImageHeaders();

	void clearBuffer(unsigned char symbol);
	std::string findFootnoteId(std::string &footnoteIdStr) const;
	bool addImageInfo(const unsigned short recordIndex);
	
	bool fillBuffer();

private:
	unsigned short myCompressionVersion;
	unsigned short myNonTextOffset;
	unsigned short myNonTextOffsetReserved; 		//TODO: Warning: isn't used
	unsigned short myFootnoteRecords;
	unsigned short mySidebarRecords;
	unsigned short myBookmarksOffset;
	unsigned short myNonTextOffsetExtraReserved; 	//TODO: Warning: isn't used
	unsigned short myImagedataOffset;
	unsigned short myImagedataOffsetReserved;		//TODO: Warning: isn't used
	unsigned short myMetadataOffset;
	unsigned short myMetadataOffsetReserved;		//TODO: Warning: isn't used
	unsigned short myFootnoteOffset;
	unsigned short mySidebarOffset;
	unsigned short myLastdataOffset;

	
	StreamDestination myDestination;
	std::map<std::string, unsigned short> myFootnotes;
	std::map<std::string, ImageInfo> myImages;

};

#endif /* __EREADERSTREAM_H__ */
