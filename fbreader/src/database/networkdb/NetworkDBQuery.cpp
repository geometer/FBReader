/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include "NetworkDBQuery.h"

const std::string NetworkDBQuery::INIT_DATABASE = \
	"	CREATE TABLE IF NOT EXISTS Links( " \
	"	link_id INTEGER PRIMARY KEY, " \
	"	title TEXT NOT NULL, " \
	"	site_name TEXT NOT NULL, " \
	"	summary TEXT, " \
	"	language TEXT, " \
	"	predefined_id TEXT, " \
	"	is_enabled INTEGER " \
	"); " \
	" " \
	"CREATE TABLE IF NOT EXISTS LinkUrls( " \
	"	key TEXT NOT NULL, " \
	"	link_id INTEGER NOT NULL REFERENCES Links(link_id), " \
	"	url TEXT, " \
	"	update_time INTEGER, " \
	"	CONSTRAINT LinkUrls_PK PRIMARY KEY (key, link_id) " \
	"); ";

const std::string NetworkDBQuery::CLEAR_DATABASE = \
	"DROP TABLE Links; " \
	"DROP TABLE LinkUrls; ";


const std::string NetworkDBQuery::FIND_NETWORK_LINK_ID  = "SELECT link_id, predefined_id FROM Links WHERE site_name = @site_name; ";
const std::string NetworkDBQuery::ADD_NETWORK_LINK  = \
	"INSERT INTO Links (title, site_name, summary, language, predefined_id, is_enabled) " \
	"	VALUES ( " \
	"		@title, " \
	"		@site_name, " \
	"		@summary, " \
	"		nullif(@language,\"\"), " \
	"		nullif(@predefined_id,\"\"), " \
	"		@is_enabled " \
	"	); " \
	" " \
	"SELECT last_insert_rowid() AS link_id; ";

const std::string NetworkDBQuery::DELETE_NETWORK_LINK  = \
	"DELETE FROM Links WHERE link_id = @link_id; ";

const std::string NetworkDBQuery::UPDATE_NETWORK_LINK  = \
	"UPDATE Links SET " \
	"	title = @title, " \
	"	summary = @summary, " \
	"	language = nullif(@language,\"\"), " \
	"	predefined_id = nullif(@predefined_id,\"\"), " \
	"	is_enabled = @is_enabled " \
	"WHERE " \
	"	link_id = @link_id; ";

const std::string NetworkDBQuery::ADD_NETWORK_LINKURL  = \
	"INSERT INTO LinkUrls (key, link_id, url, update_time) " \
	"	VALUES ( " \
	"		@key, " \
	"		@link_id, " \
	"		@url, " \
	"		@update_time " \
	"	); ";

const std::string NetworkDBQuery::FIND_NETWORK_LINKURLS  = "SELECT key, url, update_time FROM LinkUrls WHERE link_id = @link_id; ";

const std::string NetworkDBQuery::UPDATE_NETWORK_LINKURL  = \
	"UPDATE LinkUrls SET " \
	"	url = @url, " \
	"	update_time = @update_time " \
	"WHERE " \
	"	link_id = @link_id AND key = @key; ";

const std::string NetworkDBQuery::DELETE_NETWORK_LINKURLS  = \
	"DELETE FROM LinkUrls " \
	"WHERE " \
	"	link_id = @link_id; ";

const std::string NetworkDBQuery::DELETE_NETWORK_LINKURL  = \
	"DELETE FROM LinkUrls " \
	"WHERE " \
	"	link_id = @link_id AND key = @key; ";

const std::string NetworkDBQuery::LOAD_NETWORK_LINKS = "SELECT link_id, title, site_name, summary, language, predefined_id, is_enabled FROM Links; ";

const std::string NetworkDBQuery::LOAD_NETWORK_LINKURLS = "SELECT key, url, update_time FROM LinkUrls WHERE link_id = @link_id; ";
