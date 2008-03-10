/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __MIGRATION_H__
#define __MIGRATION_H__

#include <string>

#include <ZLOptions.h>

class Migration {

protected:
	static void moveOption(
		const ZLCategoryKey &oldCategory, const std::string &oldGroup, const std::string &oldName,
		const ZLCategoryKey &newCategory, const std::string &newGroup, const std::string &newName,
		const std::string &defaultValue
	);
	static bool isLikeToFileName(const std::string &str);

public:
	Migration(const std::string &version);
	virtual ~Migration();
	void doMigration();

protected:
	virtual void doMigrationInternal() = 0;

private:
	const std::string myVersion;
};

class Migration_0_8_11 : public Migration {

public:
	Migration_0_8_11();

protected:
	void doMigrationInternal();
};

class Migration_0_8_13 : public Migration {

public:
	Migration_0_8_13();

protected:
	void doMigrationInternal();
};

class Migration_0_8_16 : public Migration {

public:
	Migration_0_8_16();

protected:
	void doMigrationInternal();
};

#endif /* __MIGRATION_H__ */
