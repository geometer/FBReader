/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <stdlib.h>

#include <iostream>

#include <ZLStringUtil.h>
#include <ZLInputStream.h>

#include "PdfBookReader.h"
#include "PdfObject.h"
#include "../../bookmodel/BookModel.h"

enum PdfCharacterType {
	PDF_CHAR_REGULAR,
	PDF_CHAR_WHITESPACE,
	PDF_CHAR_DELIMITER
};

static PdfCharacterType *PdfCharacterTypeTable = 0;

static void skipWhiteSpaces(ZLInputStream &stream, char &ch) {
	while ((PdfCharacterTypeTable[(unsigned char)ch] == PDF_CHAR_WHITESPACE) &&
				 (stream.read(&ch, 1) == 1)) {
	}
}

static void readToken(ZLInputStream &stream, std::string &buffer, char &ch) {
	buffer.clear();
	skipWhiteSpaces(stream, ch);
	while (PdfCharacterTypeTable[(unsigned char)ch] == PDF_CHAR_REGULAR) {
		buffer += ch;
		if (stream.read(&ch, 1) != 1) {
			break;
		}
	}
}

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

shared_ptr<PdfObject> readObject(ZLInputStream &stream, char &ch) {
	skipWhiteSpaces(stream, ch);

	PdfObject::Type type = PdfObject::NIL;
	bool hexString = false;
	switch (ch) {
		case '(':
			hexString = false;
			type = PdfObject::STRING;
			break;
		case '<':
			stream.read(&ch, 1);
			hexString = true;
			type = (ch == '<') ? PdfObject::DICTIONARY : PdfObject::STRING;
			break;
		case '>': // end of dictionary
			stream.read(&ch, 1);
			if (ch == '>') {
				stream.read(&ch, 1);
			}
			return 0;
		case '/':
			type = PdfObject::NAME;
			break;
		case '[':
			type = PdfObject::ARRAY;
			break;
		case ']': // end of array
			stream.read(&ch, 1);
			return 0;
		case '+':
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			type = PdfObject::INTEGER_NUMBER;
			break;
		case 't':
		case 'f':
			type = PdfObject::BOOLEAN;
			break;
	}

	switch (type) {
		case PdfObject::DICTIONARY:
		{
			ch = 0;
			shared_ptr<PdfObject> name;
			shared_ptr<PdfObject> value;
			shared_ptr<PdfObject> next;
			PdfDictionaryObject *dictionary = new PdfDictionaryObject();
			while (true) {
				next = readObject(stream, ch);
				if (next.isNull()) {
					break;
				}
				PdfObject::Type oType = next->type();
				if (oType == PdfObject::NAME) {
					name = next;
					value = readObject(stream, ch);
					if (value.isNull()) {
						break;
					}
					dictionary->setObject(name, value);
				} else if (oType == PdfObject::INTEGER_NUMBER) {
					if (value.isNull() || (value->type() != PdfObject::INTEGER_NUMBER)) {
						break;
					}
					skipWhiteSpaces(stream, ch);
					if (ch != 'R') {
						break;
					}
					const int number = ((PdfIntegerObject&)*value).value();
					const int generation = ((PdfIntegerObject&)*next).value();
					dictionary->setObject(name, new PdfObjectReference(number, generation));
					value = 0;
					ch = 0;
				} else {
					break;
				}
			}
			return dictionary;
		}
		case PdfObject::NAME:
		{
			std::string name;
			stream.read(&ch, 1);
			readToken(stream, name, ch);
			return PdfNameObject::nameObject(name);
		}
		case PdfObject::BOOLEAN:
		{
			std::string name;
			readToken(stream, name, ch);
			return (name == "true") ? PdfBooleanObject::TRUE() : PdfBooleanObject::FALSE();
		}
		case PdfObject::INTEGER_NUMBER:
		{
			std::string str;
			if ((ch == '+') || (ch == '-')) {
				str += ch;
				stream.read(&ch, 1);
			}
			while ((ch >= '0') && (ch <= '9')) {
				str += ch;
				stream.read(&ch, 1);
			}
			return PdfIntegerObject::integerObject(atoi(str.c_str()));
		}
		case PdfObject::STRING:
		{
			std::string value;
			if (hexString) {
				char num[3];
				num[2] = '\0';
				while (ch != '>') {
					num[0] = ch;
					stream.read(num + 1, 1);
					value += (char)strtol(num, 0, 16);
					stream.read(&ch, 1);
				}
				ch = 0;
			} else {
				// TODO: implement
			}
			return new PdfStringObject(value);
		}
		case PdfObject::ARRAY:
		{
			PdfArrayObject *array = new PdfArrayObject();
			ch = 0;
			while (true) {
				shared_ptr<PdfObject> object = readObject(stream, ch);
				if (object.isNull()) {
					break;
				}
				array->addObject(object);
			}
			std::cerr << "PdfArrayObject " << array->size() << "\n";
			return array;
		}
		default:
			break;
	}

	std::string buffer;
	stream.read(&ch, 1);
	while (PdfCharacterTypeTable[(unsigned char)ch] == PDF_CHAR_REGULAR) {
		buffer += ch;
		stream.read(&ch, 1);
	}
	std::cerr << "buffer = " << buffer << "\n";

	return 0;
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
	readToken(stream, myBuffer, ch);
	if (address.first != atoi(myBuffer.c_str())) {
		return 0;
	}
	readToken(stream, myBuffer, ch);
	if (address.second != atoi(myBuffer.c_str())) {
		return 0;
	}
	readToken(stream, myBuffer, ch);
	if (myBuffer != "obj") {
		return 0;
	}
	return readObject(stream, ch);
}

