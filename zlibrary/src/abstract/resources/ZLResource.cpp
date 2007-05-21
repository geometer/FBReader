/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005, 2006 Mikhail Sobolev <mss@mawhrin.net>
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

#include <map>
#include <stack>

#include <shared_ptr.h>
#include <ZLXMLReader.h>
#include <ZLApplication.h>

#include "ZLResource.h"

class ZLTreeResource : public ZLResource {

public:
	static shared_ptr<ZLTreeResource> ourRoot;

public:
	static void buildTree();

protected:
	ZLTreeResource();
	bool hasValue() const;
	const std::string &value() const;

public:
	const ZLResource &child(const ZLResourceKey &key) const;

private:
	std::map<std::string,shared_ptr<ZLTreeResource> > myChildren;

friend class ZLResourceTreeReader;
};

class ZLTreeResourceWithValue : public ZLTreeResource {

private:
	ZLTreeResourceWithValue(const std::string &value);
	bool hasValue() const;
	const std::string &value() const;

private:
	const std::string myValue;

friend class ZLResourceTreeReader;
};

class ZLResourceTreeReader : public ZLXMLReader {

public:
	shared_ptr<ZLTreeResource> root();

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);

private:
	shared_ptr<ZLTreeResource> myRoot;
	std::stack<shared_ptr<ZLTreeResource> > myStack;
};

class ZLMissingResource : public ZLResource {

public:
	static const std::string ourValue;

private:
	static shared_ptr<ZLMissingResource> ourInstance;

private:
	ZLMissingResource();

public:
	static const ZLMissingResource &instance();

private:
	bool hasValue() const;
	const std::string &value() const;
	const ZLMissingResource &child(const ZLResourceKey &key) const;
};

shared_ptr<ZLTreeResource> ZLTreeResource::ourRoot;
shared_ptr<ZLMissingResource> ZLMissingResource::ourInstance;
const std::string ZLMissingResource::ourValue = "??????";

const ZLResource &ZLResource::resource(const ZLResourceKey &key) {
	ZLTreeResource::buildTree();
	if (ZLTreeResource::ourRoot.isNull()) {
		return ZLMissingResource::instance();
	}
	return ZLTreeResource::ourRoot->child(key);
}

ZLResource::ZLResource() {
}

ZLResource::~ZLResource() {
}

void ZLTreeResource::buildTree() {
	if (ourRoot.isNull()) {
		ZLResourceTreeReader reader;
		reader.readDocument(ZLApplication::ApplicationDirectory() + ZLApplication::FileNameDelimiter + "resources/en.xml");
		ourRoot = reader.root();
	}
}

ZLTreeResource::ZLTreeResource() {
}

bool ZLTreeResource::hasValue() const {
	return false;
}

const std::string &ZLTreeResource::value() const {
	return ZLMissingResource::ourValue;
}

const ZLResource &ZLTreeResource::child(const ZLResourceKey &key) const {
	std::map<std::string,shared_ptr<ZLTreeResource> >::const_iterator it = myChildren.find(key.Name);
	if (it != myChildren.end()) {
		return *it->second;
	} else {
		return ZLMissingResource::instance();
	}
}

ZLTreeResourceWithValue::ZLTreeResourceWithValue(const std::string &value) : myValue(value) {
}

bool ZLTreeResourceWithValue::hasValue() const {
	return true;
}

const std::string &ZLTreeResourceWithValue::value() const {
	return myValue;
}

const ZLMissingResource &ZLMissingResource::instance() {
	if (ourInstance.isNull()) {
		ourInstance = new ZLMissingResource();
	}
	return *ourInstance;
}

ZLMissingResource::ZLMissingResource() {
}

bool ZLMissingResource::hasValue() const {
	return false;
}

const std::string &ZLMissingResource::value() const {
	return ourValue;
}

const ZLMissingResource &ZLMissingResource::child(const ZLResourceKey&) const {
	return *this;
}

void ZLResourceTreeReader::startElementHandler(const char *tag, const char **attributes) {
	if (myRoot.isNull()) {
		myRoot = new ZLTreeResource();
		myStack.push(myRoot);
	} else {
		shared_ptr<ZLTreeResource> node;
		const char *value = attributeValue(attributes, "value");
		if (value != 0) {
			node = new ZLTreeResourceWithValue(value);
		} else {
			node = new ZLTreeResource();
		}
		myStack.top()->myChildren[tag] = node;
		myStack.push(node);
	}
}

void ZLResourceTreeReader::endElementHandler(const char*) {
	myStack.pop();
}

shared_ptr<ZLTreeResource> ZLResourceTreeReader::root() {
	return myRoot;
}
