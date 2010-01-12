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

#ifndef __NETWORKERRORS_H__
#define __NETWORKERRORS_H__

#include <string>

class NetworkErrors {

private: // disable instantiation
	NetworkErrors();

public:
	static const std::string ERROR_AUTHENTICATION_FAILED;
	static const std::string ERROR_INTERNAL;
	static const std::string ERROR_PURCHASE_NOT_ENOUGH_MONEY;
	static const std::string ERROR_PURCHASE_MISSING_BOOK;
	static const std::string ERROR_PURCHASE_ALREADY_PURCHASED;
	static const std::string ERROR_BOOK_NOT_PURCHASED;
	static const std::string ERROR_DOWNLOAD_LIMIT_EXCEEDED;

	static const std::string ERROR_LOGIN_ALREADY_TAKEN;
	static const std::string ERROR_LOGIN_WAS_NOT_SPECIFIED;
	static const std::string ERROR_PASSWORD_WAS_NOT_SPECIFIED;
	static const std::string ERROR_EMAIL_WAS_NOT_SPECIFIED;
	static const std::string ERROR_INVALID_EMAIL;
	static const std::string ERROR_TOO_MANY_REGISTRATIONS;

	static const std::string ERROR_NO_USER_EMAIL;

	static const std::string ERROR_SOMETHING_WRONG;
	static const std::string ERROR_UNSUPPORTED_OPERATION;

public:
	static std::string errorMessage(const std::string &error);
	static std::string errorMessage(const std::string &error, const std::string &arg0);
};

#endif /* __NETWORKERRORS_H__ */
