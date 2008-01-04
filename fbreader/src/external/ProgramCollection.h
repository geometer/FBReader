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

#ifndef __PROGRAMCOLLECTION_H__
#define __PROGRAMCOLLECTION_H__

#include <string>
#include <map>
#include <vector>

#include <shared_ptr.h>
#include <ZLMessage.h>
#include <ZLOptions.h>

class Program {

private:
	Program(const std::string &name, shared_ptr<ZLMessageOutputChannel> channel);

public:
	void run(const std::string &command, const std::string &parameter) const;

public:
	struct OptionDescription {
		OptionDescription(const std::string &name, const std::string &defaultValue);
		std::string OptionName;
		std::string DefaultValue;
	};
	const std::vector<OptionDescription> &options() const;

private:
	const std::string myName;
	shared_ptr<ZLMessageOutputChannel> myChannel;
	std::map<std::string,ZLCommunicationManager::Data> myCommandData;
	std::vector<OptionDescription> myOptions;
	std::map<std::string,std::string> myDefaultValues;

friend class ProgramCollection;
friend class ProgramCollectionBuilder;
};

class ProgramCollection {

public:
	mutable ZLBooleanOption EnableCollectionOption;
	mutable ZLStringOption CurrentNameOption;

public:
	ProgramCollection(const std::string &name);

	const std::vector<std::string> &names() const;
	shared_ptr<Program> currentProgram() const;
	shared_ptr<Program> program(const std::string &name) const;

private:
	std::vector<std::string> myNames;
	std::map<std::string,shared_ptr<Program> > myPrograms;

friend class ProgramCollectionBuilder;
};

class ProgramCollectionMap {

public:
	ProgramCollectionMap();
	shared_ptr<ProgramCollection> collection(const std::string &name) const;

private:
	std::map<std::string,shared_ptr<ProgramCollection> > myMap;

friend class ProgramCollectionBuilder;
};

#endif /* __PROGRAMCOLLECTION_H__ */
