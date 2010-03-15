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

#ifndef __ATOMMETADATA_H__
#define __ATOMMETADATA_H__

#include "ATOMConstructs.h"


class ATOMConstants {

private:
	ATOMConstants();

public:
	static const std::string TYPE_TEXT;
	static const std::string TYPE_HTML;
	static const std::string TYPE_XHTML;
	static const std::string TYPE_DEFAULT;
	
	static const std::string REL_ALTERNATE;
	static const std::string REL_RELATED;
	static const std::string REL_SELF;
	static const std::string REL_ENCLOSURE;
	static const std::string REL_VIA;
};





class ATOMAuthor : public ATOMPersonConstruct {

public:
	ATOMAuthor();
	ATOMAuthor(const std::string &name);
};



class ATOMCategory : public ATOMCommonAttributes {

public:
	static const std::string TERM;
	static const std::string SCHEME;
	static const std::string LABEL;

public:
	ATOMCategory();
	ATOMCategory(const std::string &termStr);

public:
	std::string &term() { return attributes()[TERM]; }
	std::string &scheme() { return attributes()[SCHEME]; }
	std::string &label() { return attributes()[LABEL]; }

	void readAttributes(const std::map<std::string, std::string> &attributes);
};



class ATOMContributor : public ATOMPersonConstruct {

public:
	ATOMContributor();
	ATOMContributor(const std::string &name);
};



class ATOMGenerator : public ATOMCommonAttributes {

public:
	static const std::string URI;
	static const std::string VERSION_ATT;

public:
	ATOMGenerator();
	ATOMGenerator(const std::string &text);

public:
	const std::string &text() const { return myText; }
	void setText(const std::string &text) { myText = text; }

	std::string &uri() { return attributes()[URI]; }
	std::string &version() { return attributes()[VERSION_ATT]; }

	void readAttributes(const std::map<std::string, std::string> &attributes);

private:
	std::string myText;
};



class ATOMIcon : public ATOMCommonAttributes {

public:
	ATOMIcon();
	ATOMIcon(const std::string &uri);

public:
	const std::string &uri() const { return myUri; }
	void setUri(const std::string &uri) { myUri = uri; }

private:
	std::string myUri;
};



class ATOMId : public ATOMCommonAttributes {

public:
	ATOMId();
	ATOMId(const std::string &uri);

	const ATOMId &operator = (const ATOMId &id);

	bool operator == (const ATOMId &id) const;
	bool operator != (const ATOMId &id) const;

public:
	const std::string &uri() const { return myUri; }
	void setUri(const std::string &uri) { myUri = uri; }

private:
	std::string myUri;
};

inline const ATOMId &ATOMId::operator = (const ATOMId &id) { myUri = id.myUri; return *this; }
inline bool ATOMId::operator == (const ATOMId &id) const { return myUri == id.myUri; }
inline bool ATOMId::operator != (const ATOMId &id) const { return myUri != id.myUri; }



class ATOMLink : public ATOMCommonAttributes {

public:
	static const std::string HREF;
	static const std::string REL;
	static const std::string TYPE;
	static const std::string HREFLANG;
	static const std::string TITLE;
	static const std::string LENGTH;

public:
	ATOMLink();
	ATOMLink(const std::string &hrefStr);
	ATOMLink(const std::string &hrefStr, const std::string &relStr);
	ATOMLink(const std::string &hrefStr, const std::string &relStr, const std::string &typeStr);
	ATOMLink(const std::string &hrefStr, const std::string &relStr, const std::string &typeStr, const std::string &titleStr);

public:
	std::string &href() { return attributes()[HREF]; }
	std::string &rel() { return attributes()[REL]; }
	std::string &type() { return attributes()[TYPE]; }
	std::string &hreflang() { return attributes()[HREFLANG]; }
	std::string &title() { return attributes()[TITLE]; }
	std::string &length() { return attributes()[LENGTH]; }

	void readAttributes(const std::map<std::string, std::string> &attributes);
};



class ATOMLogo : public ATOMCommonAttributes {

public:
	ATOMLogo();
	ATOMLogo(const std::string &uri);

public:
	const std::string &uri() const { return myUri; }
	void setUri(const std::string &uri) { myUri = uri; }

private:
	std::string myUri;
};


class ATOMPublished : public ATOMDateConstruct {

public:
	ATOMPublished();
	ATOMPublished(int year);
	ATOMPublished(int year, int month, int day);
	ATOMPublished(int year, int month, int day, int hour, int minutes, int seconds);
	ATOMPublished(int year, int month, int day, int hour, int minutes, int seconds, float sfract);
	ATOMPublished(int year, int month, int day, int hour, int minutes, int seconds, float sfract, int tzhour, int tzminutes);
};

class ATOMUpdated : public ATOMDateConstruct {

public:
	ATOMUpdated();
	ATOMUpdated(int year);
	ATOMUpdated(int year, int month, int day);
	ATOMUpdated(int year, int month, int day, int hour, int minutes, int seconds);
	ATOMUpdated(int year, int month, int day, int hour, int minutes, int seconds, float sfract);
	ATOMUpdated(int year, int month, int day, int hour, int minutes, int seconds, float sfract, int tzhour, int tzminutes);
};


#endif /* ATOMMETADATA */
