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

#include <cctype>
#include <algorithm>

#include <ZLOptions.h>
#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include <ZLFile.h>

#include "PlainTextFormat.h"

#include "../../FBOptions.h"

const std::string OPTION_Initialized = "Initialized";
const std::string OPTION_BreakType = "BreakType";
const std::string OPTION_IgnoredIndent = "IgnoredIndent";
const std::string OPTION_EmptyLinesBeforeNewSection = "EmptyLinesBeforeNewSection";
const std::string OPTION_CreateContentsTable = "CreateContentsTable";

PlainTextFormat::PlainTextFormat(const std::string &fileName) :
	InitializedOption(FBOptions::BOOKS_CATEGORY, fileName, OPTION_Initialized, false),
	BreakTypeOption(FBOptions::BOOKS_CATEGORY, fileName, OPTION_BreakType, 1),
	IgnoredIndentOption(FBOptions::BOOKS_CATEGORY, fileName, OPTION_IgnoredIndent, 1, 100, 1),
	EmptyLinesBeforeNewSectionOption(FBOptions::BOOKS_CATEGORY, fileName, OPTION_EmptyLinesBeforeNewSection, 1, 100, 1),
	CreateContentsTableOption(FBOptions::BOOKS_CATEGORY, fileName, OPTION_CreateContentsTable, false) {
}

PlainTextInfoPage::PlainTextInfoPage(ZLOptionsDialog &dialog, const std::string &fileName, const std::string &tabName, bool showContentsEntry) : myFormat(fileName) {
	if (!myFormat.initialized()) {
		PlainTextFormatDetector detector;
		shared_ptr<ZLInputStream> stream = ZLFile(fileName).inputStream();
		if (!stream.isNull()) {
			detector.detect(*stream, myFormat);
		}
	}

	ZLDialogContent &tab = dialog.createTab(tabName);

	BreakTypeOptionEntry *breakEntry = new BreakTypeOptionEntry(*this, "Break Paragraph At", myFormat.BreakTypeOption);
	myIgnoredIndentEntry = new ZLSimpleSpinOptionEntry("Ignore Indent Less Than", myFormat.IgnoredIndentOption, 1);
	tab.addOption(breakEntry);
	tab.addOption(myIgnoredIndentEntry);
	breakEntry->onValueChange(breakEntry->initialValue());

	if (showContentsEntry) {
		CreateContentsTableOptionEntry *contentsTableEntry = new CreateContentsTableOptionEntry(*this, "Build Contents Table", myFormat.CreateContentsTableOption);
		myEmptyLinesBeforeNewSectionEntry = new ZLSimpleSpinOptionEntry("Empty Lines Before New Section", myFormat.EmptyLinesBeforeNewSectionOption, 1);
		tab.addOption(contentsTableEntry);
		tab.addOption(myEmptyLinesBeforeNewSectionEntry);
		contentsTableEntry->onValueChange(contentsTableEntry->initialState());
	}
}

PlainTextInfoPage::~PlainTextInfoPage() {
}

const int BUFFER_SIZE = 4096;

