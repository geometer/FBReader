/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#include "ZLExecutionData.h"

ZLSlowProcess::ZLSlowProcess() {
}

ZLSlowProcess::~ZLSlowProcess() {
}

void ZLSlowProcess::setListener(shared_ptr<ZLSlowProcessListener> listener) {
	if (!myListener.isNull()) {
		myListener->myProcess = 0;
	}
	myListener = listener;
	if (!myListener.isNull()) {
		myListener->myProcess = this;
	}
}

void ZLSlowProcess::setPercent(int ready, int full) {
	if (!myListener.isNull()) {
		myListener->showPercent(ready, full);
	}
}

void ZLSlowProcess::onCancel() {
}

ZLSlowProcessListener::ZLSlowProcessListener() {
}

ZLSlowProcessListener::~ZLSlowProcessListener() {
}

void ZLSlowProcessListener::cancelProcess() {
	if (myProcess != 0) {
		myProcess->onCancel();
	}
}
