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

#ifndef __PASSWORDRECOVERYDIALOG_H__
#define __PASSWORDRECOVERYDIALOG_H__

#include <string>

class ZLDialog;
class NetworkAuthenticationManager;

class PasswordRecoveryDialog {

private:
	PasswordRecoveryDialog(std::string &email, const std::string &errorMessage);

	ZLDialog &dialog();

	static bool runDialog(std::string &email, std::string &errorMessage);

public:
	static bool run(NetworkAuthenticationManager &mgr);

private:
	shared_ptr<ZLDialog> myDialog;
};

inline ZLDialog &PasswordRecoveryDialog::dialog() { return *myDialog; }

#endif /* __PASSWORDRECOVERYDIALOG_H__ */