void PlainTextFormatDetector::detect(ZLInputStream &stream, PlainTextFormat &format) {
	if (!stream.open()) {
		return;
	}

	const unsigned int tableSize = 10;

	unsigned int lineCounter = 0;
	unsigned int emptyLineCounter = 0;
	unsigned int libRuHeaderLineCounter = 0;
	unsigned int stringsWithLengthLessThan81Counter = 0;
	unsigned int stringIndentTable[tableSize] = { 0 };
	unsigned int emptyLinesTable[tableSize] = { 0 };
	unsigned int emptyLinesBeforeShortStringTable[tableSize] = { 0 };

	bool currentLineIsEmpty = true;
	bool currentLineIsLikeToLibRuHeader = false;
	unsigned int currentLineLength = 0;
	unsigned int currentLineIndent = 0;
	unsigned int currentNumberOfEmptyLines = 0;
	
	char *buffer = new char[BUFFER_SIZE];
	int length;
	char previous = 0;
	do {
		length = stream.read(buffer, BUFFER_SIZE);
		const char *end = buffer + length;
		for (const char *ptr = buffer; ptr != end; ++ptr) {
			++currentLineLength;
			if ((*ptr == '\r') || ((*ptr == '\n') && (previous != '\r'))) {
				++lineCounter;
				if (currentLineIsEmpty) {
					++emptyLineCounter;
					++currentNumberOfEmptyLines;
				} else {
					emptyLinesTable[std::min(currentNumberOfEmptyLines, tableSize - 1)]++;
					if (currentLineLength < 51) {
						emptyLinesBeforeShortStringTable[std::min(currentNumberOfEmptyLines, tableSize - 1)]++;
					}
					currentNumberOfEmptyLines = 0;
				}
				if (currentLineIsLikeToLibRuHeader) {
					++libRuHeaderLineCounter;
				}
				if (currentLineLength < 81) {
					++stringsWithLengthLessThan81Counter;
				}
				if (!currentLineIsEmpty) {
					stringIndentTable[std::min(currentLineIndent, tableSize - 1)]++;
				}
				
				currentLineIsLikeToLibRuHeader = currentLineIsEmpty;
				currentLineIsEmpty = true;
				currentLineLength = 0;
				currentLineIndent = 0;
			} else if (isspace(*ptr)) {
				if (currentLineIsEmpty) {
					++currentLineIndent;
				}
			} else {
				currentLineIsEmpty = false;
				if (*ptr != '-') {
					currentLineIsLikeToLibRuHeader = false;
				}
			}
			previous = *ptr;
		}
	} while (length == BUFFER_SIZE);
	delete[] buffer;

	unsigned int nonEmptyLineCounter = lineCounter - emptyLineCounter;

	{
		unsigned int indent = 0;
		unsigned int lineWithIndent = 0;
		for (; indent < tableSize; ++indent) {
			lineWithIndent += stringIndentTable[indent];
			if (lineWithIndent > 0.1 * nonEmptyLineCounter) {
				break;
			}
		}
		format.IgnoredIndentOption.setValue(indent + 1);
	}

	{
		int breakType = 0;
		breakType |= PlainTextFormat::BREAK_PARAGRAPH_AT_EMPTY_LINE;
		if (stringsWithLengthLessThan81Counter < 0.3 * nonEmptyLineCounter) {
			breakType |= PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE;
		} else {
			breakType |= PlainTextFormat::BREAK_PARAGRAPH_AT_LINE_WITH_INDENT;
		}
		format.BreakTypeOption.setValue(breakType);
	}

	{
		unsigned int max = 0;
		unsigned index;
		int emptyLinesBeforeNewSection = -1;
		for (index = 2; index < tableSize; ++index) {
			if (max < emptyLinesBeforeShortStringTable[index]) {
				max = emptyLinesBeforeShortStringTable[index];
				emptyLinesBeforeNewSection = index;
			}
		}
		if (emptyLinesBeforeNewSection > 0) {
			for (index = tableSize - 1; index > 0; --index) {
				emptyLinesTable[index - 1] += emptyLinesTable[index];	
				emptyLinesBeforeShortStringTable[index - 1] += emptyLinesBeforeShortStringTable[index];	
			}
			for (index = emptyLinesBeforeNewSection; index < tableSize; ++index) {
				if ((emptyLinesBeforeShortStringTable[index] > 2) &&
						(emptyLinesBeforeShortStringTable[index] > 0.7 * emptyLinesTable[index])) {
					break;
				}
			}
			emptyLinesBeforeNewSection = (index == tableSize) ? -1 : (int)index;
		}
		format.EmptyLinesBeforeNewSectionOption.setValue(emptyLinesBeforeNewSection);
		format.CreateContentsTableOption.setValue(emptyLinesBeforeNewSection > 0);
	}

	format.InitializedOption.setValue(true);
}

BreakTypeOptionEntry::BreakTypeOptionEntry(PlainTextInfoPage &page, const std::string &name, ZLIntegerOption &breakTypeOption) : myPage(page), myName(name), myBreakTypeOption(breakTypeOption) {
}

BreakTypeOptionEntry::~BreakTypeOptionEntry() {
}

const std::string &BreakTypeOptionEntry::name() const {
	return myName;
}

static std::string BREAK_TYPE_VALUES[3] = { "New Line", "Empty Line", "Line With Indent"};
static std::vector<std::string> BREAK_TYPE_VALUES_VECTOR;

const std::string &BreakTypeOptionEntry::initialValue() const {
	switch (myBreakTypeOption.value()) {
		case PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE:
			return BREAK_TYPE_VALUES[0];
		case PlainTextFormat::BREAK_PARAGRAPH_AT_EMPTY_LINE:
			return BREAK_TYPE_VALUES[1];
		case PlainTextFormat::BREAK_PARAGRAPH_AT_EMPTY_LINE | PlainTextFormat::BREAK_PARAGRAPH_AT_LINE_WITH_INDENT:
		default:
			return BREAK_TYPE_VALUES[2];
	}
}

const std::vector<std::string> &BreakTypeOptionEntry::values() const {
	if (BREAK_TYPE_VALUES_VECTOR.empty()) {
		for (int i = 0; i < 3; ++i) {
			BREAK_TYPE_VALUES_VECTOR.push_back(BREAK_TYPE_VALUES[i]);
		}
	}
	return BREAK_TYPE_VALUES_VECTOR;
}

void BreakTypeOptionEntry::onAccept(const std::string &value) {
	if (value == BREAK_TYPE_VALUES[0]) {
		myBreakTypeOption.setValue(PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE);
	} else if (value == BREAK_TYPE_VALUES[1]) {
		myBreakTypeOption.setValue(PlainTextFormat::BREAK_PARAGRAPH_AT_EMPTY_LINE);
	} else if (value == BREAK_TYPE_VALUES[2]) {
		myBreakTypeOption.setValue(PlainTextFormat::BREAK_PARAGRAPH_AT_EMPTY_LINE | PlainTextFormat::BREAK_PARAGRAPH_AT_LINE_WITH_INDENT);
	}
}

void BreakTypeOptionEntry::onValueChange(const std::string &selectedValue) {
	myPage.myIgnoredIndentEntry->setVisible(selectedValue == BREAK_TYPE_VALUES[2]);
}

CreateContentsTableOptionEntry::CreateContentsTableOptionEntry(PlainTextInfoPage &page, const std::string &name, ZLBooleanOption &option) : ZLSimpleBooleanOptionEntry(name, option), myPage(page) {
}

CreateContentsTableOptionEntry::~CreateContentsTableOptionEntry() {
}

void CreateContentsTableOptionEntry::onValueChange(bool value) {
	myPage.myEmptyLinesBeforeNewSectionEntry->setVisible(value);
}
