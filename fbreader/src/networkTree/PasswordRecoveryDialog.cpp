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

#include <ZLStringUtil.h>
#include <ZLDialog.h>
#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>

#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLStringEditOptionEntry.h>

#include "PasswordRecoveryDialog.h"
#include "NetworkOperationRunnable.h"

#include "../network/authentication/NetworkAuthenticationManager.h"
#include "../network/NetworkErrors.h"

#include "../fbreader/FBReader.h"

PasswordRecoveryDialog::PasswordRecoveryDialog(std::string &email, const std::string &errorMessage) {
	myDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("PasswordRecoveryDialog"));

	if (!errorMessage.empty()) {
		myDialog->addOption("", "", new ZLSimpleStaticTextOptionEntry(errorMessage));
	}

	myDialog->addOption(ZLResourceKey("email"), new ZLStringEditOptionEntry(email));

	myDialog->addButton(ZLDialogManager::OK_BUTTON, true);
	myDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
}

bool PasswordRecoveryDialog::runDialog(std::string &email, std::string &errorMessage) {
	//const ZLResource &resource = ZLResource::resource("dialog")["PasswordRecoveryDialog"];
	while (true) {
		PasswordRecoveryDialog dlg(email, errorMessage);
		if (dlg.dialog().run()) {
			dlg.dialog().acceptValues();
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

bool PasswordRecoveryDialog::run(NetworkAuthenticationManager &mgr) {
	std::string errorMessage;
	std::string email;
	while (true) {
		if (!runDialog(email, errorMessage)) {
			LogOutRunnable logout(mgr);
			logout.executeWithUI();
			return false;
		}

		PasswordRecoveryRunnable recovery(mgr, email);
		recovery.executeWithUI();
		if (recovery.hasErrors()) {
			errorMessage = recovery.errorMessage();
			LogOutRunnable logout(mgr);
			logout.executeWithUI();
			continue;
		}

		ZLResourceKey boxKey("recoverySuccessfulBox");
		const std::string message =
			ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), email);
		ZLDialogManager::Instance().informationBox(boxKey, message);

		return true;
	}
}
