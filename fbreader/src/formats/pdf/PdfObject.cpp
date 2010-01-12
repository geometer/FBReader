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

#include <iostream>

#include <ZLInputStream.h>
#include <ZLZDecompressor.h>

#include "PdfObject.h"

PdfObject::~PdfObject() {
}

shared_ptr<PdfObject> PdfIntegerObject::integerObject(int value) {
	if ((value < 0) || (value >= 256)) {
		return new PdfIntegerObject(value);
	} else {
		static shared_ptr<PdfObject>* table = new shared_ptr<PdfObject>[256];
		if (table[value].isNull()) {
			table[value] = new PdfIntegerObject(value);
		}
		return table[value];
	}
}

PdfIntegerObject::PdfIntegerObject(int value) : myValue(value) {
	std::cerr << "PdfIntegerObject " << value << "\n";
}

int PdfIntegerObject::value() const {
	return myValue;
}

PdfObject::Type PdfIntegerObject::type() const {
	return INTEGER_NUMBER;
}

shared_ptr<PdfObject> PdfBooleanObject::TRUE() {
	static shared_ptr<PdfObject> value = new PdfBooleanObject(true);
	return value;
}

shared_ptr<PdfObject> PdfBooleanObject::FALSE() {
	static shared_ptr<PdfObject> value = new PdfBooleanObject(false);
	return value;
}

PdfBooleanObject::PdfBooleanObject(bool value) : myValue(value) {
	std::cerr << "PdfBooleanObject " << value << "\n";
}

bool PdfBooleanObject::value() const {
	return myValue;
}

PdfObject::Type PdfBooleanObject::type() const {
	return BOOLEAN;
}

PdfStringObject::PdfStringObject(const std::string &value) : myValue(value) {
	std::cerr << "PdfStringObject " << value << "\n";
}

PdfObject::Type PdfStringObject::type() const {
	return STRING;
}

std::map<std::string,shared_ptr<PdfObject> > PdfNameObject::ourObjectMap;

shared_ptr<PdfObject> PdfNameObject::nameObject(const std::string &id) {
	// TODO: process escaped characters
	std::map<std::string,shared_ptr<PdfObject> >::const_iterator it = ourObjectMap.find(id);
	if (it != ourObjectMap.end()) {
		return it->second;
	}
	std::cerr << "PdfNameObject " << id << "\n";
	shared_ptr<PdfObject> object = new PdfNameObject();
	ourObjectMap.insert(std::pair<std::string,shared_ptr<PdfObject> >(id, object));
	return object;
}

PdfNameObject::PdfNameObject() {
}

PdfObject::Type PdfNameObject::type() const {
	return NAME;
}

PdfDictionaryObject::PdfDictionaryObject() {
}

void PdfDictionaryObject::setObject(shared_ptr<PdfObject> id, shared_ptr<PdfObject> object) {
	myMap[id] = object;
}

shared_ptr<PdfObject> PdfDictionaryObject::operator[](shared_ptr<PdfObject> id) const {
	std::map<shared_ptr<PdfObject>,shared_ptr<PdfObject> >::const_iterator it = myMap.find(id);
	return (it != myMap.end()) ? it->second : 0;
}

shared_ptr<PdfObject> PdfDictionaryObject::operator[](const std::string &id) const {
	return operator[](PdfNameObject::nameObject(id));
}

PdfObject::Type PdfDictionaryObject::type() const {
	return DICTIONARY;
}

PdfArrayObject::PdfArrayObject() {
}

void PdfArrayObject::addObject(shared_ptr<PdfObject> object) {
	myVector.push_back(object);
}

shared_ptr<PdfObject> PdfArrayObject::popLast() {
	if (!myVector.empty()) {
		shared_ptr<PdfObject> last = myVector.back();
		myVector.pop_back();
		return last;
	}
	return 0;
}

int PdfArrayObject::size() const {
	return myVector.size();
}

