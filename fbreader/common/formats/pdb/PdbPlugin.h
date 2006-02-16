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
	PluckerPlugin() FORMATS_SECTION;
	~PluckerPlugin() FORMATS_SECTION;
	bool providesMetaInfo() const FORMATS_SECTION;
	bool acceptsFile(const ZLFile &file) const FORMATS_SECTION;
	bool readDescription(const std::string &path, BookDescription &description) const FORMATS_SECTION;
	bool readModel(const BookDescription &description, BookModel &model) const FORMATS_SECTION;
	const std::string &iconName() const FORMATS_SECTION;
};

class PalmDocPlugin : public PdbPlugin {

public:
	PalmDocPlugin() FORMATS_SECTION;
	~PalmDocPlugin() FORMATS_SECTION;
	bool providesMetaInfo() const FORMATS_SECTION;
	bool acceptsFile(const ZLFile &file) const FORMATS_SECTION;
	bool readDescription(const std::string &path, BookDescription &description) const FORMATS_SECTION;
	bool readModel(const BookDescription &description, BookModel &model) const FORMATS_SECTION;
	const std::string &iconName() const FORMATS_SECTION;
	FormatInfoPage *createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) FORMATS_SECTION;
};

class ZTXTPlugin : public PdbPlugin {

public:
	ZTXTPlugin() FORMATS_SECTION;
	~ZTXTPlugin() FORMATS_SECTION;
	bool providesMetaInfo() const FORMATS_SECTION;
	bool acceptsFile(const ZLFile &file) const FORMATS_SECTION;
	bool readDescription(const std::string &path, BookDescription &description) const FORMATS_SECTION;
	bool readModel(const BookDescription &description, BookModel &model) const FORMATS_SECTION;
	const std::string &iconName() const FORMATS_SECTION;
	FormatInfoPage *createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) FORMATS_SECTION;
};

inline PdbPlugin::PdbPlugin() {}
inline PdbPlugin::~PdbPlugin() {}

inline PluckerPlugin::PluckerPlugin() {}
inline PluckerPlugin::~PluckerPlugin() {}
inline bool PluckerPlugin::providesMetaInfo() const { return false; }

inline PalmDocPlugin::PalmDocPlugin() {}
inline PalmDocPlugin::~PalmDocPlugin() {}
inline bool PalmDocPlugin::providesMetaInfo() const { return false; }

inline ZTXTPlugin::ZTXTPlugin() {}
inline ZTXTPlugin::~ZTXTPlugin() {}
inline bool ZTXTPlugin::providesMetaInfo() const { return false; }

#endif /* __PDBPLUGIN_H__ */
