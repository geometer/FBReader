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

#include <map>
#include <stack>

#include <shared_ptr.h>
#include <ZLXMLReader.h>
#include <ZLibrary.h>

#include "ZLResource.h"

class ZLTreeResourcePtr;

class ZLTreeResource : public ZLResource {

public:
	static shared_ptr<ZLTreeResource> ourRoot;

public:
	static void buildTree();
	static void loadData(const std::string &language);

private:
	ZLTreeResource(const std::string &name);
	ZLTreeResource(const std::string &name, const std::string &value);
	void setValue(const std::string &value);
	bool hasValue() const;
	const std::string &value() const;

public:
	const ZLResource &operator [] (const std::string &key) const;

private:
	bool myHasValue;
	std::string myValue;
	std::map<std::string,shared_ptr<ZLTreeResource> > myChildren;

friend class ZLResourceTreeReader;
};

class ZLResourceTreeReader : public ZLXMLReader {

public:
	ZLResourceTreeReader(shared_ptr<ZLTreeResource> root);

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);

private:
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
	const ZLMissingResource &operator [] (const std::string &key) const;
};

shared_ptr<ZLTreeResource> ZLTreeResource::ourRoot;
shared_ptr<ZLMissingResource> ZLMissingResource::ourInstance;
const std::string ZLMissingResource::ourValue = "????????";

const ZLResource &ZLResource::operator [] (const ZLResourceKey &key) const {
	return (*this)[key.Name];
}

const ZLResource &ZLResource::resource(const std::string &key) {
	ZLTreeResource::buildTree();
	if (ZLTreeResource::ourRoot.isNull()) {
		return ZLMissingResource::instance();
	}
	return (*ZLTreeResource::ourRoot)[key];
}

const ZLResource &ZLResource::resource(const ZLResourceKey &key) {
	return resource(key.Name);
}

ZLResource::ZLResource(const std::string &name) : myName(name) {
}

ZLResource::~ZLResource() {
}

const std::string &ZLResource::name() const {
	return myName;
}

void ZLTreeResource::loadData(const std::string &language) {
	std::string filePath = ZLibrary::FileNameDelimiter + "resources" + ZLibrary::FileNameDelimiter + language + ".xml";
	ZLResourceTreeReader(ourRoot).readDocument(ZLibrary::ZLibraryDirectory() + filePath);
	ZLResourceTreeReader(ourRoot).readDocument(ZLibrary::ApplicationDirectory() + filePath);
}

void ZLTreeResource::buildTree() {
	if (ourRoot.isNull()) {
		ourRoot = new ZLTreeResource(std::string());
		loadData("en");
		const std::string language = ZLibrary::Language();
		if (language != "en") {
			loadData(language);
		}
	}
}

ZLTreeResource::ZLTreeResource(const std::string &name) : ZLResource(name), myHasValue(false) {
}

ZLTreeResource::ZLTreeResource(const std::string &name, const std::string &value) : ZLResource(name), myHasValue(true), myValue(value) {
}

void ZLTreeResource::setValue(const std::string &value) {
	myHasValue = true;
	myValue = value;
}

bool ZLTreeResource::hasValue() const {
	return myHasValue;
}

const std::string &ZLTreeResource::value() const {
	return myHasValue ? myValue : ZLMissingResource::ourValue;
}

const ZLResource &ZLTreeResource::operator [] (const std::string &key) const {
	std::map<std::string,shared_ptr<ZLTreeResource> >::const_iterator it = myChildren.find(key);
	if (it != myChildren.end()) {
		return *it->second;
	} else {
		return ZLMissingResource::instance();
	}
}

const ZLMissingResource &ZLMissingResource::instance() {
	if (ourInstance.isNull()) {
		ourInstance = new ZLMissingResource();
	}
	return *ourInstance;
}

ZLMissingResource::ZLMissingResource() : ZLResource(ourValue) {
}

bool ZLMissingResource::hasValue() const {
	return false;
}

const std::string &ZLMissingResource::value() const {
	return ourValue;
}

const ZLMissingResource &ZLMissingResource::operator [] (const std::string&) const {
	return *this;
}

ZLResourceTreeReader::ZLResourceTreeReader(shared_ptr<ZLTreeResource> root) {
	myStack.push(root);
}

static const std::string NODE = "node";

void ZLResourceTreeReader::startElementHandler(const char *tag, const char **attributes) {
	if (!myStack.empty() && (NODE == tag)) {
		const char *name = attributeValue(attributes, "name");
		if (name != 0) {
			const std::string sName = name;
			const char *value = attributeValue(attributes, "value");
			shared_ptr<ZLTreeResource> node = myStack.top()->myChildren[sName];
			if (node.isNull()) {
				if (value != 0) {
					node = new ZLTreeResource(sName, value);
				} else {
					node = new ZLTreeResource(sName);
				}
				myStack.top()->myChildren[sName] = node;
			} else {
				if (value != 0) {
					node->setValue(value);
				}
			}
			myStack.push(node);
		}
	}
}

void ZLResourceTreeReader::endElementHandler(const char *tag) {
	if (!myStack.empty() && (NODE == tag)) {
		myStack.pop();
	}
}
