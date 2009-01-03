/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#include "PdbPlugin.h"
#include "MobipocketStream.h"
#include "MobipocketHtmlBookReader.h"
#include "../../description/BookDescription.h"

bool MobipocketPlugin::acceptsFile(const ZLFile &file) const {
	const std::string type = PdbPlugin::fileType(file);
	return type == "BOOKMOBI";
}

void MobipocketPlugin::readDocumentInternal(const std::string &fileName, BookModel &model, const PlainTextFormat &format, const std::string &encoding, ZLInputStream &stream) const {
	MobipocketHtmlBookReader(fileName, model, format, encoding).readDocument(stream);
}

const std::string &MobipocketPlugin::iconName() const {
	static const std::string ICON_NAME = "mobipocket";
	return ICON_NAME;
}

bool MobipocketPlugin::readDescription(const std::string &path, BookDescription &description) const {
	shared_ptr<ZLInputStream> stream = ZLFile(path).inputStream();
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
		return PalmDocLikePlugin::readDescription(path, description);
	}

	WritableBookDescription wDescription(description);

	unsigned long length;
	PdbUtil::readUnsignedLong(*stream, length);

	stream->seek(4, false);

	unsigned long encodingCode;
	PdbUtil::readUnsignedLong(*stream, encodingCode);
	if (wDescription.encoding().empty()) {
		ZLEncodingConverterInfoPtr info = ZLEncodingCollection::instance().info(encodingCode);
		if (!info.isNull()) {
			wDescription.encoding() = info->name();
		}
	}

	stream->seek(52, false);

	unsigned long fullNameOffset;
	PdbUtil::readUnsignedLong(*stream, fullNameOffset);
	unsigned long fullNameLength;
	PdbUtil::readUnsignedLong(*stream, fullNameLength);

	unsigned long languageCode;
	PdbUtil::readUnsignedLong(*stream, languageCode);
	wDescription.language() = ZLLanguageUtil::languageByCode(languageCode & 0xFF, (languageCode >> 8) & 0xFF);

	stream->seek(32, false);

	unsigned long exthFlags;
	PdbUtil::readUnsignedLong(*stream, exthFlags);
	if (exthFlags & 0x40) {
		stream->seek(header.Offsets[0] + 16 + length, true);

		stream->read(test, 4);
		static const std::string EXTH = "EXTH";
		if (EXTH == test) {
			stream->seek(4, false);
			unsigned long recordsNum;
			PdbUtil::readUnsignedLong(*stream, recordsNum);
			for (unsigned long i = 0; i < recordsNum; ++i) {
				unsigned long type;
				PdbUtil::readUnsignedLong(*stream, type);
				unsigned long size;
				PdbUtil::readUnsignedLong(*stream, size);
				if (size > 8) {
					std::string value(size - 8, '\0');
					stream->read((char*)value.data(), size - 8);
					switch (type) {
						case 100:
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
							wDescription.addAuthor(value);
							break;
						}
						case 105:
							wDescription.addTag(value);
							break;
					}
				}
			}
		}
	}

	stream->seek(header.Offsets[0] + fullNameOffset, true);
	std::string title(fullNameLength, '\0');
	stream->read((char*)title.data(), fullNameLength);
	wDescription.title() = title;

	stream->close();
	return PalmDocLikePlugin::readDescription(path, description);
}
