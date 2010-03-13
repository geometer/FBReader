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

#include "LitResAuthenticationDataParser.h"


static const std::string TAG_AUTHORIZATION_OK = "catalit-authorization-ok";
static const std::string TAG_AUTHORIZATION_FAILED = "catalit-authorization-failed";

static const std::string TAG_PURCHASE_OK = "catalit-purchase-ok";
static const std::string TAG_PURCHASE_FAILED = "catalit-purchase-failed";

static const std::string TAG_DOWNLOAD_FAILED = "catalit-download-failed";

static const std::string TAG_REGISTRATION_FAILED = "catalit-registration-failed";

static const std::string TAG_PASSWORD_RECOVERY_OK = "catalit-pass-recover-ok";
static const std::string TAG_PASSWORD_RECOVERY_FAILED = "catalit-pass-recover-failed";

LitResAuthenticationDataParser::LitResAuthenticationDataParser() {
}

void LitResAuthenticationDataParser::startElementHandler(const char *tag, const char **attributes) {
	myAttributes.clear();
	while (*attributes != 0) {
		std::string name(*attributes++);
		if (*attributes == 0) {
			break;
		}
		std::string value(*attributes++);
		myAttributes.insert(std::make_pair(name, value));
	}
	processTag(tag);	
}





LitResLoginDataParser::LitResLoginDataParser(std::string &firstName, std::string &lastName, std::string &sid) : 
	myFirstName(firstName), myLastName(lastName), mySid(sid) {
}

void LitResLoginDataParser::processTag(const std::string &tag) {
	if (TAG_AUTHORIZATION_FAILED == tag) {
		setErrorCode(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	} else if (TAG_AUTHORIZATION_OK == tag) {
		myFirstName = attributes()["first-name"];
		myLastName = attributes()["first-name"];
		mySid = attributes()["sid"];
	}
}


LitResPurchaseDataParser::LitResPurchaseDataParser(std::string &account, std::string &bookId) : 
	myAccount(account), myBookId(bookId) {
}

void LitResPurchaseDataParser::processTag(const std::string &tag) {
	if (TAG_AUTHORIZATION_FAILED == tag) {
		setErrorCode(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	} else {
		myAccount = attributes()["account"];
		myBookId = attributes()["art"];
		if (TAG_PURCHASE_OK == tag) {
			// nop
		} else if (TAG_PURCHASE_FAILED == tag) {
			const std::string &error = attributes()["error"];
			if ("1" == error) {
				setErrorCode(NetworkErrors::ERROR_PURCHASE_NOT_ENOUGH_MONEY);
			} else if ("2" == error) {
				setErrorCode(NetworkErrors::ERROR_PURCHASE_MISSING_BOOK);
			} else if ("3" == error) {
				setErrorCode(NetworkErrors::ERROR_PURCHASE_ALREADY_PURCHASED);
			} else {
				setErrorCode(NetworkErrors::ERROR_INTERNAL);
			}
		}
	}
}


/*LitResDownloadErrorDataParser::LitResDownloadErrorDataParser() {
}

void LitResDownloadErrorDataParser::processTag(const std::string &tag) {
	if (TAG_AUTHORIZATION_FAILED == tag) {
		setErrorCode(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	} else {
		if (TAG_DOWNLOAD_FAILED == tag) {
			const std::string &error = attributes()["error"];
			if ("1" == error) {
				setErrorCode(NetworkErrors::ERROR_BOOK_NOT_PURCHASED);
			} else if ("2" == error) {
				setErrorCode(NetworkErrors::ERROR_DOWNLOAD_LIMIT_EXCEEDED);
			} else {
				setErrorCode(NetworkErrors::ERROR_INTERNAL);
			}
		}
	}
}*/


LitResRegisterUserDataParser::LitResRegisterUserDataParser(std::string &sid) : mySid(sid) {
}

void LitResRegisterUserDataParser::processTag(const std::string &tag) {
	if (TAG_REGISTRATION_FAILED == tag) {
		const std::string &error = attributes()["error"];
		if ("1" == error) {
			setErrorCode(NetworkErrors::ERROR_LOGIN_ALREADY_TAKEN);
		} else if ("2" == error) {
			setErrorCode(NetworkErrors::ERROR_LOGIN_WAS_NOT_SPECIFIED);
		} else if ("3" == error) {
			setErrorCode(NetworkErrors::ERROR_PASSWORD_WAS_NOT_SPECIFIED);
		} else if ("4" == error) {
			setErrorCode(NetworkErrors::ERROR_INVALID_EMAIL);
		} else if ("5" == error) {
			setErrorCode(NetworkErrors::ERROR_TOO_MANY_REGISTRATIONS);
		} else {
			setErrorCode(NetworkErrors::ERROR_INTERNAL);
		}		
	} else if (TAG_AUTHORIZATION_OK == tag) {
		mySid = attributes()["sid"];
	}
}


void LitResPasswordRecoveryDataParser::processTag(const std::string &tag) {
	if (TAG_PASSWORD_RECOVERY_FAILED == tag) {
		const std::string &error = attributes()["error"];
		if ("1" == error) {
			setErrorCode(NetworkErrors::ERROR_NO_USER_EMAIL);
		} else if ("2" == error) {
			setErrorCode(NetworkErrors::ERROR_EMAIL_WAS_NOT_SPECIFIED);
		} else {
			setErrorCode(NetworkErrors::ERROR_INTERNAL);
		}		
	} else if (TAG_PASSWORD_RECOVERY_OK == tag) {
		// NOP
	}
}
