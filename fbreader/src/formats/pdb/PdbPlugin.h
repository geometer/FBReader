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

#ifndef __PDBPLUGIN_H__
#define __PDBPLUGIN_H__

#include <shared_ptr.h>

#include "../FormatPlugin.h"

class PdbPlugin : public FormatPlugin {

public:
	static std::string fileType(const ZLFile &file);

protected:
	PdbPlugin();

public:
	virtual ~PdbPlugin();
};

class PluckerPlugin : public PdbPlugin {

public:
	bool providesMetaInfo() const;
	bool acceptsFile(const ZLFile &file) const;
	bool readDescription(const std::string &path, BookDescription &description) const;
	bool readModel(const BookDescription &description, BookModel &model) const;
	const std::string &iconName() const;
};

class SimplePdbPlugin : public PdbPlugin {

public:
	bool readDescription(const std::string &path, BookDescription &description) const;
	bool readModel(const BookDescription &description, BookModel &model) const;

protected:
	virtual shared_ptr<ZLInputStream> createStream(ZLFile &file) const = 0;
	virtual void readDocumentInternal(const std::string &fileName, BookModel &model, const class PlainTextFormat &format, const std::string &encoding, ZLInputStream &stream) const;
};

class PalmDocLikePlugin : public SimplePdbPlugin {

public:
	bool providesMetaInfo() const;
	const std::string &tryOpen(const std::string &path) const;

protected:
	shared_ptr<ZLInputStream> createStream(ZLFile &file) const;
};

class PalmDocPlugin : public PalmDocLikePlugin {

public:
	bool acceptsFile(const ZLFile &file) const;
	const std::string &iconName() const;

	void readDocumentInternal(const std::string &fileName, BookModel &model, const class PlainTextFormat &format, const std::string &encoding, ZLInputStream &stream) const;

private:
	FormatInfoPage *createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName);
};

class MobipocketPlugin : public PalmDocLikePlugin {

public:
	bool acceptsFile(const ZLFile &file) const;
	const std::string &iconName() const;

	void readDocumentInternal(const std::string &fileName, BookModel &model, const class PlainTextFormat &format, const std::string &encoding, ZLInputStream &stream) const;
};

class ZTXTPlugin : public SimplePdbPlugin {

public:
	bool providesMetaInfo() const;
	bool acceptsFile(const ZLFile &file) const;
	const std::string &iconName() const;

protected:
	shared_ptr<ZLInputStream> createStream(ZLFile &file) const;

private:
	FormatInfoPage *createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName);
};

inline PdbPlugin::PdbPlugin() {}

#endif /* __PDBPLUGIN_H__ */
