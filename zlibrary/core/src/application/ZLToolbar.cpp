/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <stdlib.h>

#include <ZLApplication.h>
#include <ZLXMLReader.h>
#include <ZLResource.h>
#include <ZLibrary.h>

class ZLToolbarCreator : public ZLXMLReader {

public:
	ZLToolbarCreator(ZLApplication::Toolbar &toolbar);
	void startElementHandler(const char *tag, const char **attributes);

private:
	ZLApplication::Toolbar &myToolbar;
};

void ZLApplication::createToolbar() {
	ZLToolbarCreator(myToolbar).readDocument(ZLibrary::DefaultFilesPathPrefix() + "toolbar.xml");
}

ZLToolbarCreator::ZLToolbarCreator(ZLApplication::Toolbar &toolbar) : myToolbar(toolbar) {
}

void ZLToolbarCreator::startElementHandler(const char *tag, const char **attributes) {
	static const std::string BUTTON = "button";
	static const std::string SEPARATOR = "separator";
	if (BUTTON == tag) {
		const char *action = attributeValue(attributes, "action");
		const char *key = attributeValue(attributes, "key");
		if ((action != 0) && (key != 0)) {
			myToolbar.addButton(atoi(action), ZLResourceKey(key));
		}
	} else if (SEPARATOR == tag) {
		myToolbar.addSeparator();
	}
}
