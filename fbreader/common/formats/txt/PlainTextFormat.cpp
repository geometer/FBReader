/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <cctype>
#include <algorithm>

#include <abstract/ZLOptions.h>
#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLOptionEntry.h>
#include <abstract/ZLFSManager.h>

#include "PlainTextFormat.h"

const std::string OPTION_Initialized = "Initialized";
const std::string OPTION_BreakType = "BreakType";
const std::string OPTION_IgnoredIndent = "IgnoredIndent";
const std::string OPTION_EmptyLinesBeforeNewSection = "EmptyLinesBeforeNewSection";
const std::string OPTION_CreateContentsTable = "CreateContentsTable";

PlainTextFormat::PlainTextFormat(const std::string &fileName) :
	InitializedOption(fileName, OPTION_Initialized, false),
	BreakTypeOption(fileName, OPTION_BreakType, 0),
	IgnoredIndentOption(fileName, OPTION_IgnoredIndent, -1),
	EmptyLinesBeforeNewSectionOption(fileName, OPTION_EmptyLinesBeforeNewSection, -1),
	CreateContentsTableOption(fileName, OPTION_CreateContentsTable, false) {
}

PlainTextInfoPage::PlainTextInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) : myFormat(fileName) {
	if (!myFormat.initialized()) {
		PlainTextFormatDetector detector;
		ZLInputStream *stream = ZLFSManager::instance().createInputStream(fileName);
		if (stream != 0) {
			detector.detect(*stream, myFormat);
			delete stream;
		}
	}

	ZLOptionsDialogTab *tab = dialog.createTab("Text");
	tab->addOption(new ZLSimpleBooleanOptionEntry("Build Contenst Table", myFormat.CreateContentsTableOption));
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
	do {
		length = stream.read(buffer, BUFFER_SIZE);
		const char *end = buffer + length;
		for (const char *ptr = buffer; ptr != end; ptr++) {
			currentLineLength++;
			if (*ptr == '\n') {
				lineCounter++;
				if (currentLineIsEmpty) {
					emptyLineCounter++;
					currentNumberOfEmptyLines++;
				} else {
					emptyLinesTable[std::min(currentNumberOfEmptyLines, tableSize - 1)]++;
					if (currentLineLength < 51) {
						emptyLinesBeforeShortStringTable[std::min(currentNumberOfEmptyLines, tableSize - 1)]++;
					}
					currentNumberOfEmptyLines = 0;
				}
				if (currentLineIsLikeToLibRuHeader) {
					libRuHeaderLineCounter++;
				}
				if (currentLineLength < 81) {
					stringsWithLengthLessThan81Counter++;
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
					currentLineIndent++;
				}
			} else {
				currentLineIsEmpty = false;
				if (*ptr != '-') {
					currentLineIsLikeToLibRuHeader = false;
				}
			}
		}
	} while (length == BUFFER_SIZE);
	delete[] buffer;

	unsigned int nonEmptyLineCounter = lineCounter - emptyLineCounter;

	{
		unsigned int indent = 0;
		unsigned int lineWithIndent = 0;
		for (; indent < tableSize; indent++) {
			lineWithIndent += stringIndentTable[indent];
			if (lineWithIndent > 0.1 * nonEmptyLineCounter) {
				break;
			}
		}
		format.IgnoredIndentOption.setValue(indent);
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
		for (index = 2; index < tableSize; index++) {
			if (max < emptyLinesBeforeShortStringTable[index]) {
				max = emptyLinesBeforeShortStringTable[index];
				emptyLinesBeforeNewSection = index;
			}
		}
		if (emptyLinesBeforeNewSection > 0) {
			for (index = tableSize - 1; index > 0; index--) {
				emptyLinesTable[index - 1] += emptyLinesTable[index];	
				emptyLinesBeforeShortStringTable[index - 1] += emptyLinesBeforeShortStringTable[index];	
			}
			for (index = emptyLinesBeforeNewSection; index < tableSize; index++) {
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
