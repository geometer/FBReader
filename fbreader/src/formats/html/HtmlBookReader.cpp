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

#include <cctype>

#include <ZLFileImage.h>

#include "../txt/PlainTextFormat.h"
#include "HtmlBookReader.h"
#include "../../bookmodel/BookModel.h"

class HtmlControlTagAction : public HtmlTagAction {

public:
	HtmlControlTagAction(HtmlBookReader &reader, TextKind kind);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);

private:
	TextKind myKind;
};

class HtmlHeaderTagAction : public HtmlTagAction {

public:
	HtmlHeaderTagAction(HtmlBookReader &reader, TextKind kind);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);

private:
	TextKind myKind;
};

class HtmlIgnoreTagAction : public HtmlTagAction {

public:
	HtmlIgnoreTagAction(HtmlBookReader &reader);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlHrefTagAction : public HtmlTagAction {

public:
	HtmlHrefTagAction(HtmlBookReader &reader);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlImageTagAction : public HtmlTagAction {

public:
	HtmlImageTagAction(HtmlBookReader &reader);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlBreakTagAction : public HtmlTagAction {

public:
	enum BreakType {
		BREAK_AT_START = 1,
		BREAK_AT_END = 2,
		BREAK_AT_START_AND_AT_END = BREAK_AT_START | BREAK_AT_END
	};
	HtmlBreakTagAction(HtmlBookReader &reader, BreakType breakType);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);

private:
	BreakType myBreakType;
};

class HtmlPreTagAction : public HtmlTagAction {

public:
	HtmlPreTagAction(HtmlBookReader &reader);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlListTagAction : public HtmlTagAction {

public:
	HtmlListTagAction(HtmlBookReader &reader);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlListItemTagAction : public HtmlTagAction {

public:
	HtmlListItemTagAction(HtmlBookReader &reader);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

HtmlTagAction::HtmlTagAction(HtmlBookReader &reader) : myReader(reader) {
}

HtmlTagAction::~HtmlTagAction() {
}

HtmlControlTagAction::HtmlControlTagAction(HtmlBookReader &reader, TextKind kind) : HtmlTagAction(reader), myKind(kind) {
}

void HtmlControlTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
	std::vector<TextKind> &list = myReader.myKindList;
	int index;
	for (index = list.size() - 1; index >= 0; --index) {
		if (list[index] == myKind) {
			break;
		}
	}
	if (start) {
		if (index == -1) {
			bookReader().pushKind(myKind);
			myReader.myKindList.push_back(myKind);
			bookReader().addControl(myKind, true);
		}
	} else {
		if (index >= 0) {
			for (int i = list.size() - 1; i >= index; --i) {
				bookReader().addControl(list[i], false);
				bookReader().popKind();
			}
			for (unsigned int j = index + 1; j < list.size(); ++j) {
				bookReader().addControl(list[j], true);
				bookReader().pushKind(list[j]);
			}
			list.erase(list.begin() + index);
		}
	}
}

HtmlHeaderTagAction::HtmlHeaderTagAction(HtmlBookReader &reader, TextKind kind) : HtmlTagAction(reader), myKind(kind) {
}

void HtmlHeaderTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
	bookReader().endParagraph();
	if (start) {
		if (myReader.myBuildTableOfContent) {
			bookReader().insertEndOfSectionParagraph();
			bookReader().enterTitle();
			bookReader().beginContentsParagraph();
		}
		bookReader().pushKind(myKind);
	} else {
		bookReader().popKind();
		if (myReader.myBuildTableOfContent) {
			bookReader().endContentsParagraph();
			bookReader().exitTitle();
		}
	}
	bookReader().beginParagraph();
}

HtmlIgnoreTagAction::HtmlIgnoreTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlIgnoreTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
	if (start) {
		++myReader.myIgnoreDataCounter;
	} else {
		--myReader.myIgnoreDataCounter;
	}
}

HtmlHrefTagAction::HtmlHrefTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlHrefTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes) {
	if (start) {
		for (unsigned int i = 0; i < attributes.size(); ++i) {
			if (attributes[i].Name == "NAME") {
				bookReader().addHyperlinkLabel(attributes[i].Value);
			} else if (!myReader.myIsHyperlink && (attributes[i].Name == "HREF")) {
				const std::string &value = attributes[i].Value;
				if (!value.empty() && (value[0] == '#')) {
					bookReader().addHyperlinkControl(HYPERLINK, value.substr(1));
					myReader.myIsHyperlink = true;
				}
			}
		}
	} else if (myReader.myIsHyperlink) {
		bookReader().addControl(HYPERLINK, false);
		myReader.myIsHyperlink = false;
	}
}

HtmlImageTagAction::HtmlImageTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlImageTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes) {
	if (start) {
		bookReader().endParagraph();
		for (unsigned int i = 0; i < attributes.size(); ++i) {
			if (attributes[i].Name == "SRC") {
				std::string fileName = HtmlReader::decodeURL(attributes[i].Value);
				bookReader().addImageReference(fileName);
				bookReader().addImage(fileName,
					new ZLFileImage("image/auto", myReader.myBaseDirPath + fileName, 0)
				);
				break;
			}
		}
		bookReader().beginParagraph();
	}
}

