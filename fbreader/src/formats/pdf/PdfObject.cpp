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

#include <iostream>

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
