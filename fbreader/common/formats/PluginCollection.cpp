/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "FormatPlugin.h"

#include "fb2/FB2Plugin.h"
#include "docbook/DocBookPlugin.h"
#include "html/HtmlPlugin.h"
#include "txt/TxtPlugin.h"

PluginCollection *PluginCollection::ourInstance = 0;

PluginCollection &PluginCollection::instance() {
	if (ourInstance == 0) {
		ourInstance = new PluginCollection();
		ourInstance->myPlugins.push_back(new FB2Plugin());
		ourInstance->myPlugins.push_back(new DocBookPlugin());
		ourInstance->myPlugins.push_back(new HtmlPlugin());
		ourInstance->myPlugins.push_back(new TxtPlugin());
	}
	return *ourInstance;
}

FormatPlugin *PluginCollection::plugin(const std::string &fileName, bool strong) {
	for (std::vector<FormatPlugin*>::iterator it = myPlugins.begin(); it != myPlugins.end(); it++) {
		if ((!strong || (*it)->providesMetaInfo()) && (*it)->acceptsFile(fileName)) {
			return *it;
		}
	}
	return 0;
}