HtmlBreakTagAction::HtmlBreakTagAction(HtmlBookReader &reader, BreakType breakType) : HtmlTagAction(reader), myBreakType(breakType) {
}

void HtmlBreakTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
	if ((start && (myBreakType & BREAK_AT_START)) ||
			(!start && (myBreakType & BREAK_AT_END))) {
		bookReader().endParagraph();
		bookReader().beginParagraph();
	}
}

HtmlPreTagAction::HtmlPreTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlPreTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
	bookReader().endParagraph();
	myReader.myIsPreformatted = start;
	myReader.mySpaceCounter = -1;
	myReader.myBreakCounter = 0;
	if (myReader.myFormat.breakType() == PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE) {
		if (start) {
			bookReader().pushKind(PREFORMATTED);
		} else {
			bookReader().popKind();
		}
	}
	bookReader().beginParagraph();
}

HtmlListTagAction::HtmlListTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlListTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
	if (start) {
		myReader.myListNumStack.push(0/*(tag.Code == _UL) ? 0 : 1*/);
	} else if (!myReader.myListNumStack.empty()) {
		myReader.myListNumStack.pop();
	}
}

HtmlListItemTagAction::HtmlListItemTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlListItemTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
	if (start) {
		bookReader().endParagraph();
		bookReader().beginParagraph();
		if (!myReader.myListNumStack.empty()) {
			//TODO: add spaces and number/bullet
			myReader.addConvertedDataToBuffer("\342\200\242 ", 4, false);
		}
	}
}

void HtmlBookReader::addAction(const std::string &tag, HtmlTagAction *action) {
	std::map<std::string,HtmlTagAction*>::iterator it = myActionMap.find(tag);
	if (it != myActionMap.end()) {
		if (it->second != 0) {
			delete it->second;
		}
		myActionMap.erase(it);
	}
	myActionMap.insert(std::pair<std::string,HtmlTagAction*>(tag, action));
}

void HtmlBookReader::setBuildTableOfContent(bool build) {
	myBuildTableOfContent = build;
}

