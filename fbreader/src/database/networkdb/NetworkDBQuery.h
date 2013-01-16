/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __NETWORKDBQUERY_H__
#define __NETWORKDBQUERY_H__

#include <string>

class NetworkDBQuery {

public:
	static const std::string INIT_DATABASE;
	static const std::string CLEAR_DATABASE;

	static const std::string FIND_NETWORK_LINK_ID;
	static const std::string ADD_NETWORK_LINK;
	static const std::string UPDATE_NETWORK_LINK;
	static const std::string DELETE_NETWORK_LINK;

	static const std::string ADD_NETWORK_LINKURL;
	static const std::string DELETE_NETWORK_LINKURLS;
	static const std::string FIND_NETWORK_LINKURLS;
	static const std::string DELETE_NETWORK_LINKURL;
	static const std::string UPDATE_NETWORK_LINKURL;

	static const std::string LOAD_NETWORK_LINKS;
	static const std::string LOAD_NETWORK_LINKURLS;

private: // disable creation Instances
	NetworkDBQuery();
};
#endif /* __NETWORKDBQUERY_H__ */
