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
#include <optionEntries/ZLStringEditOptionEntry.h>


#include "RegisterUserDialog.h"
#include "NetworkOperationRunnable.h"

#include "../network/authentication/NetworkAuthenticationManager.h"
#include "../network/NetworkErrors.h"

#include "../fbreader/FBReader.h"


class HiddenValueEntry : public ZLPasswordOptionEntry {

public:
	HiddenValueEntry(std::string &value);

	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	std::string &myValue;
};

HiddenValueEntry::HiddenValueEntry(std::string &value) : myValue(value) {
}

const std::string &HiddenValueEntry::initialValue() const {
	return myValue;
}

void HiddenValueEntry::onAccept(const std::string &value) {
	myValue = value;
}


RegisterUserDialog::RegisterUserDialog(const std::string &login, const std::string &password, const std::string &email, const std::string &errorMessage) {
	myDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("RegisterUserDialog"));

	if (!errorMessage.empty()) {
		myDialog->addOption("", "", new ZLSimpleStaticTextOptionEntry(errorMessage));
	}

	myLogin = login;
	myPassword0 = myPassword1 = password;
	myEMail = email;

	myDialog->addOption(ZLResourceKey("login"), new ZLStringEditOptionEntry(myLogin));
	myDialog->addOption(ZLResourceKey("password"), new HiddenValueEntry(myPassword0));
	myDialog->addOption(ZLResourceKey("confirmPassword"), new HiddenValueEntry(myPassword1));
	myDialog->addOption(ZLResourceKey("email"), new ZLStringEditOptionEntry(myEMail));

	myDialog->addButton(ZLDialogManager::OK_BUTTON, true);
	myDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
}

bool RegisterUserDialog::runDialog(std::string &login, std::string &password, std::string &email, std::string &errorMessage) {
	const ZLResource &resource = ZLResource::resource("dialog")["RegisterUserDialog"];
	while (true) {
		RegisterUserDialog dlg(login, password, email, errorMessage);
		if (dlg.dialog().run()) {
			dlg.dialog().acceptValues();
			login = dlg.myLogin;
			password = dlg.myPassword0;
			email = dlg.myEMail;
			if (login.empty()) {
				errorMessage = NetworkErrors::errorMessage(NetworkErrors::ERROR_LOGIN_WAS_NOT_SPECIFIED);
				continue;
			}
			if (dlg.myPassword0 != dlg.myPassword1) {
				errorMessage = resource["differentPasswords"].value();
				password.clear();
				continue;
			}
			if (email.empty()) {
				errorMessage = NetworkErrors::errorMessage(NetworkErrors::ERROR_EMAIL_WAS_NOT_SPECIFIED);
				continue;
			}
			size_t atPos = email.find('@');
			if (atPos >= (email.size() - 1) || email.find('.', atPos) == std::string::npos) {
				errorMessage = NetworkErrors::errorMessage(NetworkErrors::ERROR_INVALID_EMAIL);
				continue;
			}
			return true;
		}
		return false;
	}
}

class RegisterUserDialogRunnable : public ZLRunnable {
public:
	RegisterUserDialogRunnable(NetworkAuthenticationManager &mgr, shared_ptr<ZLExecutionData::Listener> listener);
	void run();
	
	NetworkAuthenticationManager &mgr;
	shared_ptr<ZLExecutionData::Listener> listener;
	shared_ptr<ZLRunnable> myHolder;
	std::string errorMessage;
	std::string login;
	std::string password;
	std::string email;
	
private:
	void finish(const std::string &error);
	void onRun(ZLUserDataHolder &data, const std::string &error);
	void onLogOut(ZLUserDataHolder &data, const std::string &error);
	void onRegistered(ZLUserDataHolder &data, const std::string &error);
	void onAuthorisationCheck(ZLUserDataHolder &data, const std::string &error);
	void onInitialized(ZLUserDataHolder &data, const std::string &error);
};

RegisterUserDialogRunnable::RegisterUserDialogRunnable(NetworkAuthenticationManager &mgr, shared_ptr<ZLExecutionData::Listener> listener)
    : mgr(mgr), listener(listener), myHolder(this) {
	ZLTimeManager::Instance().addAutoRemovableTask(myHolder);
}

void RegisterUserDialogRunnable::finish(const std::string &error) {
	listener->finished(error);
	ZLTimeManager::deleteLater(myHolder);
	myHolder.reset();
}

void RegisterUserDialogRunnable::run() {
	if (!RegisterUserDialog::runDialog(login, password, email, errorMessage)) {
		errorMessage = "Canceled";
		new LogOutRunnable(mgr, ZLExecutionData::createListener(this, &RegisterUserDialogRunnable::onLogOut));
		return;
	}
	
	mgr.registerUser(login, password, email,
	                 ZLExecutionData::createListener(this, &RegisterUserDialogRunnable::onRegistered));
}

void RegisterUserDialogRunnable::onRun(ZLUserDataHolder &, const std::string &) {
	run();
}

void RegisterUserDialogRunnable::onLogOut(ZLUserDataHolder &, const std::string &) {
	finish(errorMessage);
}

void RegisterUserDialogRunnable::onRegistered(ZLUserDataHolder &, const std::string &error) {
	if (!error.empty()) {
		errorMessage = error;
		new LogOutRunnable(mgr, ZLExecutionData::createListener(this, &RegisterUserDialogRunnable::onRun));
		return;
	}
	mgr.isAuthorised(ZLExecutionData::createListener(this, &RegisterUserDialogRunnable::onAuthorisationCheck));
}

void RegisterUserDialogRunnable::onAuthorisationCheck(ZLUserDataHolder &, const std::string &error) {
	if (error.empty() && mgr.needsInitialization()) {
		mgr.initialize(ZLExecutionData::createListener(this, &RegisterUserDialogRunnable::onInitialized));
	} else {
		finish(error);
	}
}

void RegisterUserDialogRunnable::onInitialized(ZLUserDataHolder &data, const std::string &error) {
        (void)data;
	if (error.empty()) {
		finish(std::string());
		return;
	}
	ZLDialogManager::Instance().errorBox(
		ZLResourceKey("networkError"),
		error
	);
	errorMessage = error;
	new LogOutRunnable(mgr, ZLExecutionData::createListener(this, &RegisterUserDialogRunnable::onLogOut));
}

bool RegisterUserDialog::run(NetworkAuthenticationManager &mgr, shared_ptr<ZLExecutionData::Listener> listener) {
	new RegisterUserDialogRunnable(mgr, listener);
	return false;
}
