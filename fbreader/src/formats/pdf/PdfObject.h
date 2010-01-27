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

#ifndef __PDFOBJECT_H__
#define __PDFOBJECT_H__

#include <string>
#include <vector>
#include <map>

#include <shared_ptr.h>

class ZLInputStream;

class PdfObject {

public:
	static shared_ptr<PdfObject> readObject(ZLInputStream &stream, char &ch);
	static void readToken(ZLInputStream &stream, std::string &buffer, char &ch);

protected:
	static void skipWhiteSpaces(ZLInputStream &stream, char &ch);

public:
	enum Type {
		BOOLEAN,
		INTEGER_NUMBER,
		REAL_NUMBER,
		STRING,
		NAME,
		ARRAY,
		DICTIONARY,
		STREAM,
		NIL,
		REFERENCE
	};

	virtual ~PdfObject();

	virtual Type type() const = 0;
};

class PdfBooleanObject : public PdfObject {

public:
	static shared_ptr<PdfObject> TRUE();
	static shared_ptr<PdfObject> FALSE();

private:
	PdfBooleanObject(bool value);

public:
	bool value() const;

private:
	Type type() const;

private:
	const bool myValue;
};

class PdfIntegerObject : public PdfObject {

public:
	static shared_ptr<PdfObject> integerObject(int value);

private:
	PdfIntegerObject(int value);

public:
	int value() const;

private:
	Type type() const;

private:
	const int myValue;
};

class PdfStringObject : public PdfObject {

private:
	PdfStringObject(const std::string &value);

private:
	Type type() const;

private:
	std::string myValue;

friend shared_ptr<PdfObject> PdfObject::readObject(ZLInputStream &stream, char &ch);
};

class PdfNameObject : public PdfObject {

public:
	static shared_ptr<PdfObject> nameObject(const std::string &id);

private:
	static std::map<std::string,shared_ptr<PdfObject> > ourObjectMap;

private:
	PdfNameObject();

private:
	Type type() const;
};

class PdfDictionaryObject : public PdfObject {

private:
	PdfDictionaryObject();
	void setObject(shared_ptr<PdfObject> id, shared_ptr<PdfObject> object);

public:
	shared_ptr<PdfObject> operator [] (shared_ptr<PdfObject> id) const;
	shared_ptr<PdfObject> operator [] (const std::string &id) const;

private:
	Type type() const;

private:
	std::map<shared_ptr<PdfObject>,shared_ptr<PdfObject> > myMap;

friend shared_ptr<PdfObject> PdfObject::readObject(ZLInputStream &stream, char &ch);
};

class PdfStreamObject : public PdfObject {

private:
	PdfStreamObject(const PdfDictionaryObject &dictionary, ZLInputStream &dataStream);

private:
	Type type() const;

private:
	std::string myData;
	/*
	enum EncodingType {
		UNKNOWN,
		FLATE,
	};
	std::vector<EncodingType> myFilters;
	*/

friend shared_ptr<PdfObject> PdfObject::readObject(ZLInputStream &stream, char &ch);
};

class PdfArrayObject : public PdfObject {

private:
	PdfArrayObject();
	void addObject(shared_ptr<PdfObject> object);
	shared_ptr<PdfObject> popLast();

public:
	int size() const;
	shared_ptr<PdfObject> operator [] (int index) const;

private:
	Type type() const;

private:
	std::vector<shared_ptr<PdfObject> > myVector;

friend shared_ptr<PdfObject> PdfObject::readObject(ZLInputStream &stream, char &ch);
};

class PdfObjectReference : public PdfObject {

public:
	PdfObjectReference(int number, int generation);

	int number() const;
	int generation() const;

private:
	Type type() const;

private:
	const int myNumber;
	const int myGeneration;
};

#endif /* __PDFOBJECT_H__ */
