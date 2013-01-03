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

#include "ZLExecutionUtil.h"

//TODO maybe simplify these methods

std::string ZLExecutionUtil::ourHandlerId = "_zl_handler";

class ZLExecutionHandler : public ZLNetworkRequest::Listener {
public:
	ZLExecutionHandler(shared_ptr<ZLUserDataHolder> data, ZLExecutionUtil::Handler handler)
		: myData(data), myHandler(handler) {
	}

	virtual void showPercent(int ready, int full) {
		(void) ready;
		(void) full;
	}

	virtual void finished(const std::string &error = std::string()) {
		(*myHandler)(*myData, error);
	}

private:
	shared_ptr<ZLUserDataHolder> myData;
	ZLExecutionUtil::Handler myHandler;
};

shared_ptr<ZLNetworkRequest::Listener> ZLExecutionUtil::createListener(shared_ptr<ZLUserDataHolder> data) {
	return new ZLExecutionHandler(data, handleHelper);
}

void ZLExecutionUtil::handleHelper(ZLUserDataHolder &data, const std::string &error) {
	shared_ptr<ZLUserData> userData = data.getUserData(ourHandlerId);
	if (!userData.isNull())
		static_cast<AbstractHandlerHelper&>(*userData).handle(data, error);
}
