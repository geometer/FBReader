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
#include <ZLImage.h>
#include <ZLStringUtil.h>
#include <ZLDir.h>
#include <ZLInputStream.h>
#include <ZLLogger.h>

#include "OEBPlugin.h"
#include "OEBMetaInfoReader.h"
#include "OEBBookReader.h"
#include "OEBCoverReader.h"
#include "OEBTextStream.h"
#include "../../bookmodel/BookModel.h"
#include "../../library/Book.h"

static const std::string OPF = "opf";
static const std::string OEBZIP = "oebzip";
static const std::string EPUB = "epub";

OEBPlugin::~OEBPlugin() {
}

bool OEBPlugin::providesMetaInfo() const {
	return true;
}

bool OEBPlugin::acceptsFile(const ZLFile &file) const {
	const std::string &extension = file.extension();
	return (extension == OPF) || (extension == OEBZIP) || (extension == EPUB);
}

std::string OEBPlugin::opfFileName(const std::string &oebFileName) {
	ZLFile oebFile = ZLFile(oebFileName);
	if (oebFile.extension() == OPF) {
		return oebFileName;
	}

	ZLLogger::Instance().println("epub", "Looking for opf file in " + oebFileName);
	oebFile.forceArchiveType(ZLFile::ZIP);
	shared_ptr<ZLDir> zipDir = oebFile.directory(false);
	if (zipDir.isNull()) {
		ZLLogger::Instance().println("epub", "Couldn't open zip archive");
		return std::string();
	}
	std::vector<std::string> fileNames;
	zipDir->collectFiles(fileNames, false);
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
		ZLLogger::Instance().println("epub", "Item: " + *it);
		if (ZLStringUtil::stringEndsWith(*it, ".opf")) {
			return zipDir->itemPath(*it);
		}
	}
	ZLLogger::Instance().println("epub", "Opf file not found");
	return std::string();
}

bool OEBPlugin::readMetaInfo(Book &book) const {
	const std::string &path = book.filePath();
	shared_ptr<ZLInputStream> lock = ZLFile(path).inputStream();
	const std::string opf = opfFileName(path);
	bool code = OEBMetaInfoReader(book).readMetaInfo(opf);
	if (code && book.language().empty()) {
		shared_ptr<ZLInputStream> oebStream = new OEBTextStream(opf);
		detectLanguage(book, *oebStream);
	}
	return code;
}

class InputStreamLock : public ZLUserData {

public:
	InputStreamLock(shared_ptr<ZLInputStream> stream);

private:
	shared_ptr<ZLInputStream> myStream;
};

InputStreamLock::InputStreamLock(shared_ptr<ZLInputStream> stream) : myStream(stream) {
}

bool OEBPlugin::readModel(BookModel &model) const {
	const std::string &filePath = model.book()->filePath();
	model.addUserData(
		"inputStreamLock",
		new InputStreamLock(ZLFile(filePath).inputStream())
	);
	return OEBBookReader(model).readBook(opfFileName(filePath));
}

shared_ptr<ZLImage> OEBPlugin::coverImage(const Book &book) const {
	const std::string opf = opfFileName(book.filePath());
	return OEBCoverReader().readCover(opf);
}

const std::string &OEBPlugin::iconName() const {
	static const std::string ICON_NAME = "oeb";
	return ICON_NAME;
}
