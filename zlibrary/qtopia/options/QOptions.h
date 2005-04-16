/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __QOPTIONS_H__
#define __QOPTIONS_H__

#include <qpe/config.h>

#include "../../abstract/options/ZLOptions_internal.h"

class QOptions : public ZLOptions {

public:
	static void createInstance(const char *name) { ourInstance = new QOptions(name); }
	
private:
	Config myConfig;

private:
	QOptions(const char *name) : myConfig(name) {}

public:
	void setGroup(const std::string &name) { myConfig.setGroup(name.c_str()); }
	void clearGroup() { myConfig.clearGroup(); }
	
	void unsetValue(const std::string &name) { myConfig.removeEntry(name.c_str()); }

	bool booleanValue(const std::string &name, bool defaultValue) { return myConfig.readBoolEntry(name.c_str(), defaultValue); }
	void setValue(const std::string &name, bool value) { myConfig.writeEntry(name.c_str(), value); }

	long integerValue(const std::string &name, long defaultValue) { return myConfig.readNumEntry(name.c_str(), defaultValue); }
	void setValue(const std::string &name, long value) { myConfig.writeEntry(name.c_str(), (int)value); }

	double doubleValue(const std::string &name, double defaultValue) { return myConfig.readEntry(name.c_str(), QString::number(defaultValue)).toDouble(); }
	void setValue(const std::string &name, double value) { myConfig.writeEntry(name.c_str(), QString::number(value)); }

	std::string stringValue(const std::string &name, const std::string &defaultValue) { return myConfig.readEntry(name.c_str(), defaultValue.c_str()).ascii(); }
	void setValue(const std::string &name, const std::string &value) { myConfig.writeEntry(name.c_str(), value.c_str()); }
};

#endif /* __QOPTIONS_H__ */
