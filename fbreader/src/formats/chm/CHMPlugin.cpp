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

#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>
#include <ZLFile.h>
#include <ZLInputStream.h>

#include "CHMPlugin.h"
#include "CHMFile.h"
#include "CHMFileImage.h"
#include "CHMReferenceCollection.h"
#include "HHCReader.h"
#include "HHCReferenceCollector.h"
#include "../txt/PlainTextFormat.h"
#include "HtmlSectionReader.h"
#include "../util/MergedStream.h"
#include "../html/HtmlReaderStream.h"

#include "../../bookmodel/BookModel.h"
#include "../../library/Book.h"

bool CHMPlugin::acceptsFile(const ZLFile &file) const {
	return file.extension() == "chm";
}

class CHMTextStream : public MergedStream {

public:
	CHMTextStream(CHMFileInfo &chmFile, shared_ptr<ZLInputStream> base);

private:
	void resetToStart();
	shared_ptr<ZLInputStream> nextStream();

private:
	CHMFileInfo &myCHMFile;
	shared_ptr<ZLInputStream> myBase;
	std::vector<std::string> myEntryNames;
	size_t myIndex;
};

CHMTextStream::CHMTextStream(CHMFileInfo &chmFile, shared_ptr<ZLInputStream> base) : myCHMFile(chmFile), myBase(base) {
}

void CHMTextStream::resetToStart() {
	myIndex = 0;

	if (!myEntryNames.empty()) {
		return;
	}

	CHMFileInfo::FileNames names = myCHMFile.sectionNames(myBase);
	if (names.empty()) {
		return;
	}

	CHMReferenceCollection referenceCollection;

	referenceCollection.addReference(names.Start, false);
	referenceCollection.addReference(names.Home, false);

	shared_ptr<ZLInputStream> tocStream = myCHMFile.entryStream(myBase, names.TOC);
	if (!tocStream.isNull() && tocStream->open()) {
		referenceCollection.setPrefix(names.TOC);
		HHCReferenceCollector(referenceCollection).readDocument(*tocStream);
	}

	while (referenceCollection.containsNonProcessedReferences()) {
		myEntryNames.push_back(referenceCollection.nextReference());
	}
}

shared_ptr<ZLInputStream> CHMTextStream::nextStream() {
	while (myIndex < myEntryNames.size()) {
		shared_ptr<ZLInputStream> stream = myCHMFile.entryStream(myBase, myEntryNames[myIndex++]);
		if (!stream.isNull()) {
			return new HtmlReaderStream(stream, 50000);
		}
	}
	return 0;
}

bool CHMPlugin::readMetaInfo(Book &book) const {
	ZLFile file(book.filePath());
	shared_ptr<ZLInputStream> stream = file.inputStream();
	if (stream.isNull() || !stream->open()) {
		return false;
	}

	CHMFileInfo chmFile(book.filePath());
	if (!chmFile.init(*stream)) {
		return false;
	}

	CHMFileInfo::FileNames names = chmFile.sectionNames(stream);
	if (names.empty()) {
		return false;
	}

	/*
	shared_ptr<ZLInputStream> entryStream = chmFile.entryStream(stream, names.Start);
	if (entryStream.isNull()) {
		entryStream = chmFile.entryStream(stream, names.Home);
	}
	if (entryStream.isNull()) {
		entryStream = chmFile.entryStream(stream, names.TOC);
	}
	/ *
	if (entryStream.isNull()) {
		chmFile.entryStream(stream, names.Index);
	}
	* /
	if (entryStream.isNull()) {
		return false;
	}
	*/

	CHMTextStream textStream(chmFile, stream);
	detectEncodingAndLanguage(book, textStream);
	if (book.encoding().empty()) {
		return false;
	}

	return true;
}

class CHMHyperlinkMatcher : public BookModel::HyperlinkMatcher {

public:
	BookModel::Label match(const std::map<std::string,BookModel::Label> &lMap, const std::string &id) const;
};

BookModel::Label CHMHyperlinkMatcher::match(const std::map<std::string,BookModel::Label> &lMap, const std::string &id) const {
	std::map<std::string,BookModel::Label>::const_iterator it = lMap.find(id);
	if (it != lMap.end()) {
		return it->second;
	}
	size_t index = id.find('#');
	if (index != std::string::npos) {
		it = lMap.find(id.substr(0, index));
	}
	return (it != lMap.end()) ? it->second : BookModel::Label(0, -1);
}

bool CHMPlugin::readModel(BookModel &model) const {
	model.setHyperlinkMatcher(new CHMHyperlinkMatcher());

	const Book &book = *model.book();
	const std::string &filePath = book.filePath();

	shared_ptr<ZLInputStream> stream = ZLFile(filePath).inputStream();
	if (stream.isNull() || !stream->open()) {
		return false;
	}

	shared_ptr<CHMFileInfo> info = new CHMFileInfo(filePath);
	if (!info->init(*stream)) {
		return false;
	}

	CHMFileInfo::FileNames names = info->sectionNames(stream);
	if (names.empty()) {
		return false;
	}

	CHMReferenceCollection referenceCollection;

	referenceCollection.addReference(names.Start, false);
	referenceCollection.addReference(names.Home, false);
	
	const std::string &encoding = book.encoding();

	shared_ptr<ZLInputStream> tocStream = info->entryStream(stream, names.TOC);
	HHCReader hhcReader(referenceCollection, model, encoding);
	if (!tocStream.isNull() && tocStream->open()) {
		referenceCollection.setPrefix(names.TOC);
		hhcReader.readDocument(*tocStream);
	}

	/*
	if (!tocStream.isNull() && tocStream->open()) {
		std::string buf;
		buf.append(tocStream->sizeOfOpened(), '\0');
		tocStream->read((char*)buf.data(), buf.length());
		std::cerr << "[ " << names.TOC << " ]\n" << buf << "\n";
	}
	*/

	int contentCounter = 0;
	PlainTextFormat format(filePath);
	HtmlSectionReader reader(model, format, encoding, info, referenceCollection);
	while (referenceCollection.containsNonProcessedReferences()) {
		const std::string fileName = referenceCollection.nextReference();
		if (ZLStringUtil::stringEndsWith(fileName, ".jpg") ||
		    ZLStringUtil::stringEndsWith(fileName, ".gif")) {
			std::string lowerCasedFileName = ZLUnicodeUtil::toLower(fileName);
			BookReader bookReader(model);
			bookReader.setMainTextModel();
			bookReader.addHyperlinkLabel(lowerCasedFileName);
			bookReader.pushKind(REGULAR);
			bookReader.beginParagraph();
			bookReader.addImageReference(lowerCasedFileName);
			bookReader.addImage(fileName, new CHMFileImage(info, fileName));
			bookReader.endParagraph();
			bookReader.insertEndOfTextParagraph();
		} else {
			shared_ptr<ZLInputStream> entryStream = info->entryStream(stream, fileName);
			if (!entryStream.isNull() && entryStream->open()) {
				/*
				std::string buf;
				buf.append(entryStream->sizeOfOpened(), '\0');
				entryStream->read((char*)buf.data(), buf.length());
				std::cerr << "[ " << fileName << " ]\n" << buf << "\n";
				entryStream->open();
				*/
				reader.setSectionName(fileName);
				reader.readDocument(*entryStream);
				++contentCounter;
			}
		}
	}
	if (contentCounter == 0) {
		return false;
	}

	hhcReader.setReferences();


	return true;
}

const std::string &CHMPlugin::iconName() const {
	static const std::string ICON_NAME = "html";
	return ICON_NAME;
}
