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

#include <cstdlib>
#include <iostream>

#include <ZLStringUtil.h>
#include <ZLInputStream.h>

#include "PdfBookReader.h"
#include "PdfObject.h"
#include "../../bookmodel/BookModel.h"

static void readLine(ZLInputStream &stream, std::string &buffer) {
	buffer.clear();
	char ch;
	while (1) {
		if (stream.read(&ch, 1) != 1) {
			return;
		}
		if ((ch == 10) || (ch == 13)) {
			if (!buffer.empty()) {
				return;
			}
		} else {
			buffer += ch;
		}
	}
}

PdfBookReader::PdfBookReader(BookModel &model) : myModelReader(model) {
}

PdfBookReader::~PdfBookReader() {
}

shared_ptr<PdfObject> PdfBookReader::readObjectFromLocation(ZLInputStream &stream, const std::pair<int,int> &address) {
	std::map<std::pair<int,int>,int>::const_iterator jt = myObjectLocationMap.find(address);
	if (jt == myObjectLocationMap.end()) {
		return 0;
	}
	stream.seek(jt->second, true);
	char ch = 0;
	PdfObject::readToken(stream, myBuffer, ch);
	if (address.first != atoi(myBuffer.c_str())) {
		return 0;
	}
	PdfObject::readToken(stream, myBuffer, ch);
	if (address.second != atoi(myBuffer.c_str())) {
		return 0;
	}
	PdfObject::readToken(stream, myBuffer, ch);
	if (myBuffer != "obj") {
		return 0;
	}
	return PdfObject::readObject(stream, ch);
}

shared_ptr<PdfObject> PdfBookReader::resolveReference(shared_ptr<PdfObject> ref, ZLInputStream &stream) {
	if (ref.isNull() || (ref->type() != PdfObject::REFERENCE)) {
		return ref;
	}
	const PdfObjectReference &reference = (const PdfObjectReference&)*ref;
	const std::pair<int,int>address(reference.number(), reference.generation());
	std::map<std::pair<int,int>,shared_ptr<PdfObject> >::const_iterator it = myObjectMap.find(address);
	if (it != myObjectMap.end()) {
		return it->second;
	}
	std::map<std::pair<int,int>,int>::const_iterator jt = myObjectLocationMap.find(address);
	shared_ptr<PdfObject> object = readObjectFromLocation(stream, address);
	myObjectMap.insert(std::pair<std::pair<int,int>,shared_ptr<PdfObject> >(address, object));
	return object;
}

static void stripBuffer(std::string &buffer) {
	int index = buffer.find('%');
	if (index >= 0) {
		buffer.erase(index);
	}
	ZLStringUtil::stripWhiteSpaces(buffer);
}

bool PdfBookReader::readReferenceTable(ZLInputStream &stream, int xrefOffset) {
	while (true) {
		stream.seek(xrefOffset, true);
		readLine(stream, myBuffer);
		stripBuffer(myBuffer);
		if (myBuffer != "xref") {
			return false;
		}

		while (true) {
			readLine(stream, myBuffer);
			stripBuffer(myBuffer);
			if (myBuffer == "trailer") {
				break;
			}
			const int index = myBuffer.find(' ');
			const int start = atoi(myBuffer.c_str());
			const int len = atoi(myBuffer.c_str() + index + 1);
			for (int i = 0; i < len; ++i) {
				readLine(stream, myBuffer);
				stripBuffer(myBuffer);
				if (myBuffer.length() != 18) {
					return false;
				}
				const int objectOffset = atoi(myBuffer.c_str());
				const int objectGeneration = atoi(myBuffer.c_str() + 11);
				const bool objectInUse = myBuffer[17] == 'n';
				if (objectInUse) {
					myObjectLocationMap[std::pair<int,int>(start + i, objectGeneration)] = objectOffset;
				}
			}
		}
		char ch = 0;
		shared_ptr<PdfObject> trailer = PdfObject::readObject(stream, ch);
		if (trailer.isNull() || (trailer->type() != PdfObject::DICTIONARY)) {
			return false;
		}
		if (myTrailer.isNull()) {
			myTrailer = trailer;
		}
		PdfDictionaryObject &trailerDictionary = (PdfDictionaryObject&)*trailer;
		shared_ptr<PdfObject> previous = trailerDictionary["Prev"];
		if (previous.isNull()) {
			return true;
		}

		if (previous->type() != PdfObject::INTEGER_NUMBER) {
			return false;
		}
		xrefOffset = ((PdfIntegerObject&)*previous).value();
	}
}

