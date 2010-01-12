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

#include <ZLFile.h>
#include <ZLInputStream.h>
#include <ZLEncodingConverter.h>
#include <ZLStringUtil.h>
#include <ZLLanguageUtil.h>
#include <ZLFileImage.h>

#include "PdbPlugin.h"
#include "EReaderStream.h"
#include "PmlBookReader.h"

#include "../../library/Book.h"

bool EReaderPlugin::providesMetaInfo() const {
	return true;
}

bool EReaderPlugin::acceptsFile(const ZLFile &file) const {
	return PdbPlugin::fileType(file) == "PNRdPPrs";
}

void EReaderPlugin::readDocumentInternal(const std::string& fileName, BookModel &model, const PlainTextFormat &format, const std::string &encoding, ZLInputStream &stream) const {
	if (!stream.open())	{
		//TODO maybe anything else opens stream
		return;
	}
	BookReader bookReader(model);
	PmlBookReader pmlBookReader(bookReader, format, encoding);
	bookReader.setMainTextModel();
	pmlBookReader.readDocument(stream);
	EReaderStream &estream = (EReaderStream&)stream;
	const std::map<std::string, EReaderStream::ImageInfo>& imageIds = estream.images();
	for(std::map<std::string, EReaderStream::ImageInfo>::const_iterator it = imageIds.begin(); it != imageIds.end(); ++it) {
		const std::string id = it->first;
		bookReader.addImage(id, new ZLFileImage(it->second.Type, fileName, it->second.Offset, it->second.Size));
	}
	const std::map<std::string, unsigned short>& footnoteIds = estream.footnotes();
	for(std::map<std::string, unsigned short>::const_iterator it = footnoteIds.begin(); it != footnoteIds.end(); ++it) {
		const std::string id = it->first;
		if (estream.switchStreamDestination(EReaderStream::FOOTNOTE, id)) {
			bookReader.setFootnoteTextModel(id);
			bookReader.addHyperlinkLabel(id);
			pmlBookReader.readDocument(estream);
		}
	}
	stream.close();
}

const std::string &EReaderPlugin::iconName() const {
	static const std::string ICON_NAME = "unknown";
	return ICON_NAME;
}

shared_ptr<ZLInputStream> EReaderPlugin::createStream(ZLFile &file) const {
	return new EReaderStream(file);
}

const std::string &EReaderPlugin::tryOpen(const std::string &path) const {
	ZLFile file(path);
	EReaderStream stream(file);
	stream.open();
	return stream.error();
}

bool EReaderPlugin::readMetaInfo(Book &book) const {
	shared_ptr<ZLInputStream> stream = ZLFile(book.filePath()).inputStream();
	if (stream.isNull() || ! stream->open()) {
		return false;
	}
	PdbHeader header;
	if (!header.read(stream)) {
		return false;
	}
	stream->seek(header.Offsets[0] + 46, true);
	unsigned short metaInfoOffset;
	PdbUtil::readUnsignedShort(*stream, metaInfoOffset);
	if (metaInfoOffset == 0 || metaInfoOffset >= header.Offsets.size()) {
		return false;
	}
	size_t currentOffset = header.Offsets[metaInfoOffset];
	size_t nextOffset =
		(metaInfoOffset + 1 < (unsigned short)header.Offsets.size()) ?
			header.Offsets[metaInfoOffset + 1] : stream->sizeOfOpened();
	if (nextOffset <= currentOffset) {
		return false;
	}
	size_t length = nextOffset - currentOffset;

	char* metaInfoBuffer = new char[length];
	stream->seek(currentOffset, true);
	stream->read(metaInfoBuffer, length);
	std::string metaInfoStr(metaInfoBuffer, length);
	delete[] metaInfoBuffer;

	std::string metaInfoData[5]; // Title; Author; Rights; Publisher; isbn;
	for (size_t i = 0; i < 5; ++i) { 
		const size_t index = metaInfoStr.find('\0');
		metaInfoData[i] = metaInfoStr.substr(0,index);
		metaInfoStr = metaInfoStr.substr(index + 1);
	}
	
	if (!metaInfoData[0].empty()) {
		book.setTitle(metaInfoData[0]);
	}
	
	if (!metaInfoData[1].empty()) {
		book.addAuthor(metaInfoData[1]);
	}

	stream->close();
	return SimplePdbPlugin::readMetaInfo(book);
}
