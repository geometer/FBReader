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

#ifndef __TXTPLUGIN_H__
#define __TXTPLUGIN_H__

#include "../FormatPlugin.h"

class TxtPlugin : public FormatPlugin {

public:
	TxtPlugin() FORMATS_SECTION;
	~TxtPlugin() FORMATS_SECTION;
	bool providesMetaInfo() const FORMATS_SECTION;
	bool acceptsFile(const std::string &fileName) const FORMATS_SECTION;
	bool readDescription(const std::string &fileName, BookDescription &description) const FORMATS_SECTION;
	bool readModel(const BookDescription &description, BookModel &model) const FORMATS_SECTION;
	const std::string &iconName() const FORMATS_SECTION;
	FormatInfoPage *createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) FORMATS_SECTION;
};

inline TxtPlugin::TxtPlugin() {}
inline TxtPlugin::~TxtPlugin() {}
inline bool TxtPlugin::providesMetaInfo() const { return false; }

#endif /* __TXTPLUGIN_H__ */
