/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include "ZLEncodingConverter.h"
#include "DummyEncodingConverter.h"
#include "IConvEncodingConverter.h"
#include "MyEncodingConverter.h"

ZLEncodingConverterProvider::ZLEncodingConverterProvider() {
}

ZLEncodingConverterProvider::~ZLEncodingConverterProvider() {
}

bool ZLEncodingConverterInfo::canCreateConverter() const {
	for (std::vector<std::string>::const_iterator it = myAliases.begin(); it != myAliases.end(); ++it) {
		{
			DummyEncodingConverterProvider provider;
			if (provider.providesConverter(*it)) {
				return true;
			}
		}

		{
			MyEncodingConverterProvider provider;
			if (provider.providesConverter(*it)) {
				return true;
			}
		}

		{
			IConvEncodingConverterProvider provider;
			if (provider.providesConverter(*it)) {
				return true;
			}
		}
	}

	return false;
}

shared_ptr<ZLEncodingConverter> ZLEncodingConverterInfo::createConverter() const {
	for (std::vector<std::string>::const_iterator it = myAliases.begin(); it != myAliases.end(); ++it) {
		{
			DummyEncodingConverterProvider provider;
			if (provider.providesConverter(*it)) {
				return provider.createConverter(*it);
			}
		}

		{
			MyEncodingConverterProvider provider;
			if (provider.providesConverter(*it)) {
				return provider.createConverter(*it);
			}
		}

		{
			IConvEncodingConverterProvider provider;
			if (provider.providesConverter(*it)) {
				return provider.createConverter(*it);
			}
		}
	}

	return DummyEncodingConverterProvider().createConverter("");
}

ZLEncodingConverter::ZLEncodingConverter() {
}

ZLEncodingConverter::~ZLEncodingConverter() {
}

void ZLEncodingConverter::reset() {
}

void ZLEncodingConverter::convert(std::string &dst, const std::string &src) {
	convert(dst, src.data(), src.data() + src.length());
}

ZLEncodingConverterInfo::ZLEncodingConverterInfo(const std::string &name, const std::string &region) : myName(name), myVisibleName(region + " (" + name + ")") {
	addAlias(myName);
}

const std::string &ZLEncodingConverterInfo::name() const {
	return myName;
}

const std::string &ZLEncodingConverterInfo::visibleName() const {
	return myVisibleName;
}

void ZLEncodingConverterInfo::addAlias(const std::string &alias) {
	myAliases.push_back(alias);
}
