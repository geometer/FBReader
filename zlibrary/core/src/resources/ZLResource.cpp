/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLFile.h>
#include <ZLXMLReader.h>
#include <ZLibrary.h>
#include <ZLStringUtil.h>

#include "ZLResource.h"

class ZLTreeResourcePtr;

class ZLTreeResource : public ZLResource {

private:
	class Condition {
	public:
		virtual bool accepts(int number) = 0;
	};

	class ValueCondition : public Condition {
	public:
		ValueCondition(int value);
		bool accepts(int number);

	private:
		const int myValue;
	};

	class RangeCondition : public Condition {
	public:
		RangeCondition(int min, int max);
		bool accepts(int number);

	private:
		const int myMin;
		const int myMax;
	};

	class ModRangeCondition : public Condition {
	public:
		ModRangeCondition(int min, int max, int base);
		bool accepts(int number);

	private:
		const int myMin;
		const int myMax;
		const int myBase;
	};

	class ModCondition : public Condition {
	public:
		ModCondition(int mod, int base);
		bool accepts(int number);

	private:
		const int myMod;
		const int myBase;
	};

	static shared_ptr<Condition> parseCondition(std::string description);

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
	const std::string &value(int number) const;

public:
	const ZLResource &operator [] (const std::string &key) const;

private:
	bool myHasValue;
	std::string myValue;
	std::map<std::string,shared_ptr<ZLTreeResource> > myChildren;

	typedef std::map<shared_ptr<Condition>,std::string> Conditionals;
	Conditionals myConditionalValues;

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
	static const ZLMissingResource &Instance();

private:
	bool hasValue() const;
	const std::string &value() const;
	const std::string &value(int number) const;
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
		return ZLMissingResource::Instance();
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
	ZLResourceTreeReader(ourRoot).readDocument(ZLFile(
		ZLibrary::ZLibraryDirectory() + filePath
	));
	ZLResourceTreeReader(ourRoot).readDocument(ZLFile(
		ZLibrary::ApplicationDirectory() + filePath
	));
}

shared_ptr<ZLTreeResource::Condition> ZLTreeResource::parseCondition(std::string description) {
	std::vector<std::string> parts = ZLStringUtil::split(description, ZLStringUtil::SPACE);
	if (parts.empty()) {
		return 0;
	}
	std::string condition = parts.at(0);
	if (condition == "range" && parts.size() == 3) {
		return new RangeCondition(ZLStringUtil::stringToInteger(parts.at(1)), ZLStringUtil::stringToInteger(parts.at(2)));
	} else if (condition == "mod" && parts.size() == 3) {
		return new ModCondition(ZLStringUtil::stringToInteger(parts.at(1)), ZLStringUtil::stringToInteger(parts.at(2)));
	} else if (condition == "modrange" && parts.size() == 4) {
		return new ModRangeCondition(
			ZLStringUtil::stringToInteger(parts.at(1)),
			ZLStringUtil::stringToInteger(parts.at(2)),
			ZLStringUtil::stringToInteger(parts.at(3))
		);
	} else if (condition == "value" && parts.size() == 2) {
		return new ValueCondition(ZLStringUtil::stringToInteger(parts.at(1)));
	}
	return 0;
}

void ZLTreeResource::buildTree() {
	if (ourRoot.isNull()) {
		ourRoot = new ZLTreeResource(std::string());
		loadData("en");
		const std::string language = ZLibrary::Language();
		if (language != "en") {
			loadData(language);
		}
		const std::string country = ZLibrary::Country();
		if (!country.empty()) {
			loadData(language + '_' + country);
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

const std::string &ZLTreeResource::value(int number) const {
	if (!myConditionalValues.empty()) {
		for (Conditionals::const_iterator it = myConditionalValues.begin(); it != myConditionalValues.end(); ++it) {
			if ((*it).first->accepts(number)) {
				return (*it).second;
			}
		}
	}
	return myHasValue ? myValue : ZLMissingResource::ourValue;
}

const ZLResource &ZLTreeResource::operator [] (const std::string &key) const {
	std::map<std::string,shared_ptr<ZLTreeResource> >::const_iterator it = myChildren.find(key);
	if (it != myChildren.end()) {
		return *it->second;
	} else {
		return ZLMissingResource::Instance();
	}
}

const ZLMissingResource &ZLMissingResource::Instance() {
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

const std::string &ZLMissingResource::value(int /*number*/) const {
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
		const char *condition = attributeValue(attributes, "condition");
		const char *value = attributeValue(attributes, "value");
		shared_ptr<ZLTreeResource> peek = myStack.top();
		if (name != 0) {
			const std::string sName = name;
			shared_ptr<ZLTreeResource> node = peek->myChildren[sName];
			if (node.isNull()) {
				if (value != 0) {
					node = new ZLTreeResource(sName, value);
				} else {
					node = new ZLTreeResource(sName);
				}
				peek->myChildren[sName] = node;
			} else {
				if (value != 0) {
					node->setValue(value);
					node->myConditionalValues.clear();
				}
			}
			myStack.push(node);
		} else if (condition != 0 && value != 0) {
			shared_ptr<ZLTreeResource::Condition> compiled = ZLTreeResource::parseCondition(condition);
			if (!compiled.isNull()) {
				peek->myConditionalValues[compiled] = value;
			}
			myStack.push(peek);
		}
	}
}

void ZLResourceTreeReader::endElementHandler(const char *tag) {
	if (!myStack.empty() && (NODE == tag)) {
		myStack.pop();
	}
}

ZLTreeResource::ValueCondition::ValueCondition(int value) : myValue(value) {}

bool ZLTreeResource::ValueCondition::accepts(int number) {
	return myValue == number;
}

ZLTreeResource::RangeCondition::RangeCondition(int min, int max) : myMin(min), myMax(max) {}

bool ZLTreeResource::RangeCondition::accepts(int number) {
	return myMin <= number && number <= myMax;
}

ZLTreeResource::ModRangeCondition::ModRangeCondition(int min, int max, int base) : myMin(min), myMax(max), myBase(base) {}

bool ZLTreeResource::ModRangeCondition::accepts(int number) {
	number = number % myBase;
	return myMin <= number && number <= myMax;
}

ZLTreeResource::ModCondition::ModCondition(int mod, int base) : myMod(mod), myBase(base) {}

bool ZLTreeResource::ModCondition::accepts(int number) {
	return number % myBase == myMod;
}