shared_ptr<PdfObject> PdfBookReader::resolveReference(shared_ptr<PdfObject> ref, ZLInputStream &stream) {
	if (ref.isNull() || (ref->type() != PdfObject::REFERENCE)) {
		return 0;
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
		shared_ptr<PdfObject> trailer = readObject(stream, ch);
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
	if (PdfCharacterTypeTable == 0) {
		PdfCharacterTypeTable = new PdfCharacterType[256];
		for (int i = 0; i < 256; ++i) {
			PdfCharacterTypeTable[i] = PDF_CHAR_REGULAR;
		}
		PdfCharacterTypeTable[0] = PDF_CHAR_WHITESPACE;
		PdfCharacterTypeTable[9] = PDF_CHAR_WHITESPACE;
		PdfCharacterTypeTable[10] = PDF_CHAR_WHITESPACE;
		PdfCharacterTypeTable[12] = PDF_CHAR_WHITESPACE;
		PdfCharacterTypeTable[13] = PDF_CHAR_WHITESPACE;
		PdfCharacterTypeTable[32] = PDF_CHAR_WHITESPACE;
		PdfCharacterTypeTable['('] = PDF_CHAR_DELIMITER;
		PdfCharacterTypeTable[')'] = PDF_CHAR_DELIMITER;
		PdfCharacterTypeTable['<'] = PDF_CHAR_DELIMITER;
		PdfCharacterTypeTable['>'] = PDF_CHAR_DELIMITER;
		PdfCharacterTypeTable['['] = PDF_CHAR_DELIMITER;
		PdfCharacterTypeTable[']'] = PDF_CHAR_DELIMITER;
		PdfCharacterTypeTable['{'] = PDF_CHAR_DELIMITER;
		PdfCharacterTypeTable['}'] = PDF_CHAR_DELIMITER;
		PdfCharacterTypeTable['/'] = PDF_CHAR_DELIMITER;
		PdfCharacterTypeTable['%'] = PDF_CHAR_DELIMITER;
	}

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
	
	shared_ptr<PdfObject> count = pageRootNodeDictionary["Count"];
	if (!count.isNull() && (count->type() == PdfObject::INTEGER_NUMBER)) {
		std::cerr << "count = " << ((PdfIntegerObject&)*count).value() << "\n";
	}

	return true;
}
