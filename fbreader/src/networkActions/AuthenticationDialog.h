/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __AUTHENTICATIONDIALOG_H__
#define __AUTHENTICATIONDIALOG_H__

#include <string>
#include <shared_ptr.h>
#include "../network/UserList.h"

class ZLDialog;
class NetworkAuthenticationManager;
class AuthenticationDialogListener;

class AuthenticationListener {
public:
	virtual ~AuthenticationListener();
	
	virtual void onAuthenticationFinished(bool result) = 0;
};

class AuthenticationDialog {

private:
	AuthenticationDialog(NetworkAuthenticationManager &mgr, UserList &userList, const std::string &errorMessage, std::string &password);

	ZLDialog &dialog();

	static bool runDialog(NetworkAuthenticationManager &mgr, UserList &userList, const std::string &errorMessage, std::string &password);

public:
	static bool run(NetworkAuthenticationManager &mgr, shared_ptr<AuthenticationListener> listener = 0);

private:
	shared_ptr<ZLDialog> myDialog;
	UserList &myUserList;
	friend class AuthenticationDialogListener;
};

inline ZLDialog &AuthenticationDialog::dialog() { return *myDialog; }

#endif /* __AUTHENTICATIONDIALOG_H__ */
