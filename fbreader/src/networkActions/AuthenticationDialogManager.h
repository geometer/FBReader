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

#ifndef __AUTHENTICATIONDIALOGMANAGER_H__
#define __AUTHENTICATIONDIALOGMANAGER_H__

#include <ZLNetworkRequest.h>

#include "../network/authentication/NetworkAuthenticationManager.h"

class AuthenticationDialogManager {

public:
	static std::string authAndInitAsync(NetworkAuthenticationManager &manager, shared_ptr<ZLNetworkRequest::Listener> listener);
	static std::string athoriseIfCan(NetworkAuthenticationManager &manager, shared_ptr<ZLNetworkRequest::Listener> listener);

private:
	AuthenticationDialogManager() {}
};

#endif /* __AUTHENTICATIONDIALOGMANAGER_H__ */
