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

/* 
 * Information about Palm Markup Language was taken from:
 * http://www.m.ereader.com/ereader/help/dropbook/pml.htm
 * http://ccit205.wikispaces.com/Palm+Markup+Language+(PML)
 */

#include <cstdlib>
#include <cctype>

#include <ZLFile.h>
#include <ZLInputStream.h>

#include "PmlReader.h"

static const int pmlStreamBufferSize = 4096;

const std::string PmlReader::ourDefaultParameter = "";

PmlReader::PmlReader(const std::string &encoding) : EncodedTextReader(encoding) {
}

PmlReader::~PmlReader() {
}

bool PmlReader::readDocument(ZLInputStream& stream) {
	myStreamBuffer = new char[pmlStreamBufferSize];
	
	myIsInterrupted = false;

	myState.Italic = false;
	myState.Bold = false;
	myState.Underlined = false;
	myState.SmallCaps = false;
	myState.Subscript = false;
    myState.Superscript = false;
	myState.Alignment = ALIGN_UNDEFINED;
	myState.FontSize = NORMAL;
	myState.Indent = 0;
	myState.IndentBlockOn = false;
	myState.BoldBlockOn = false;
	myState.FootnoteLinkOn = false;
	myState.InternalLinkOn = false;
	myState.InvisibleText = false;
	
	bool code = parseDocument(stream);

	delete[] myStreamBuffer;
	
	return code;
}

bool PmlReader::parseDocument(ZLInputStream &stream) {
	enum {
		READ_NORMAL_DATA,
		READ_TAG,
		READ_TAG_PARAMETER,
	} parserState = READ_NORMAL_DATA;

	size_t tagNameLength = 0;
	std::string tagName;
	std::string parameterString;
	
	bool startParameterReading = false;
	size_t tagCounter = 0;
	static bool FLAG = true;

	while (!myIsInterrupted) {
		const char *ptr = myStreamBuffer;
		const char *end = myStreamBuffer + stream.read(myStreamBuffer, pmlStreamBufferSize);
		if (ptr == end) {
			break;
		}
		const char *dataStart = ptr;
		bool readNextChar = true;
		while (ptr != end) {
			switch (parserState) {
				case READ_NORMAL_DATA:
					if (*ptr == '\n') {
						if (ptr > dataStart) {
							processCharData(dataStart, ptr - dataStart);
						}
						newLine();
						FLAG = true;
						dataStart = ptr + 1;
					} else if (FLAG && isspace(*ptr)) {
					} else {
						FLAG = false;
						if (*ptr == '\\') {
							if (ptr > dataStart) {
								processCharData(dataStart, ptr - dataStart);
							}
							dataStart = ptr + 1;
							tagName.erase();
							parserState = READ_TAG;
						}
					}
					break;
				case READ_TAG:
					if ((ptr == dataStart) && (tagName.empty())) {
						if (*ptr == '\\') {
							processCharData(ptr, 1);
							dataStart = ptr + 1;
							parserState = READ_NORMAL_DATA;
						} else {
							tagNameLength = findTagLength(ptr);
							if (tagNameLength == 0) {
								dataStart = ptr + 1;
								parserState = READ_NORMAL_DATA;
								++tagCounter;
							} else {
								--tagNameLength;
							}
						}
					} else {
						if (tagNameLength == 0) {
							tagName.append(dataStart, ptr - dataStart);
							if (*ptr == '=') {
								dataStart = ptr + 1; 
								parameterString.erase();
								parserState = READ_TAG_PARAMETER;
								++tagCounter;
							} else {
								readNextChar = false;
								processTag(tagName);
								dataStart = ptr;
								parserState = READ_NORMAL_DATA;
								++tagCounter;
							}
						} else {
							--tagNameLength;
						}
					}
					break;
				case READ_TAG_PARAMETER:
					if (*ptr == '"') { 
						if (!startParameterReading) {
							startParameterReading = true;
							dataStart = ptr + 1;  
						} else {
							parameterString.append(dataStart, ptr - dataStart);
							processTag(tagName, parameterString);
							parserState = READ_NORMAL_DATA;
							dataStart =  ptr + 1;
							startParameterReading = false;
						}
					}
					break;
			}
			if (readNextChar) {
				++ptr;
			} else {
				readNextChar = true;
			}
		}
		if (dataStart < end) {
			switch (parserState) {
				case READ_NORMAL_DATA:
					processCharData(dataStart, end - dataStart);
				case READ_TAG:
					tagName.append(dataStart, end - dataStart);
					break;
				case READ_TAG_PARAMETER:
					parameterString.append(dataStart, end - dataStart);
					break;
				default:
					break;
			}
		}
	}
	return myIsInterrupted;
}

