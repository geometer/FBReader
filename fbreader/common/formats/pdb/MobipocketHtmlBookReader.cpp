/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLFile.h>
#include <abstract/ZLFileImage.h>
#include <abstract/ZLStringUtil.h>

#include "MobipocketHtmlBookReader.h"
#include "MobipocketStream.h"

class MobipocketHtmlImageTagAction : public HtmlTagAction {

public:
  MobipocketHtmlImageTagAction(HtmlBookReader &reader);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class MobipocketHtmlHrTagAction : public HtmlTagAction {

public:
  MobipocketHtmlHrTagAction(HtmlBookReader &reader);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

MobipocketHtmlImageTagAction::MobipocketHtmlImageTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlImageTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes) {
  if (start) {
    for (unsigned int i = 0; i < attributes.size(); ++i) {
      if (attributes[i].Name == "RECINDEX") {
        int index = atoi(attributes[i].Value.c_str());
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

void MobipocketHtmlHrTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
  if (start) {
    bookReader().insertEndOfSectionParagraph();
  }
}

MobipocketHtmlBookReader::MobipocketHtmlBookReader(const std::string &fileName, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlBookReader("", model, format, encoding), myImageCounter(0), myFileName(fileName) {
  addAction("IMG",  new MobipocketHtmlImageTagAction(*this));
  addAction("HR",  new MobipocketHtmlHrTagAction(*this));
  //addAction("A",  new MobipocketHtmlHrefTagAction(*this));
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
