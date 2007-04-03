/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLFile.h>
#include <ZLApplication.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "../xml/ZLXMLReader.h"

#include "ZLEncodingConverter.h"
#include "EncodingCollectionReader.h"

std::vector<shared_ptr<ZLEncodingSet> > ZLEncodingCollection::ourSets;
std::map<std::string,ZLEncodingConverterInfoPtr> ZLEncodingCollection::ourInfosByName;

std::string ZLEncodingCollection::encodingDescriptionPath() {
	return ZLApplication::ZLibraryDirectory() + ZLApplication::FileNameDelimiter + "encodings";
}

void ZLEncodingCollection::init() {
	if (!ourSets.empty()) {
		return;
	}

	const std::string prefix = encodingDescriptionPath() + ZLApplication::FileNameDelimiter;
	EncodingCollectionReader().readDocument(prefix + "Encodings.xml");
}

const std::vector<shared_ptr<ZLEncodingSet> > &ZLEncodingCollection::sets() {
	init();
	return ourSets;
}

ZLEncodingConverterInfoPtr ZLEncodingCollection::info(const std::string &name) {
	init();
	return ourInfosByName[ZLUnicodeUtil::toLower(name)];
}

ZLEncodingConverterInfoPtr ZLEncodingCollection::defaultInfo() {
	return info("UTF-8");
}

ZLEncodingConverterInfoPtr ZLEncodingCollection::info(int code) {
	std::string name;
	ZLStringUtil::appendNumber(name, code);
	return info(name);
}
