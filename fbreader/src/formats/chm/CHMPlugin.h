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

#ifndef __CHMPLUGIN_H__
#define __CHMPLUGIN_H__

#include "../FormatPlugin.h"

class CHMPlugin : public FormatPlugin {

public:
	CHMPlugin();
	~CHMPlugin();
	bool providesMetaInfo() const;
	bool acceptsFile(const ZLFile &file) const;
	bool readDescription(const std::string &path, BookDescription &description) const;
	bool readModel(const BookDescription &description, BookModel &model) const;
	const std::string &iconName() const;
};

inline CHMPlugin::CHMPlugin() {}
inline CHMPlugin::~CHMPlugin() {}
inline bool CHMPlugin::providesMetaInfo() const { return false; }

#endif /* __CHMPLUGIN_H__ */
