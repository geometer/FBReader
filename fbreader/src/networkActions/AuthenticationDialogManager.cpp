/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include <ZLExecutionUtil.h>
#include <ZLResource.h>
#include <ZLTimeManager.h>

#include "../network/NetworkErrors.h"
#include "../network/NetworkLink.h"
#include "AuthenticationDialog.h"
#include "NetworkOperationRunnable.h"

#include "AuthenticationDialogManager.h"

class AuthenticationDialogListener : public ZLNetworkRequest::Listener {
public:
	AuthenticationDialogListener(NetworkAuthenticationManager &mgr, shared_ptr<ZLNetworkRequest::Listener> listener);

	void returnAnswer(bool result);
	virtual void finished(const std::string &error = std::string());

private:
	void restart(const std::string &error);

	enum State { LogOut, Authorisation, Initialization };

	shared_ptr<ZLNetworkRequest::Listener> myHolder;

	NetworkAuthenticationManager &myManager;
	shared_ptr<ZLNetworkRequest::Listener> myListener;
	std::string myPassword;
	UserList myUserList;
	std::string myError;
	State myState;
};

AuthenticationDialogListener::AuthenticationDialogListener(NetworkAuthenticationManager &mgr, shared_ptr<ZLNetworkRequest::Listener> listener)
	: myHolder(this), myManager(mgr), myListener(listener), myUserList(mgr.Link.getSiteName()), myState(LogOut) {
	finished(std::string()); //start state machine from LogOut state
}

void AuthenticationDialogListener::returnAnswer(bool result) {
	if (result) {
		myUserList.saveUser(myManager.currentUserName());
	}
	// TODO: Return notable error
	myListener->setUIStatus(false);
	myListener->finished(result ? std::string() : "Some error");
	ZLTimeManager::deleteLater(myHolder);
	myHolder.reset();
}



void AuthenticationDialogListener::finished(const std::string &error) {
	myError = error;
	myListener->setUIStatus(false);
	switch (myState) {
		case LogOut:
			if (!AuthenticationDialog::run(myManager.UserNameOption, myUserList, myError, myPassword)) {
				myManager.logOut();
				returnAnswer(false);
				return;
			}
			if (myManager.UserNameOption.value().empty()) {
				const ZLResource &resource = ZLResource::resource("dialog")["AuthenticationDialog"];
				restart(resource["loginIsEmpty"].value());
			} else {
				myState = Authorisation;
				myListener->setUIStatus(true);
				myManager.authorise(myPassword, myHolder);
				return;
			}
			break;
		case Authorisation:
			if (!myError.empty()) {
				restart(myError);
				return;
			}
			if (myManager.needsInitialization()) {
				myState = Initialization;
				myListener->setUIStatus(true);
				myManager.initialize(myHolder);
			} else {
				returnAnswer(true);
			}
			break;
		case Initialization:
			if (!myError.empty()) {
				restart(myError);
				return;
			}
			returnAnswer(true);
			break;
	}
}

void AuthenticationDialogListener::restart(const std::string &error) {
	myPassword.clear();
	myState = LogOut;
	finished(error);
	//TODO it was autoremovable task here
}


class AuthoriseIfCanListener : public ZLNetworkRequest::Listener {
public:
	AuthoriseIfCanListener(NetworkAuthenticationManager &mgr, shared_ptr<ZLNetworkRequest::Listener> listener);

	void returnAnswer(std::string answer = std::string());
	virtual void finished(const std::string &error = std::string());

private:
	enum State { Init, AuthorisationCheck, Initialization};

	shared_ptr<ZLNetworkRequest::Listener> myHolder;

	NetworkAuthenticationManager &myManager;
	shared_ptr<ZLNetworkRequest::Listener> myListener;
	State myState;
};

AuthoriseIfCanListener::AuthoriseIfCanListener(NetworkAuthenticationManager &mgr, shared_ptr<ZLNetworkRequest::Listener> listener)
	: myHolder(this), myManager(mgr), myListener(listener), myState(Init) {
	finished(std::string()); //start state machine from Init state
}

void AuthoriseIfCanListener::returnAnswer(std::string answer) {
	myListener->setUIStatus(false);
	myListener->finished(answer);
	ZLTimeManager::deleteLater(myHolder);
	myHolder.reset();
}

void AuthoriseIfCanListener::finished(const std::string &error) {
	myListener->setUIStatus(false);
	switch (myState) {
		case Init:
			myListener->setUIStatus(true);
			myState = AuthorisationCheck;
			myManager.isAuthorised(myHolder);
			break;
		case AuthorisationCheck:
			if (!error.empty()) {
				NetworkErrors::showErrorMessage(error);
				returnAnswer(error);
				return;
			}
			if (myManager.needsInitialization()) {
				myState = Initialization;
				myListener->setUIStatus(true);
				myManager.initialize(myHolder);
			} else {
				returnAnswer();
			}
			break;
		case Initialization:
			if (!error.empty()) {
				NetworkErrors::showErrorMessage(error);
				returnAnswer(error);
				return;
			}
			returnAnswer();
			break;
	}
}

std::string AuthenticationDialogManager::authAndInitAsync(NetworkAuthenticationManager &manager, shared_ptr<ZLNetworkRequest::Listener> listener) {
	new AuthenticationDialogListener(manager, listener);
	return std::string();
}

std::string AuthenticationDialogManager::athoriseIfCan(NetworkAuthenticationManager &manager, shared_ptr<ZLNetworkRequest::Listener> listener) {
	new AuthoriseIfCanListener(manager, listener);
	return std::string();
}
