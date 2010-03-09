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

#include <ZLDialog.h>
#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>

#include <optionEntries/ZLSimpleOptionEntry.h>

#include "AuthenticationDialog.h"
#include "NetworkOperationRunnable.h"
#include "../network/UserList.h"
#include "../network/authentication/NetworkAuthenticationManager.h"

class UserNamesEntry : public ZLComboOptionEntry {

public:
	UserNamesEntry(UserList &userList, ZLStringOption &userNameOption);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	UserList &myUserList;
	ZLStringOption &myUserNameOption;
};

UserNamesEntry::UserNamesEntry(UserList &userList, ZLStringOption &userNameOption) : 
	ZLComboOptionEntry(true), myUserList(userList), myUserNameOption(userNameOption) {
}

const std::string &UserNamesEntry::initialValue() const {
	return myUserNameOption.value();
}

const std::vector<std::string> &UserNamesEntry::values() const {
	return myUserList.users();
}

void UserNamesEntry::onAccept(const std::string &value) {
	myUserList.addUser(value);
	myUserNameOption.setValue(value);
}


class PasswordOptionEntry : public ZLPasswordOptionEntry {

public:
	PasswordOptionEntry(std::string &password);

	virtual const std::string &initialValue() const;
	virtual void onAccept(const std::string &value);

private:
	std::string &myPassword;
};

PasswordOptionEntry::PasswordOptionEntry(std::string &password) : myPassword(password) {
}

const std::string &PasswordOptionEntry::initialValue() const {
	static const std::string _empty;
	return _empty;
}

void PasswordOptionEntry::onAccept(const std::string &value) {
	myPassword = value;
}

AuthenticationDialog::AuthenticationDialog(NetworkAuthenticationManager &mgr, UserList &userList, const std::string &errorMessage, std::string &password) :
	myUserList(userList) {
	myDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("AuthenticationDialog"));

	if (!errorMessage.empty()) {
		myDialog->addOption("", "", new ZLSimpleStaticTextOptionEntry(errorMessage));
	}

	myDialog->addOption(ZLResourceKey("login"), new UserNamesEntry(myUserList, mgr.UserNameOption));
	myDialog->addOption(ZLResourceKey("password"), new PasswordOptionEntry(password));
	if (mgr.skipIPSupported()) {
		myDialog->addOption(ZLResourceKey("skipIP"), mgr.SkipIPOption);
	}

	myDialog->addButton(ZLDialogManager::OK_BUTTON, true);
	myDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
}

bool AuthenticationDialog::runDialog(NetworkAuthenticationManager &mgr, UserList &userList, const std::string &errorMessage, std::string &password) {
	AuthenticationDialog dlg(mgr, userList, errorMessage, password);
	if (dlg.dialog().run()) {
		dlg.dialog().acceptValues();
		return true;
	}
	return false;
}

bool AuthenticationDialog::run(NetworkAuthenticationManager &mgr) {
	std::string errorMessage;
	UserList userList;
	while (true) {
		std::string password;
		if (!runDialog(mgr, userList, errorMessage, password)) {
			LogOutRunnable logout(mgr);
			logout.executeWithUI();
			return false;
		}

		if (mgr.UserNameOption.value().empty()) {
			const ZLResource &resource = ZLResource::resource("dialog")["AuthenticationDialog"];
			errorMessage = resource["loginIsEmpty"].value();
			continue;
		}

		AuthoriseRunnable authoriser(mgr, password);
		authoriser.executeWithUI();
		if (authoriser.hasErrors()) {
			errorMessage = authoriser.errorMessage();
			LogOutRunnable logout(mgr);
			logout.executeWithUI();
			continue;
		}
		if (mgr.needsInitialization()) {
			InitializeAuthenticationManagerRunnable initializer(mgr);
			initializer.executeWithUI();
			if (initializer.hasErrors()) {
				errorMessage = initializer.errorMessage();
				LogOutRunnable logout(mgr);
				logout.executeWithUI();
				continue;
			}
		}
		userList.saveUser(mgr.currentUserName());
		return true;
	}
}