size_t PmlReader::findTagLength(const char* ptr) {
	switch(*ptr) {		// tag action description			| close	| support	|
		case 'p':		// new page							|	-	|	+		|	
		case 'x':		// new chapter and new page			|	+	|	+		|
		case 'c':		// center alignment block			|	+	|	+		|
		case 'r':		// right alignment block			|	+	|	+		|
		case 'i':		// italize block					|	+	|	+		|
		case 'u':		// underlined block					|	+	|	+		|
		case 'o':		// overstrike block					|	+	|	-		|
		case 'v':		// invisible text block				|	+	|	+		|
		case 't':		// indent block						|	+	|	+		|
		case 'T':		// indent with value				|	-	|	+		|
		case 'w':		// embed text width rule			|	-	|	-		|	
		case 'n':		// switch to normal font			|	-	|	+		|	
		case 's':		// switch to std font				|+ or \n|	+		|	
		case 'b':		// switch to bold font (deprecated)	|+ or \n|	-		|	
		case 'l':		// switch to large font				|+ or \n|	+		|	
		case 'B':		// mark text as bold				|	+	|	+		|	
		case 'k':		// smaller font size and uppercase	|	+	|	+		|	
		case 'm':		// insert named image				|	-	|	+		|	
		case 'q':		// reference to another spot		| 	+	|	+		|
		case 'Q':		// link anchor for \q reference		|	-	|	+		|
		case '-':		// soft hyphen						|	-	|	-		|
		case 'I':		// reference index item				|	-	|	-		|
			return 1;
		case 'X':		// XN - new chapter, n indent level	|	+	|	-		|
		case 'S':		// Sp - mark text as superscript	|	+	|	+		|
						// Sb - mark text as subscript		|	+	|	+		|
						// Sd -	link to a sidebar			|	+	|	-		| 
		case 'C':		// CN - chapter title + indent level|	-	|	-		|
		case 'F':		// Fn - link to a footnote			|	+	|	+		|
			return 2;
		default:
			return 0;
	}
}


void PmlReader::interrupt() {
	myIsInterrupted = true;
}


void PmlReader::processTag(std::string &tagName, const std::string &parameter) {
	const char tagDeterminant = *tagName.data();
	switch (tagDeterminant) {
		case 'p':
			newPage();
			break;
		case 'x':
			//TODO add close tag processing
			newPage();
			break;
		case 'B':
			if (!myState.BoldBlockOn) {
				processFontProperty(FONT_BOLD);
			}
			break;
		case 'i': 
			processFontProperty(FONT_ITALIC);
			break;
		case 'u': 
			processFontProperty(FONT_UNDERLINED);
			break;
		case 'v': 
			myState.InvisibleText = !myState.InvisibleText;;
			break;
		case 'c':
			processAlignment(ALIGN_CENTER);
			break;
		case 'r':
			processAlignment(ALIGN_RIGHT);
			break;
		case 'n':
			processFontSize(NORMAL);
			break;
		case 'b':
			myState.BoldBlockOn = !myState.BoldBlockOn;
			processFontProperty(FONT_BOLD);
			break;
		case 's':
			processFontSize(SMALLER);
			break;
		case 'l':
			processFontSize(LARGER);
			break;
		case 'k':
			myState.SmallCaps = !myState.SmallCaps;
			processFontSize(SMALLER);
			break;
		case 'S':
			if (tagName == "Sb") {
				processFontProperty(FONT_SUBSCRIPT);
			} else if (tagName == "Sp") {
				processFontProperty(FONT_SUPERSCRIPT);
			} else if (tagName == "Sd") {
				//processSidebarLink();
			}
			break;
		case 't':
			processIndent();
			break;
		case 'T':
			processIndent(parameter);
			myState.IndentBlockOn = false;
			break;
		case 'w':
			//addHorizontalRule(parameter);
			break;	
		case 'F':
			processLink(FOOTNOTE, parameter);
			break;
		case 'q':
			processLink(INTERNAL_HYPERLINK, parameter);
			break;
		case 'Q':
			addLinkLabel(parameter);
			break;
		case 'm':
			addImageReference(parameter);
			break;
		default:
			//std::cerr << "PmlReader: unsupported tag: name: " << tagName << "	parameter: " << parameter << "\n"; 
			break;
	}
}

