/*
 * Copyright (C) 2008-2012 Geometer Plus <contact@geometerplus.com>
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

#include <map>

#include <ZLStringUtil.h>

#include "Migration.h"
#include "../fbreader/FBReaderActions.h"
#include "../database/networkdb/NetworkDB.h"

Migration_0_99_1::Migration_0_99_1() : Migration("0.99.1") {
}

void Migration_0_99_1::doMigrationInternal() {
	shared_ptr<DBCommand> cmd = SQLiteFactory::createCommand("DROP TABLE IF EXISTS NetFiles", NetworkDB::Instance().connection());
	cmd->execute();
}
