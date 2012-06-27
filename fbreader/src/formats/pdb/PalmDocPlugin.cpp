/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include "PdbPlugin.h"
#include "PalmDocStream.h"
#include "MobipocketHtmlBookReader.h"
#include "../txt/PlainTextFormat.h"
#include "../util/TextFormatDetector.h"

bool PalmDocPlugin::acceptsFile(const ZLFile &file) const {
	return PdbPlugin::fileType(file) == "TEXtREAd";
}

void PalmDocPlugin::readDocumentInternal(const ZLFile &file, BookModel &model, const PlainTextFormat &format, const std::string &encoding, ZLInputStream &stream) const {
	stream.open();
	bool readAsPalmDoc = ((PalmDocStream&)stream).hasExtraSections();
	stream.close();
	if (readAsPalmDoc) {
		MobipocketHtmlBookReader(file, model, format, encoding).readDocument(stream);
	} else {
		SimplePdbPlugin::readDocumentInternal(file, model, format, encoding, stream);
	}
}

FormatInfoPage *PalmDocPlugin::createInfoPage(ZLOptionsDialog &dialog, const ZLFile &file) {
	shared_ptr<ZLInputStream> stream = createStream(file);
	stream->open();
	bool readAsPalmDoc = ((PalmDocStream&)*stream).hasExtraSections();
	stream->close();
	if (!readAsPalmDoc) {
		return new PlainTextInfoPage(dialog, file, ZLResourceKey("Text"), !TextFormatDetector().isHtml(*stream));
	} else {
		return 0;
	}
}