void PmlReader::processCharData(const char* data, size_t len, bool convert) {
	if(!myState.InvisibleText) {
		addCharData(data, len, convert);
	}
}

void PmlReader::processFontProperty(PmlReader::FontProperty property) {
	switch (property) {
		case FONT_BOLD:
			myState.Bold = !myState.Bold;
			switchFontProperty(FONT_BOLD);
			break;
		case FONT_ITALIC:
		   	myState.Italic = !myState.Italic;
			switchFontProperty(FONT_ITALIC);
			break;
		case FONT_UNDERLINED:
			myState.Underlined = !myState.Underlined;
			switchFontProperty(FONT_UNDERLINED);
			break;
		case FONT_SUBSCRIPT:
			myState.Subscript = !myState.Subscript;
			switchFontProperty(FONT_SUBSCRIPT);
			break;
		case FONT_SUPERSCRIPT:
			myState.Superscript = !myState.Superscript;
			switchFontProperty(FONT_SUPERSCRIPT);
			break;
	}
}

void PmlReader::processAlignment(ZLTextAlignmentType alignment) {
	if (myState.Alignment != alignment) {
		myState.Alignment = alignment;
	} else {
		myState.Alignment = ALIGN_UNDEFINED;
	}
	newParagraph();
}

void PmlReader::processFontSize(FontSizeType sizeType) {
	if (myState.FontSize != sizeType) {
		myState.FontSize = sizeType;
	} else {
		myState.FontSize = NORMAL;
	}
	setFontSize();
}

void PmlReader::processIndent(const std::string& parameter) {
	int indentPercentSize = 5;
	if (!parameter.empty()) {
		const int index = parameter.find('%');
        if (index != -1) {
        	const std::string indentValueStr = parameter.substr(0, index);
			indentPercentSize = atoi(indentValueStr.data());
		} else {
			indentPercentSize = 5;
		}
	}
	if(!myState.IndentBlockOn) {
		myState.Indent = indentPercentSize;
	} else {
		myState.Indent = 0;
	}
	myState.IndentBlockOn = !myState.IndentBlockOn;
	newParagraph();
}

void PmlReader::processLink(FBTextKind kind, const std::string &parameter) {
	switch(kind) {
		case FOOTNOTE:
			myState.FootnoteLinkOn = !myState.FootnoteLinkOn;
			addLink(FOOTNOTE, parameter, myState.FootnoteLinkOn);	
			break;
		case INTERNAL_HYPERLINK:
			myState.InternalLinkOn = !myState.InternalLinkOn;
			if (parameter.size() > 1) { 
				// '#' character has to stand before link label , so we should omit '#' for getting label
				addLink(INTERNAL_HYPERLINK, parameter.substr(1), myState.InternalLinkOn);	
			} else {
				// In case trailing or corrupted tag we use parameter entirely
				addLink(INTERNAL_HYPERLINK, parameter, myState.InternalLinkOn);	
			}
			break;
		default:
			break;
	}
}
