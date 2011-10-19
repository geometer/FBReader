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
#include <ZLTimeManager.h>

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

class AuthenticationHelper : public ZLRunnable {
public:
	AuthenticationHelper(shared_ptr<ZLExecutionData::Listener> listener, const std::string &error)
	    : myListener(listener), myError(error) {
	}
	
	virtual void run() {
		myListener->finished(myError);
	}
	
private:
	shared_ptr<ZLExecutionData::Listener> myListener;
	const std::string myError;
};

class AuthenticationDialogListener : public ZLExecutionData::Listener {
public:
	AuthenticationDialogListener(NetworkAuthenticationManager &mgr, shared_ptr<ZLExecutionData::Listener> listener);
	
	void finish(bool result);
	virtual void showPercent(int ready, int full);
	virtual void finished(const std::string &error = std::string());
	
private:
	void onFinished(ZLUserDataHolder &data, const std::string &error);
	void restart(const std::string &error);
	
	enum State { LogOut, LogOutFinal, Authorisation, Initialization };
	shared_ptr<ZLExecutionData::Listener> myHolder;
	NetworkAuthenticationManager &myManager;
	shared_ptr<ZLExecutionData::Listener> myListener;
	std::string myPassword;
	UserList *myUserList;
	std::string myError;
	State myState;
};

AuthenticationDialogListener::AuthenticationDialogListener(NetworkAuthenticationManager &mgr, shared_ptr<ZLExecutionData::Listener> listener)
    : myHolder(this), myManager(mgr), myListener(listener), myUserList(new UserList), myState(LogOut) {
	finished(std::string());
}

void AuthenticationDialogListener::finish(bool result) {
	if (result) {
		myUserList->saveUser(myManager.currentUserName());
		delete myUserList;
		myUserList = NULL;
	}
	// FIXME: Return notable error
	myListener->finished(result ? std::string() : "Some error");
	ZLTimeManager::deleteLater(myHolder);
	myHolder.reset();
}

void AuthenticationDialogListener::showPercent(int ready, int full) {
	(void) ready;
	(void) full;
}

void AuthenticationDialogListener::finished(const std::string &error) {
	myError = error;
	if (myState == LogOut) {
		if (!AuthenticationDialog::runDialog(myManager, *myUserList, myError, myPassword)) {
			myState = LogOutFinal;
			new LogOutRunnable(myManager, myHolder);
			return;
		}
		if (myManager.UserNameOption.value().empty()) {
			const ZLResource &resource = ZLResource::resource("dialog")["AuthenticationDialog"];
			restart(resource["loginIsEmpty"].value());
		} else {
			myState = Authorisation;
			myManager.authorise(myPassword, ZLExecutionData::createListener(this, &AuthenticationDialogListener::onFinished));
			return;
		}
	} else if (myState == LogOutFinal) {
		finish(false);
	} else if (myState == Authorisation) {
		if (!myError.empty()) {
			restart(myError);
			return;
		}
		if (myManager.needsInitialization()) {
			myState = Initialization;
			myManager.initialize(myHolder);
		} else {
			finish(true);
		}
	} else if (myState == Initialization) {
		if (!myError.empty()) {
			restart(myError);
			return;
		}
		finish(true);
	}
}

void AuthenticationDialogListener::onFinished(ZLUserDataHolder &data, const std::string &error) {
	(void) data;
	finished(error);
}

void AuthenticationDialogListener::restart(const std::string &error) {
	myPassword.clear();
	myState = LogOut;
	ZLTimeManager::Instance().addAutoRemovableTask(new AuthenticationHelper(myHolder, error));
}

bool AuthenticationDialog::run(NetworkAuthenticationManager &mgr, shared_ptr<ZLExecutionData::Listener> listener) {
	new AuthenticationDialogListener(mgr, listener);
	return false;
}
