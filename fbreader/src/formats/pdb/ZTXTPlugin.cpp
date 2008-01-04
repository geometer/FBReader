/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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
#include "ZTXTStream.h"
#include "../txt/PlainTextFormat.h"
#include "../util/TextFormatDetector.h"

bool ZTXTPlugin::providesMetaInfo() const {
	return false;
}

bool ZTXTPlugin::acceptsFile(const ZLFile &file) const {
	return PdbPlugin::fileType(file) == "zTXTGPlm";
}

shared_ptr<ZLInputStream> ZTXTPlugin::createStream(ZLFile &file) const {
	return new ZTXTStream(file);
}

const std::string &ZTXTPlugin::iconName() const {
	static const std::string ICON_NAME = "weasel";
	return ICON_NAME;
}

FormatInfoPage *ZTXTPlugin::createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) {
	ZLFile file(fileName);
	shared_ptr<ZLInputStream> stream = createStream(file);
	return new PlainTextInfoPage(dialog, fileName, ZLResourceKey("Text"), !TextFormatDetector().isHtml(*stream));
}
