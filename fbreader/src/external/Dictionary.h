/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <string>
#include <map>
#include <vector>

#include <shared_ptr.h>
#include <ZLOptions.h>
#include <ZLMessage.h>

class Dictionary;

class DictionaryCollection {

public:
	DictionaryCollection();

	const std::vector<std::string> &names() const;
	shared_ptr<Dictionary> currentDictionary() const;

public:
	mutable ZLBooleanOption EnableIntegrationOption;
	mutable ZLStringOption CurrentNameOption;

private:
	std::vector<std::string> myNames;
	std::map<std::string, shared_ptr<Dictionary> > myDictionaries;

friend class DictionaryCollectionBuilder;
};

class Dictionary {

private:
	Dictionary(shared_ptr<ZLCommunicator> communicator);

public:
	void openInDictionary(const std::string &word) const;

private:
	shared_ptr<ZLCommunicator> myCommunicator;
	ZLCommunicationManager::Data myShowWordData;

friend class DictionaryCollection;
friend class DictionaryCollectionBuilder;
};

#endif /* __DICTIONARY_H__ */
