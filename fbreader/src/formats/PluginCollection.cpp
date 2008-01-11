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

#include <ZLStringUtil.h>

#include "FormatPlugin.h"

#include "fb2/FB2Plugin.h"
//#include "docbook/DocBookPlugin.h"
#include "html/HtmlPlugin.h"
#include "txt/TxtPlugin.h"
#include "pdb/PdbPlugin.h"
#include "tcr/TcrPlugin.h"
#include "oeb/OEBPlugin.h"
#include "chm/CHMPlugin.h"
#include "rtf/RtfPlugin.h"
#include "openreader/OpenReaderPlugin.h"

PluginCollection *PluginCollection::ourInstance = 0;

PluginCollection &PluginCollection::instance() {
	if (ourInstance == 0) {
		ourInstance = new PluginCollection();
		ourInstance->myPlugins.push_back(new FB2Plugin());
		//ourInstance->myPlugins.push_back(new DocBookPlugin());
		ourInstance->myPlugins.push_back(new HtmlPlugin());
		ourInstance->myPlugins.push_back(new TxtPlugin());
		ourInstance->myPlugins.push_back(new PluckerPlugin());
		ourInstance->myPlugins.push_back(new PalmDocPlugin());
		ourInstance->myPlugins.push_back(new MobipocketPlugin());
		ourInstance->myPlugins.push_back(new ZTXTPlugin());
		ourInstance->myPlugins.push_back(new TcrPlugin());
		ourInstance->myPlugins.push_back(new CHMPlugin());
		ourInstance->myPlugins.push_back(new OEBPlugin());
		ourInstance->myPlugins.push_back(new RtfPlugin());
		ourInstance->myPlugins.push_back(new OpenReaderPlugin());
	}
	return *ourInstance;
}

void PluginCollection::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
		ourInstance = 0;
	}
}

PluginCollection::PluginCollection() :
	LanguageAutoDetectOption(ZLCategoryKey::CONFIG, "Format", "AutoDetect", true),
	DefaultLanguageOption(ZLCategoryKey::CONFIG, "Format", "DefaultLanguageS", "en"),
	DefaultEncodingOption(ZLCategoryKey::CONFIG, "Format", "DefaultEncoding", "windows-1252") {
}

PluginCollection::~PluginCollection() {
	for (std::vector<FormatPlugin*>::const_iterator it = myPlugins.begin(); it != myPlugins.end(); ++it) {
		delete *it;
	}
}

FormatPlugin *PluginCollection::plugin(const ZLFile &file, bool strong) {
	for (std::vector<FormatPlugin*>::iterator it = myPlugins.begin(); it != myPlugins.end(); ++it) {
		if ((!strong || (*it)->providesMetaInfo()) && (*it)->acceptsFile(file)) {
			return *it;
		}
	}
	return 0;
}
