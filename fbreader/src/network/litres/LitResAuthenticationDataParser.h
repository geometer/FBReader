/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __LITRESAUTHENTICATIONDATAPARSER_H__
#define __LITRESAUTHENTICATIONDATAPARSER_H__

#include <ZLXMLReader.h>

#include "../NetworkErrors.h"


class LitResAuthenticationDataParser : public ZLXMLReader {

public:
	LitResAuthenticationDataParser();

	const std::string &error() const;
	void reset();

private:
	void startElementHandler(const char *tag, const char **attributes);

protected:
	void setErrorMessage(const std::string &msg);
	std::map<std::string, std::string> &attributes();

	virtual void processTag(const std::string &tag) = 0;

private:
	std::map<std::string, std::string> myAttributes;
	std::string myErrorMsg;
};

inline const std::string &LitResAuthenticationDataParser::error() const { return myErrorMsg; }
inline void LitResAuthenticationDataParser::setErrorMessage(const std::string &msg) { myErrorMsg = NetworkErrors::errorMessage(msg); } 
inline std::map<std::string, std::string> &LitResAuthenticationDataParser::attributes() { return myAttributes; }


class LitResLoginDataParser : public LitResAuthenticationDataParser {

public:
	LitResLoginDataParser(std::string &firstName, std::string &lastName, std::string &sid);

private:
	void processTag(const std::string &tag);

private:
	std::string &myFirstName;
	std::string &myLastName;
	std::string &mySid;
};


class LitResPurchaseDataParser : public LitResAuthenticationDataParser {

public:
	LitResPurchaseDataParser(std::string &account, std::string &bookId);

private:
	void processTag(const std::string &tag);

private:
	std::string &myAccount;
	std::string &myBookId;
};


class LitResDownloadErrorDataParser : public LitResAuthenticationDataParser {

public:
	LitResDownloadErrorDataParser();

private:
	void processTag(const std::string &tag);

};

class LitResRegisterUserDataParser : public LitResAuthenticationDataParser {

public:
	LitResRegisterUserDataParser(std::string &sid);

private:
	void processTag(const std::string &tag);

private:
	std::string &mySid;
};

class LitResPasswordRecoveryDataParser : public LitResAuthenticationDataParser {

private:
	void processTag(const std::string &tag);
};

#endif /* __LITRESAUTHENTICATIONDATAPARSER_H__ */
