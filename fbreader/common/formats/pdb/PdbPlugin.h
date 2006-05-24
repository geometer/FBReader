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

#ifndef __PDBPLUGIN_H__
#define __PDBPLUGIN_H__

#include <abstract/shared_ptr.h>

#include "../FormatPlugin.h"

class PdbPlugin : public FormatPlugin {

protected:
	PdbPlugin() FORMATS_SECTION;

public:
	virtual ~PdbPlugin() FORMATS_SECTION;

protected:
	std::string fileType(const ZLFile &file) const FORMATS_SECTION;
};

class PluckerPlugin : public PdbPlugin {

public:
	bool providesMetaInfo() const FORMATS_SECTION;
	bool acceptsFile(const ZLFile &file) const FORMATS_SECTION;
	bool readDescription(const std::string &path, BookDescription &description) const FORMATS_SECTION;
	bool readModel(const BookDescription &description, BookModel &model) const FORMATS_SECTION;
	const std::string &iconName() const FORMATS_SECTION;
};

class SimplePdbPlugin : public PdbPlugin {

public:
	bool providesMetaInfo() const FORMATS_SECTION;
	bool readDescription(const std::string &path, BookDescription &description) const FORMATS_SECTION;
	bool readModel(const BookDescription &description, BookModel &model) const FORMATS_SECTION;
	FormatInfoPage *createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) FORMATS_SECTION;

protected:
	virtual shared_ptr<ZLInputStream> createStream(ZLFile &file) const = 0;
};

class PalmDocPlugin : public SimplePdbPlugin {

public:
	bool acceptsFile(const ZLFile &file) const FORMATS_SECTION;
	const std::string &iconName() const FORMATS_SECTION;

protected:
	shared_ptr<ZLInputStream> createStream(ZLFile &file) const;
};

class ZTXTPlugin : public SimplePdbPlugin {

public:
	bool acceptsFile(const ZLFile &file) const FORMATS_SECTION;
	const std::string &iconName() const FORMATS_SECTION;

protected:
	shared_ptr<ZLInputStream> createStream(ZLFile &file) const;
};

inline PdbPlugin::PdbPlugin() {}

#endif /* __PDBPLUGIN_H__ */
