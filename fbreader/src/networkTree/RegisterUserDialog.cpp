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

bool RegisterUserDialog::run(NetworkAuthenticationManager &mgr) {
	std::string errorMessage;
	std::string login;
	std::string password;
	std::string email;
	while (true) {
		if (!runDialog(login, password, email, errorMessage)) {
			LogOutRunnable logout(mgr);
			logout.executeWithUI();
			return false;
		}

		RegisterUserRunnable registration(mgr, login, password, email);
		registration.executeWithUI();
		if (registration.hasErrors()) {
			errorMessage = registration.errorMessage();
			LogOutRunnable logout(mgr);
			logout.executeWithUI();
			continue;
		}

		if (mgr.isAuthorised().Status != B3_FALSE && mgr.needsInitialization()) {
			InitializeAuthenticationManagerRunnable initializer(mgr);
			initializer.executeWithUI();
			if (initializer.hasErrors()) {
				initializer.showErrorMessage();
				LogOutRunnable logout(mgr);
				logout.executeWithUI();
				return false;
			}
		}
		return true;
	}
}
