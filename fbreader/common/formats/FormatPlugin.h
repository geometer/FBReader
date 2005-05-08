/*
 * FBReader -- electronic book reader
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

#ifndef __FORMATPLUGIN_H__
#define __FORMATPLUGIN_H__

#include <string>
#include <vector>

class BookDescription;
class BookModel;

class FormatPlugin {

protected:
	FormatPlugin() FORMATS_SECTION;
	
public:
	virtual ~FormatPlugin() FORMATS_SECTION;
	virtual bool providesMetaInfo() const FORMATS_SECTION = 0;
	virtual bool acceptsFile(const std::string &fileName) const FORMATS_SECTION = 0;
	virtual bool readDescription(const std::string &fileName, BookDescription &description) const FORMATS_SECTION = 0;
	virtual bool readModel(const BookDescription &description, BookModel &model) const FORMATS_SECTION = 0;
	virtual const std::string &iconName() const FORMATS_SECTION = 0;
};

class PluginCollection {

public:
	static PluginCollection &instance() FORMATS_SECTION;
	static void deleteInstance() FORMATS_SECTION;

private:
	PluginCollection() FORMATS_SECTION;
	~PluginCollection() FORMATS_SECTION;
	
public:
	FormatPlugin *plugin(const std::string &fileName, bool strong) FORMATS_SECTION;

private:
	static PluginCollection *ourInstance;

	std::vector<FormatPlugin*> myPlugins;
};

inline FormatPlugin::FormatPlugin() {}
inline FormatPlugin::~FormatPlugin() {}

#endif /* __FORMATPLUGIN_H__ */