shared_ptr<PdfObject> PdfArrayObject::operator[](int index) const {
	return myVector[index];
}

PdfObject::Type PdfArrayObject::type() const {
	return ARRAY;
}

PdfObjectReference::PdfObjectReference(int number, int generation) : myNumber(number), myGeneration(generation) {
}

int PdfObjectReference::number() const {
	return myNumber;
}

int PdfObjectReference::generation() const {
	return myGeneration;
}

PdfObject::Type PdfObjectReference::type() const {
	return REFERENCE;
}

PdfStreamObject::PdfStreamObject(const PdfDictionaryObject &dictionary, ZLInputStream &dataStream) {
	char ch;
	skipWhiteSpaces(dataStream, ch);

	shared_ptr<PdfObject> length = dictionary["Length"];
	if (!length.isNull() && (length->type() == INTEGER_NUMBER)) {
		int value = ((PdfIntegerObject&)*length).value();
		if (value > 0) {
			shared_ptr<PdfObject> filter = dictionary["Filter"];
			if (filter == PdfNameObject::nameObject("FlateDecode")) {
				dataStream.seek(1, false);
				ZLZDecompressor decompressor(value - 2);
				char buffer[2048];
				while (true) {
					size_t size = decompressor.decompress(dataStream, buffer, 2048);
					if (size == 0) {
						break;
					}
					myData.append(buffer, size);
				}
				std::cerr << myData << "\n";
			} else {
				myData.append(value, '\0');
				myData[0] = ch;
				dataStream.read((char*)myData.data() + 1, value - 1);
			}
		}
	}

	/*
	shared_ptr<PdfObject> filter = dictionary["Filter"];
	if (!filter.isNull()) {
		switch (filter->type()) {
			default:
				break;
			case NAME:
				myFilters.push_back(
					(filter == PdfNameObject::nameObject("FlateDecode")) ?
						FLATE : UNKNOWN
				);
				break;
			case ARRAY:
			{
				// TODO: process filters array
			}
		}
	}
	*/
}

PdfObject::Type PdfStreamObject::type() const {
	return STREAM;
}

enum PdfCharacterType {
	PDF_CHAR_REGULAR,
	PDF_CHAR_WHITESPACE,
	PDF_CHAR_DELIMITER
};

static PdfCharacterType *PdfCharacterTypeTable = 0;

void PdfObject::skipWhiteSpaces(ZLInputStream &stream, char &ch) {
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

	while ((PdfCharacterTypeTable[(unsigned char)ch] == PDF_CHAR_WHITESPACE) &&
				 (stream.read(&ch, 1) == 1)) {
	}
}

void PdfObject::readToken(ZLInputStream &stream, std::string &buffer, char &ch) {
	buffer.clear();
	skipWhiteSpaces(stream, ch);
	while (PdfCharacterTypeTable[(unsigned char)ch] == PDF_CHAR_REGULAR) {
		buffer += ch;
		if (stream.read(&ch, 1) != 1) {
			break;
		}
	}
}

shared_ptr<PdfObject> PdfObject::readObject(ZLInputStream &stream, char &ch) {
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
			std::string token;
			readToken(stream, token, ch);
			if (token == "stream") {
				shared_ptr<PdfObject> d = dictionary;
				return new PdfStreamObject(*dictionary, stream);
			} else {
				return dictionary;
			}
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
				skipWhiteSpaces(stream, ch);
				if (ch == 'R') {
					const int size = array->size();
					if ((size >= 2) &&
							((*array)[size - 1]->type() == PdfObject::INTEGER_NUMBER) &&
							((*array)[size - 2]->type() == PdfObject::INTEGER_NUMBER)) {
						const int generation = ((PdfIntegerObject&)*array->popLast()).value();
						const int number = ((PdfIntegerObject&)*array->popLast()).value();
						array->addObject(new PdfObjectReference(number, generation));
						ch = 0;
					}
				}
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
