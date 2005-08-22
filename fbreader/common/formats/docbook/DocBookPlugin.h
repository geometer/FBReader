/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __DOCBOOKPLUGIN_H__
#define __DOCBOOKPLUGIN_H__

#include "../FormatPlugin.h"

class DocBookPlugin : public FormatPlugin {

public:
	DocBookPlugin() FORMATS_SECTION;
	~DocBookPlugin() FORMATS_SECTION;
	bool providesMetaInfo() const FORMATS_SECTION;
	bool acceptsFile(const std::string &extension) const FORMATS_SECTION;
	bool readDescription(const std::string &path, BookDescription &description) const FORMATS_SECTION;
	bool readModel(const BookDescription &description, BookModel &model) const FORMATS_SECTION;
	const std::string &iconName() const FORMATS_SECTION;
};

inline DocBookPlugin::DocBookPlugin() {}
inline DocBookPlugin::~DocBookPlugin() {}
inline bool DocBookPlugin::providesMetaInfo() const { return true; }

#endif /* __DOCBOOKPLUGIN_H__ */
