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

#ifndef __FORMATPLUGIN_H__
#define __FORMATPLUGIN_H__

#include <string>
#include <vector>

#include <abstract/ZLOptions.h>

#include "util/EncodingDetector.h"

class BookDescription;
class BookModel;
class ZLOptionsDialog;
class ZLOptionsDialogTab;
class ZLFile;
class ZLInputStream;

class FormatInfoPage {

protected:
  FormatInfoPage() FORMATS_SECTION;

public:
  virtual ~FormatInfoPage() FORMATS_SECTION;
};

class FormatPlugin {

protected:
  FormatPlugin() FORMATS_SECTION;
  
public:
  virtual ~FormatPlugin() FORMATS_SECTION;
  virtual bool providesMetaInfo() const FORMATS_SECTION = 0;
  virtual bool acceptsFile(const ZLFile &file) const FORMATS_SECTION = 0;
  virtual bool readDescription(const std::string &path, BookDescription &description) const FORMATS_SECTION = 0;
  virtual bool readModel(const BookDescription &description, BookModel &model) const FORMATS_SECTION = 0;
  virtual const std::string &iconName() const FORMATS_SECTION = 0;
  virtual FormatInfoPage *createInfoPage(ZLOptionsDialog &dialog, const std::string &path) FORMATS_SECTION;

protected:
  void detectEncoding(BookDescription &description, ZLInputStream &stream) const;
  void defaultLanguage(BookDescription &description) const;
  void defaultTitle(BookDescription &description, const std::string &title) const;
};

class PluginCollection {

public:
  ZLIntegerOption DefaultLanguageOption;
  
public:
  static PluginCollection &instance() FORMATS_SECTION;
  static void deleteInstance() FORMATS_SECTION;

private:
  PluginCollection() FORMATS_SECTION;
  ~PluginCollection() FORMATS_SECTION;
  
public:
  FormatPlugin *plugin(const ZLFile &file, bool strong) FORMATS_SECTION;

private:
  static PluginCollection *ourInstance;

  std::vector<FormatPlugin*> myPlugins;
};

inline FormatInfoPage::FormatInfoPage() {}
inline FormatInfoPage::~FormatInfoPage() {}
inline FormatPlugin::FormatPlugin() {}
inline FormatPlugin::~FormatPlugin() {}
inline FormatInfoPage *FormatPlugin::createInfoPage(ZLOptionsDialog&, const std::string&) { return 0; }

#endif /* __FORMATPLUGIN_H__ */
