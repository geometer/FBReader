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
#include <ZLImage.h>
#include <ZLFileImage.h>

#include "PdbPlugin.h"
#include "PalmDocStream.h"
#include "MobipocketHtmlBookReader.h"

#include "../../library/Book.h"

bool MobipocketPlugin::acceptsFile(const ZLFile &file) const {
	return PdbPlugin::fileType(file) == "BOOKMOBI";
}

void MobipocketPlugin::readDocumentInternal(const std::string &fileName, BookModel &model, const PlainTextFormat &format, const std::string &encoding, ZLInputStream &stream) const {
	MobipocketHtmlBookReader(fileName, model, format, encoding).readDocument(stream);
}

const std::string &MobipocketPlugin::iconName() const {
	static const std::string ICON_NAME = "mobipocket";
	return ICON_NAME;
}

bool MobipocketPlugin::readMetaInfo(Book &book) const {
	shared_ptr<ZLInputStream> stream = ZLFile(book.filePath()).inputStream();
	if (stream.isNull() || ! stream->open()) {
		return false;
	}
	PdbHeader header;
	if (!header.read(stream)) {
		return false;
	}
	stream->seek(header.Offsets[0] + 16, true);
	char test[5];
	test[4] = '\0';
	stream->read(test, 4);
	static const std::string MOBI = "MOBI";
	if (MOBI != test) {
		return PalmDocLikePlugin::readMetaInfo(book);
	}

	unsigned long length;
	PdbUtil::readUnsignedLongBE(*stream, length);

	stream->seek(4, false);

	unsigned long encodingCode;
	PdbUtil::readUnsignedLongBE(*stream, encodingCode);
	if (book.encoding().empty()) {
		ZLEncodingConverterInfoPtr info = ZLEncodingCollection::Instance().info(encodingCode);
		if (!info.isNull()) {
			book.setEncoding(info->name());
		}
	}

	stream->seek(52, false);

	unsigned long fullNameOffset;
	PdbUtil::readUnsignedLongBE(*stream, fullNameOffset);
	unsigned long fullNameLength;
	PdbUtil::readUnsignedLongBE(*stream, fullNameLength);

	unsigned long languageCode;
	PdbUtil::readUnsignedLongBE(*stream, languageCode);
	book.setLanguage(ZLLanguageUtil::languageByCode(languageCode & 0xFF, (languageCode >> 8) & 0xFF));

	stream->seek(32, false);

	unsigned long exthFlags;
	PdbUtil::readUnsignedLongBE(*stream, exthFlags);
	if (exthFlags & 0x40) {
		stream->seek(header.Offsets[0] + 16 + length, true);

		stream->read(test, 4);
		static const std::string EXTH = "EXTH";
		if (EXTH == test) {
			stream->seek(4, false);
			unsigned long recordsNum;
			PdbUtil::readUnsignedLongBE(*stream, recordsNum);
			for (unsigned long i = 0; i < recordsNum; ++i) {
				unsigned long type;
				PdbUtil::readUnsignedLongBE(*stream, type);
				unsigned long size;
				PdbUtil::readUnsignedLongBE(*stream, size);
				if (size > 8) {
					std::string value(size - 8, '\0');
					stream->read((char*)value.data(), size - 8);
					switch (type) {
						case 100: // author
						{
							int index = value.find(',');
							if (index != -1) {
								std::string part0 = value.substr(0, index);
								std::string part1 = value.substr(index + 1);
								ZLStringUtil::stripWhiteSpaces(part0);
								ZLStringUtil::stripWhiteSpaces(part1);
								value = part1 + ' ' + part0;
							} else {
								ZLStringUtil::stripWhiteSpaces(value);
							}
							book.addAuthor(value);
							break;
						}
						case 105: // subject
							book.addTag(value);
							break;
					}
				}
			}
		}
	}

	stream->seek(header.Offsets[0] + fullNameOffset, true);
	std::string title(fullNameLength, '\0');
	stream->read((char*)title.data(), fullNameLength);
	book.setTitle(title);

	stream->close();
	return PalmDocLikePlugin::readMetaInfo(book);
}

shared_ptr<ZLImage> MobipocketPlugin::coverImage(const Book &book) const {
	shared_ptr<ZLInputStream> stream = ZLFile(book.filePath()).inputStream();
	if (stream.isNull() || ! stream->open()) {
		return 0;
	}
	PdbHeader header;
	if (!header.read(stream)) {
		return 0;
	}
	stream->seek(header.Offsets[0] + 16, true);
	char test[5];
	test[4] = '\0';
	stream->read(test, 4);
	static const std::string MOBI = "MOBI";
	if (MOBI != test) {
		return 0;
	}

	unsigned long length;
	PdbUtil::readUnsignedLongBE(*stream, length);

	stream->seek(104, false);

	unsigned long exthFlags;
	unsigned long coverIndex = (unsigned long)-1;
	unsigned long thumbIndex = (unsigned long)-1;
	PdbUtil::readUnsignedLongBE(*stream, exthFlags);
	if (exthFlags & 0x40) {
		stream->seek(header.Offsets[0] + 16 + length, true);

		stream->read(test, 4);
		static const std::string EXTH = "EXTH";
		if (EXTH != test) {
			return 0;
		}
		stream->seek(4, false);
		unsigned long recordsNum;
		PdbUtil::readUnsignedLongBE(*stream, recordsNum);
		for (unsigned long i = 0; i < recordsNum; ++i) {
			unsigned long type;
			PdbUtil::readUnsignedLongBE(*stream, type);
			unsigned long size;
			PdbUtil::readUnsignedLongBE(*stream, size);
			switch (type) {
				case 201: // coveroffset
					if (size == 12) {
						PdbUtil::readUnsignedLongBE(*stream, coverIndex);
					} else {
						stream->seek(size - 8, false);
					}
					break;
				case 202: // thumboffset
					if (size == 12) {
						PdbUtil::readUnsignedLongBE(*stream, thumbIndex);
					} else {
						stream->seek(size - 8, false);
					}
					break;
				default:
					stream->seek(size - 8, false);
					break;
			}
		}
	}
	stream->close();

	if (coverIndex == (unsigned long)-1) {
		if (thumbIndex == (unsigned long)-1) {
			return 0;
		}
		coverIndex = thumbIndex;
	}

	ZLFile file(book.filePath());
	PalmDocStream pbStream(file);
	if (!pbStream.open()) {
		return 0;
	}
	int index = pbStream.firstImageLocationIndex(book.filePath());
	if (index >= 0) {
		std::pair<int,int> imageLocation = pbStream.imageLocation(pbStream.header(), index + coverIndex);
		if ((imageLocation.first > 0) && (imageLocation.second > 0)) {
			return new ZLFileImage(
				"image/auto",
				book.filePath(),
				imageLocation.first,
				imageLocation.second
			);
		}
	}
	return 0;
}
