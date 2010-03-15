/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include "ATOMMetadata.h"


const std::string ATOMConstants::TYPE_TEXT = "text";
const std::string ATOMConstants::TYPE_HTML = "html";
const std::string ATOMConstants::TYPE_XHTML = "xhtml";

const std::string ATOMConstants::TYPE_DEFAULT = TYPE_TEXT;

const std::string ATOMConstants::REL_ALTERNATE = "alternate";
const std::string ATOMConstants::REL_RELATED = "related";
const std::string ATOMConstants::REL_SELF = "self";
const std::string ATOMConstants::REL_ENCLOSURE = "enclosure";
const std::string ATOMConstants::REL_VIA = "via";



const std::string ATOMCategory::TERM = "term";
const std::string ATOMCategory::SCHEME = "scheme";
const std::string ATOMCategory::LABEL = "label";

const std::string ATOMGenerator::URI = "uri";
const std::string ATOMGenerator::VERSION_ATT = "version";

const std::string ATOMLink::HREF = "href";
const std::string ATOMLink::REL = "rel";
const std::string ATOMLink::TYPE = "type";
const std::string ATOMLink::HREFLANG = "hreflang";
const std::string ATOMLink::TITLE = "title";
const std::string ATOMLink::LENGTH = "length";



ATOMAuthor::ATOMAuthor() {
}

ATOMAuthor::ATOMAuthor(const std::string &name) : ATOMPersonConstruct(name) {
}



ATOMCategory::ATOMCategory() {
}

ATOMCategory::ATOMCategory(const std::string &termStr) {
	term() = termStr;
}

void ATOMCategory::readAttributes(const std::map<std::string, std::string> &attributes) {
	ATOMCommonAttributes::readAttributes(attributes);
	readAttribute(TERM, attributes);
	readAttribute(SCHEME, attributes);
	readAttribute(LABEL, attributes);
}



ATOMContributor::ATOMContributor() {
}

ATOMContributor::ATOMContributor(const std::string &name) : ATOMPersonConstruct(name) {
}



ATOMGenerator::ATOMGenerator() {
}

ATOMGenerator::ATOMGenerator(const std::string &text) : myText(text) {
}

void ATOMGenerator::readAttributes(const std::map<std::string, std::string> &attributes) {
	ATOMCommonAttributes::readAttributes(attributes);
	readAttribute(URI, attributes);
	readAttribute(VERSION_ATT, attributes);
}



ATOMIcon::ATOMIcon() {
}

ATOMIcon::ATOMIcon(const std::string &uri) : myUri(uri) {
}



ATOMId::ATOMId() {
}

ATOMId::ATOMId(const std::string &uri) : myUri(uri) {
}



ATOMLink::ATOMLink() {
}

ATOMLink::ATOMLink(const std::string &hrefStr) {
	href() = hrefStr;
}

ATOMLink::ATOMLink(const std::string &hrefStr, const std::string &relStr) {
	href() = hrefStr;
	rel() = relStr;
}

ATOMLink::ATOMLink(const std::string &hrefStr, const std::string &relStr, const std::string &typeStr){
	href() = hrefStr;
	rel() = relStr;
	type() = typeStr;
}

ATOMLink::ATOMLink(const std::string &hrefStr, const std::string &relStr, const std::string &typeStr, const std::string &titleStr) {
	href() = hrefStr;
	rel() = relStr;
	type() = typeStr;
	title() = titleStr;
}

void ATOMLink::readAttributes(const std::map<std::string, std::string> &attributes) {
	ATOMCommonAttributes::readAttributes(attributes);
	readAttribute(HREF, attributes);
	readAttribute(REL, attributes);
	readAttribute(TYPE, attributes);
	readAttribute(HREFLANG, attributes);
	readAttribute(TITLE, attributes);
	readAttribute(LENGTH, attributes);
}

ATOMLogo::ATOMLogo() {
}

ATOMLogo::ATOMLogo(const std::string &uri) : myUri(uri) {
}



ATOMPublished::ATOMPublished() : ATOMDateConstruct(0) {
}

ATOMPublished::ATOMPublished(int year) : ATOMDateConstruct(year) {
}

ATOMPublished::ATOMPublished(int year, int month, int day) : ATOMDateConstruct(year, month, day) {
}

ATOMPublished::ATOMPublished(int year, int month, int day, int hour, int minutes, int seconds) : 
	ATOMDateConstruct(year, month, day, hour, minutes, seconds) {
}

ATOMPublished::ATOMPublished(int year, int month, int day, int hour, int minutes, int seconds, float sfract) : 
	ATOMDateConstruct(year, month, day, hour, minutes, seconds, sfract) {
}

ATOMPublished::ATOMPublished(int year, int month, int day, int hour, int minutes, int seconds, float sfract, int tzhour, int tzminutes) : 
	ATOMDateConstruct(year, month, day, hour, minutes, seconds, sfract, tzhour, tzminutes) {
}



ATOMUpdated::ATOMUpdated() : ATOMDateConstruct(0) {
}

ATOMUpdated::ATOMUpdated(int year) : ATOMDateConstruct(year) {
}

ATOMUpdated::ATOMUpdated(int year, int month, int day) : ATOMDateConstruct(year, month, day) {
}

ATOMUpdated::ATOMUpdated(int year, int month, int day, int hour, int minutes, int seconds) : 
	ATOMDateConstruct(year, month, day, hour, minutes, seconds) {
}

ATOMUpdated::ATOMUpdated(int year, int month, int day, int hour, int minutes, int seconds, float sfract) : 
	ATOMDateConstruct(year, month, day, hour, minutes, seconds, sfract) {
}

ATOMUpdated::ATOMUpdated(int year, int month, int day, int hour, int minutes, int seconds, float sfract, int tzhour, int tzminutes) : 
	ATOMDateConstruct(year, month, day, hour, minutes, seconds, sfract, tzhour, tzminutes) {
}


