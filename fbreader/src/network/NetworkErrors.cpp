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

#include <ZLResource.h>
#include <ZLStringUtil.h>

#include "NetworkErrors.h"

const std::string NetworkErrors::ERROR_AUTHENTICATION_FAILED = "authenticationFailed";
const std::string NetworkErrors::ERROR_INTERNAL = "internalError";
const std::string NetworkErrors::ERROR_PURCHASE_NOT_ENOUGH_MONEY = "purchaseNotEnoughMoney";
const std::string NetworkErrors::ERROR_PURCHASE_MISSING_BOOK = "purchaseMissingBook";
const std::string NetworkErrors::ERROR_PURCHASE_ALREADY_PURCHASED = "purchaseAlreadyPurchased";
const std::string NetworkErrors::ERROR_BOOK_NOT_PURCHASED = "bookNotPurchased";
const std::string NetworkErrors::ERROR_DOWNLOAD_LIMIT_EXCEEDED = "downloadLimitExceeded";

const std::string NetworkErrors::ERROR_LOGIN_ALREADY_TAKEN = "loginAlreadyTaken";
const std::string NetworkErrors::ERROR_LOGIN_WAS_NOT_SPECIFIED = "loginNotSpecified";
const std::string NetworkErrors::ERROR_PASSWORD_WAS_NOT_SPECIFIED = "passwordNotSpecified";
const std::string NetworkErrors::ERROR_EMAIL_WAS_NOT_SPECIFIED = "emailNotSpecified";
const std::string NetworkErrors::ERROR_INVALID_EMAIL = "invalidEMail";
const std::string NetworkErrors::ERROR_TOO_MANY_REGISTRATIONS = "tooManyRegistrations";

const std::string NetworkErrors::ERROR_NO_USER_EMAIL = "noUserEmail";

const std::string NetworkErrors::ERROR_SOMETHING_WRONG = "somethingWrongMessage";
const std::string NetworkErrors::ERROR_UNSUPPORTED_OPERATION = "unsupportedOperation";


std::string NetworkErrors::errorMessage(const std::string &error) {
	if (error.empty()) {
		return "";
	}
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
	return errorResource[error].value();
}

std::string NetworkErrors::errorMessage(const std::string &error, const std::string &arg0) {
	if (error.empty()) {
		return "";
	}
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
	return ZLStringUtil::printf(errorResource[error].value(), arg0);
}

