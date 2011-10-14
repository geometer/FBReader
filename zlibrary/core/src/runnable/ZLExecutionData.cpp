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

#include <ZLExecutionData.h>

const ZLTypeId ZLExecutionData::TYPE_ID(ZLObjectWithRTTI::TYPE_ID);

std::set<ZLExecutionData::Runner*> ZLExecutionData::ourRunners;
std::string ZLExecutionData::ourHandlerId = "_zl_handler";

std::string ZLExecutionData::perform(shared_ptr<ZLExecutionData> data) {
	Vector dataVector;
	dataVector.push_back(data);
	return perform(dataVector);
}

std::string ZLExecutionData::perform(const Vector &dataVector) {
	std::string result;
	for (std::set<Runner*>::const_iterator it = ourRunners.begin(); it != ourRunners.end(); ++it) {
		std::string part = (*it)->perform(dataVector);
		if (!part.empty()) {
			if (!result.empty()) {
				result += '\n';
			}
			result += part;
		}
	}
	return result;
}

ZLExecutionData::ZLExecutionData() {
}

ZLExecutionData::~ZLExecutionData() {
}

void ZLExecutionData::setListener(shared_ptr<Listener> listener) {
	if (!myListener.isNull()) {
		myListener->myProcess = 0;
	}
	myListener = listener;
	if (!myListener.isNull()) {
		myListener->myProcess = this;
	}
}

class ZLExecutionHandler : public ZLExecutionData::Listener {
public:
	ZLExecutionHandler(ZLUserDataHolder &data, ZLExecutionData::Handler handler)
	    : myData(data), myHandler(handler) {
	}
	
	ZLExecutionHandler(shared_ptr<ZLUserDataHolder> data, ZLExecutionData::Handler handler)
	    : myData(*data), myHolder(data), myHandler(handler) {
	}
	
	virtual void showPercent(int ready, int full) {
		(void) ready;
		(void) full;
	}

	virtual void finished(const std::string &error = std::string()) {
		(*myHandler)(myData, error);
	}
	
private:
	ZLUserDataHolder &myData;
	shared_ptr<ZLUserDataHolder> myHolder;
	ZLExecutionData::Handler myHandler;
};

void ZLExecutionData::setHandler(Handler handler) {
	setListener(new ZLExecutionHandler(*this, handler));
}

shared_ptr<ZLExecutionData::Listener> ZLExecutionData::createListener(shared_ptr<ZLUserDataHolder> data) {
	return new ZLExecutionHandler(data, handleHelper);
}

void ZLExecutionData::handleHelper(ZLUserDataHolder &data, const std::string &error) {
	shared_ptr<ZLUserData> userData = data.getUserData(ourHandlerId);
	if (!userData.isNull())
		static_cast<AbstractHandlerHelper&>(*userData).handle(data, error);
}

bool ZLExecutionData::hasListener() const {
	return !myListener.isNull();
}

void ZLExecutionData::setPercent(int ready, int full) {
	if (!myListener.isNull()) {
		myListener->showPercent(ready, full);
	}
}

void ZLExecutionData::finished(const std::string &error) {
	if (!myListener.isNull()) {
		myListener->finished(error);
	}
}

shared_ptr<ZLExecutionData::Listener> ZLExecutionData::listener() const {
	return myListener;
}

void ZLExecutionData::onCancel() {
}

ZLExecutionData::Runner::Runner() {
	ourRunners.insert(this);
}

ZLExecutionData::Runner::~Runner() {
	ourRunners.erase(this);
}

std::string ZLExecutionData::Runner::perform(shared_ptr<ZLExecutionData> data) const {
	Vector dataVector;
	dataVector.push_back(data);
	return perform(dataVector);
}

ZLExecutionData::Listener::Listener() {
}

ZLExecutionData::Listener::~Listener() {
}

void ZLExecutionData::Listener::cancelProcess() {
	if (myProcess != 0) {
		myProcess->onCancel();
	}
}