HtmlBookReader::HtmlBookReader(const std::string &baseDirectoryPath, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlReader(encoding), myBookReader(model), myBaseDirPath(baseDirectoryPath), myFormat(format), myBuildTableOfContent(true) {
	addAction("EM", new HtmlControlTagAction(*this, EMPHASIS));
	addAction("STRONG", new HtmlControlTagAction(*this, STRONG));
	addAction("B", new HtmlControlTagAction(*this, BOLD));
	addAction("I", new HtmlControlTagAction(*this, ITALIC));
	addAction("TT", new HtmlControlTagAction(*this, CODE));
	addAction("CODE", new HtmlControlTagAction(*this, CODE));
	addAction("CITE", new HtmlControlTagAction(*this, CITE));
	addAction("SUB", new HtmlControlTagAction(*this, SUB));
	addAction("SUP", new HtmlControlTagAction(*this, SUP));
	addAction("H1", new HtmlHeaderTagAction(*this, H1));
	addAction("H2", new HtmlHeaderTagAction(*this, H2));
	addAction("H3", new HtmlHeaderTagAction(*this, H3));
	addAction("H4", new HtmlHeaderTagAction(*this, H4));
	addAction("H5", new HtmlHeaderTagAction(*this, H5));
	addAction("H6", new HtmlHeaderTagAction(*this, H6));
	addAction("HEAD", new HtmlIgnoreTagAction(*this));
	addAction("TITLE", new HtmlIgnoreTagAction(*this));
	addAction("STYLE", new HtmlIgnoreTagAction(*this));
	addAction("SELECT", new HtmlIgnoreTagAction(*this));
	addAction("SCRIPT", new HtmlIgnoreTagAction(*this));
	addAction("A", new HtmlHrefTagAction(*this));
	addAction("TR", new HtmlBreakTagAction(*this, HtmlBreakTagAction::BREAK_AT_END));
	addAction("DIV", new HtmlBreakTagAction(*this, HtmlBreakTagAction::BREAK_AT_END));
	addAction("DT", new HtmlBreakTagAction(*this, HtmlBreakTagAction::BREAK_AT_START));
	addAction("P", new HtmlBreakTagAction(*this, HtmlBreakTagAction::BREAK_AT_START_AND_AT_END));
	addAction("BR", new HtmlBreakTagAction(*this, HtmlBreakTagAction::BREAK_AT_START_AND_AT_END));
	addAction("IMG", new HtmlImageTagAction(*this));
	addAction("UL", new HtmlListTagAction(*this));
	addAction("MENU", new HtmlListTagAction(*this));
	addAction("DIR", new HtmlListTagAction(*this));
	addAction("OL", new HtmlListTagAction(*this));
	addAction("LI", new HtmlListItemTagAction(*this));
	addAction("PRE", new HtmlPreTagAction(*this));
	// addAction("DD", 0);
	// addAction("DL", 0);
	// addAction("DFN", 0);
	// addAction("SAMP", 0);
	// addAction("KBD", 0);
	// addAction("VAR", 0);
	// addAction("ABBR", 0);
	// addAction("ACRONYM", 0);
	// addAction("BLOCKQUOTE", 0);
	// addAction("Q", 0);
	// addAction("INS", 0);
	// addAction("DEL", 0);
	// addAction("BODY", 0);
}

HtmlBookReader::~HtmlBookReader() {
	for (std::map<std::string,HtmlTagAction*>::const_iterator it = myActionMap.begin(); it != myActionMap.end(); ++it) {
		if (it->second != 0) {
			delete it->second;
		}
	}
}

void HtmlBookReader::addConvertedDataToBuffer(const char *text, int len, bool convert) {
	if (len > 0) {
		if (convert) {
			myConverter->convert(myConverterBuffer, text, text + len);
			myBookReader.addData(myConverterBuffer);
			myBookReader.addContentsData(myConverterBuffer);
			myConverterBuffer.erase();
		} else {
			std::string strText(text, len);
			myBookReader.addData(strText);
			myBookReader.addContentsData(strText);
		}
	}
}

bool HtmlBookReader::tagHandler(const HtmlTag &tag) {
	myConverter->reset();

	HtmlTagAction *action = myActionMap[tag.Name];
	if (action != 0) {
		action->run(tag.Start, tag.Attributes);
	}

	return true;
}

bool HtmlBookReader::characterDataHandler(const char *text, int len, bool convert) {
	if (myIgnoreDataCounter == 0) {
		const char *start = text;
		const char *end = text + len;
		if (myIsPreformatted) {
			int breakType = myFormat.breakType();
			if (breakType & PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE) {
				for (const char *ptr = text; ptr != end; ++ptr) {
					if (*ptr == '\n') {
						mySpaceCounter = 0;
						if (start < ptr) {
							addConvertedDataToBuffer(start, ptr - start, convert);
						} else {
							static const std::string SPACE = " ";
							myBookReader.addData(SPACE);
						}
						myBookReader.endParagraph();
						myBookReader.beginParagraph();
						start = ptr + 1;
					} else if (mySpaceCounter >= 0) {
						if (isspace(*ptr)) {
							++mySpaceCounter;
						} else {
							myBookReader.addFixedHSpace(mySpaceCounter);
							mySpaceCounter = -1;
						}
					}
				}
				addConvertedDataToBuffer(start, end - start, convert);
			} else if (breakType & PlainTextFormat::BREAK_PARAGRAPH_AT_LINE_WITH_INDENT) {
				for (const char *ptr = text; ptr != end; ++ptr) {
					if (isspace(*ptr)) {
						if (*ptr == '\n') {
							mySpaceCounter = 0;
						} else if (mySpaceCounter >= 0) {
							++mySpaceCounter;
						}
					} else {
						if (mySpaceCounter > myFormat.ignoredIndent()) {
							if (ptr - start > mySpaceCounter) {
								addConvertedDataToBuffer(start, ptr - start - mySpaceCounter, convert);
								myBookReader.endParagraph();
								myBookReader.beginParagraph();
							}
							start = ptr;
						}
						mySpaceCounter = -1;
					}
				}
				mySpaceCounter = std::max(mySpaceCounter, 0);
				if (end - start > mySpaceCounter) {
					addConvertedDataToBuffer(start, end - start - mySpaceCounter, convert);
				}
			} else if (breakType & PlainTextFormat::BREAK_PARAGRAPH_AT_EMPTY_LINE) {
				for (const char *ptr = start; ptr != end; ++ptr) {
					if (isspace(*ptr)) {
						if (*ptr == '\n') {
							++myBreakCounter;
						}
					} else {
						if (myBreakCounter > 1) {
							addConvertedDataToBuffer(start, ptr - start, convert);
							myBookReader.endParagraph();
							myBookReader.beginParagraph();
							start = ptr;
						}
						myBreakCounter = 0;
					}
				}
				addConvertedDataToBuffer(start, end - start, convert);
			}
		} else {
			if (!myIsStarted) {
				for (; start != end; ++start) {
					if (!isspace(*start)) {
						myIsStarted = true;
						break;
					}
				}
			}
			addConvertedDataToBuffer(start, end - start, convert);
		}
	}
	return true;
}

void HtmlBookReader::startDocumentHandler() {
	while (!myListNumStack.empty()) {
		myListNumStack.pop();
	}
	myConverterBuffer.erase();
	myKindList.clear();

	myBookReader.reset();
	myBookReader.setMainTextModel();
	myBookReader.pushKind(REGULAR);
	myBookReader.beginParagraph();
	myIgnoreDataCounter = 0;
	myIsPreformatted = false;
	myIsHyperlink = false;
	myIsStarted = false;

	mySpaceCounter = -1;
	myBreakCounter = 0;
}

void HtmlBookReader::endDocumentHandler() {
	myBookReader.endParagraph();
}