bool PdfBookReader::readBook(shared_ptr<ZLInputStream> stream) {
	if (stream.isNull() || !stream->open()) {
		return false;
	}

	readLine(*stream, myBuffer);
	if (!ZLStringUtil::stringStartsWith(myBuffer, "%PDF-")) {
		return false;
	}

	std::string version = myBuffer.substr(5);
	std::cerr << "version = " << version << "\n";

	size_t eofOffset = stream->sizeOfOpened();
	if (eofOffset < 100) {
		return false;
	}

	stream->seek(eofOffset - 100, true);
	bool readXrefOffset = false;
	size_t xrefOffset = (size_t)-1;
	while (true) {
		readLine(*stream, myBuffer);
		if (myBuffer.empty()) {
			break;
		}
		stripBuffer(myBuffer);
		if (readXrefOffset) {
			if (!myBuffer.empty()) {
				xrefOffset = atoi(myBuffer.c_str());
				break;
			}
		} else if (myBuffer == "startxref") {
			readXrefOffset = true;
		}
	}

	if (!readReferenceTable(*stream, xrefOffset)) {
		return false;
	}

	PdfDictionaryObject &trailerDictionary = (PdfDictionaryObject&)*myTrailer;
	shared_ptr<PdfObject> root = resolveReference(trailerDictionary["Root"], *stream);
	if (root.isNull() || (root->type() != PdfObject::DICTIONARY)) {
		return false;
	}

	PdfDictionaryObject &rootDictionary = (PdfDictionaryObject&)*root;
	if (rootDictionary["Type"] != PdfNameObject::nameObject("Catalog")) {
		return false;
	}
	shared_ptr<PdfObject> pageRootNode = resolveReference(rootDictionary["Pages"], *stream);
	if (pageRootNode.isNull() || (pageRootNode->type() != PdfObject::DICTIONARY)) {
		return false;
	}
	PdfDictionaryObject &pageRootNodeDictionary = (PdfDictionaryObject&)*pageRootNode;
	if (pageRootNodeDictionary["Type"] != PdfNameObject::nameObject("Pages")) {
		return false;
	}
	
	/*
	shared_ptr<PdfObject> count = pageRootNodeDictionary["Count"];
	if (!count.isNull() && (count->type() == PdfObject::INTEGER_NUMBER)) {
		std::cerr << "count = " << ((PdfIntegerObject&)*count).value() << "\n";
	}
	*/
	shared_ptr<PdfObject> pages = pageRootNodeDictionary["Kids"];
	if (pages.isNull() || (pages->type() != PdfObject::ARRAY)) {
		return false;
	}
	const PdfArrayObject& pagesArray = (const PdfArrayObject&)*pages;
	const size_t pageNumber = pagesArray.size();
	for (size_t i = 0; i < pageNumber; ++i) {
		processPage(pagesArray[i], *stream);
	}

	return true;
}

void PdfBookReader::processContents(shared_ptr<PdfObject> contentsObject, ZLInputStream &stream) {
	contentsObject = resolveReference(contentsObject, stream);
}

void PdfBookReader::processPage(shared_ptr<PdfObject> pageObject, ZLInputStream &stream) {
	pageObject = resolveReference(pageObject, stream);
	if (pageObject.isNull() || pageObject->type() != PdfObject::DICTIONARY) {
		return;
	}
	const PdfDictionaryObject &pageDictionary = (const PdfDictionaryObject&)*pageObject;
	shared_ptr<PdfObject> contents = pageDictionary["Contents"];
	if (contents.isNull()) {
		return;
	}
	switch (contents->type()) {
		default:
			break;
		case PdfObject::REFERENCE:
			processContents(contents, stream);
			break;
		case PdfObject::ARRAY:
		{
			const PdfArrayObject &array = (const PdfArrayObject&)*contents;
			const size_t len = array.size();
			for (size_t i = 0; i < len; ++i) {
				processContents(array[i], stream);
			}
			break;
		}
	}
}
