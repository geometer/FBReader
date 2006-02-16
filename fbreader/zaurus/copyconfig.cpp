/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <string>
#include <qstring.h>

#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLOptions.h>

void convertUtf(std::string &str) {
	QString s = QString::fromUtf8(str.c_str());
	str = s.ascii();
}

void copyConfig() {
	ZLFile oldConfig("~/Settings/FBReader.conf");
	ZLFile newConfig("~/.FBReader/config.xml");

	if (!oldConfig.exists() || newConfig.exists()) {
		return;
	}

	shared_ptr<ZLInputStream> stream = oldConfig.inputStream();
	if (stream.isNull() || !stream->open()) {
		return;
	}

	std::string group;
	std::string valueString;

	const int bufferSize = 1024;
	char buffer[bufferSize];
	int len;
	bool newLineFlag = true;
	const char *groupStart = 0;
	const char *valueStart = 0;
	while ((len = stream->read(buffer, bufferSize)) != 0) {
		const char *end = buffer + len;
		for (const char *ptr = buffer; ptr != end; ptr++) {
			if (*ptr == '\n') {
				if (groupStart) {
					group.append(groupStart, ptr - groupStart);
					groupStart = 0;
					convertUtf(group);
					int index = group.rfind(']');
					if (index >= 0) {
						group = group.substr(0, index);
					} else {
						group.erase();
					}
				} else if (valueStart) {
					valueString.append(valueStart, ptr - valueStart);
					valueStart = 0;
					convertUtf(valueString);
					int index = valueString.find('=');
					if (index > 1) {
						std::string name = valueString.substr(0, index - 1);
						std::string value = valueString.substr(index + 2);
						if (!group.empty() && !name.empty() && !value.empty()) {
							ZLStringOption(group, name, "").setValue(value);
						}
					}
				}
			} else if (newLineFlag) {
				if (*ptr == '[') {
					group.erase();
					groupStart = ptr + 1;
				} else {
					valueString.erase();
					valueStart = ptr;
				}
			}
			newLineFlag = *ptr == '\n';
		}
		if (groupStart) {
			group.append(groupStart, end - groupStart);
			groupStart = buffer;
		} else if (valueStart) {
			valueString.append(valueStart, end - valueStart);
			valueStart = buffer;
		}
	}

	stream->close();
}
