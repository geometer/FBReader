/*
 * FBReader -- electronic book reader
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

#include <algorithm>

#include <ZLFile.h>
#include <ZLFileImage.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "MobipocketHtmlBookReader.h"
#include "MobipocketStream.h"
#include "../html/HtmlTagActions.h"

class MobipocketHtmlImageTagAction : public HtmlTagAction {

public:
	MobipocketHtmlImageTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlHrTagAction : public HtmlTagAction {

public:
	MobipocketHtmlHrTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlHrefTagAction : public HtmlHrefTagAction {

public:
	MobipocketHtmlHrefTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

MobipocketHtmlImageTagAction::MobipocketHtmlImageTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlImageTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		for (unsigned int i = 0; i < tag.Attributes.size(); ++i) {
			if (tag.Attributes[i].Name == "RECINDEX") {
				int index = atoi(tag.Attributes[i].Value.c_str());
				if (index > 0) {
					int &imageCounter = ((MobipocketHtmlBookReader&)myReader).myImageCounter;
					imageCounter = std::max(imageCounter, index);
					bool stopParagraph = bookReader().paragraphIsOpen();
					if (stopParagraph) {
						bookReader().endParagraph();
					}
					std::string id;
					ZLStringUtil::appendNumber(id, index);
					bookReader().addImageReference(id);
					if (stopParagraph) {
						bookReader().beginParagraph();
					}
				}
				break;
			}
		}
	}
}

MobipocketHtmlHrTagAction::MobipocketHtmlHrTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlHrTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		if (bookReader().contentsParagraphIsOpen()) {
			bookReader().endContentsParagraph();
			bookReader().exitTitle();
		}
		bookReader().insertEndOfSectionParagraph();
	}
}

MobipocketHtmlHrefTagAction::MobipocketHtmlHrefTagAction(HtmlBookReader &reader) : HtmlHrefTagAction(reader) {
}

void MobipocketHtmlHrefTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		for (unsigned int i = 0; i < tag.Attributes.size(); ++i) {
			if (ZLUnicodeUtil::toLower(tag.Attributes[i].Name) == "filepos") {
				const std::string &value = tag.Attributes[i].Value;
				if (!value.empty()) {
					std::string label = "&";
					ZLStringUtil::appendNumber(label, atoi(tag.Attributes[i].Value.c_str()));
					setHyperlinkType(INTERNAL_HYPERLINK);
					bookReader().addHyperlinkControl(INTERNAL_HYPERLINK, label);
					return;
				}
			}
		}
		std::string label = "&";
		ZLStringUtil::appendNumber(label, tag.Offset);
		bookReader().addHyperlinkLabel(label);
	}
	HtmlHrefTagAction::run(tag);
}

shared_ptr<HtmlTagAction> MobipocketHtmlBookReader::createAction(const std::string &tag) {
	if (tag == "IMG") {
		return new MobipocketHtmlImageTagAction(*this);
	} else if (tag == "HR") {
		return new MobipocketHtmlHrTagAction(*this);
	} else if (tag == "A") {
		return new MobipocketHtmlHrefTagAction(*this);
	}
	return HtmlBookReader::createAction(tag);
}

MobipocketHtmlBookReader::MobipocketHtmlBookReader(const std::string &fileName, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlBookReader("", model, format, encoding), myImageCounter(0), myFileName(fileName) {
	setBuildTableOfContent(false);
}

void MobipocketHtmlBookReader::readDocument(ZLInputStream &stream) {
	HtmlBookReader::readDocument(stream);

	shared_ptr<ZLInputStream> fileStream = ZLFile(myFileName).inputStream();
	bool found = false;
	int index = 0;
	if (!fileStream.isNull() && fileStream->open()) {
		char bu[2];
		std::pair<int,int> firstImageLocation = ((MobipocketStream&)stream).imageLocation(1);
		fileStream->seek(firstImageLocation.first, false);
		while ((firstImageLocation.first > 0) && (firstImageLocation.second > 0)) {
			if (firstImageLocation.second > 2) {
				fileStream->read(bu, 2);
				if ((strncmp(bu, "BM", 2) == 0) || (strncmp(bu, "GI", 2) == 0)) {
					found = true;
					break;
				}
				fileStream->seek(firstImageLocation.second - 2, false);
			} else {
				fileStream->seek(firstImageLocation.second, false);
			}
			index++;
			firstImageLocation = ((MobipocketStream&)stream).imageLocation(index + 1);
		}
		fileStream->close();
	}

	if (found) {
		for (int i = 1; i <= myImageCounter; i++) {
			std::pair<int,int> imageLocation = ((MobipocketStream&)stream).imageLocation(i + index);
			if ((imageLocation.first > 0) && (imageLocation.second > 0)) {
				std::string id;
				ZLStringUtil::appendNumber(id, i);
				myBookReader.addImage(id, new ZLFileImage("image/auto", myFileName, imageLocation.first, imageLocation.second));
			}
		}
	}
}
