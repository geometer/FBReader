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

#ifndef __ZLEXECUTIONUTIL_H__
#define __ZLEXECUTIONUTIL_H__

#include <ZLUserData.h>
#include "ZLNetworkRequest.h"

class ZLExecutionUtil {
	//code for convenient async syntax createListener method:
	public:
		template <typename T, typename Method>
		static shared_ptr<ZLNetworkRequest::Listener> createListener(shared_ptr<ZLUserDataHolder> data, T object, Method method) {
			data->addUserData(ourHandlerId, new HandlerHelper<T, Method>(object, method));
			return createListener(data);
		}
		template <typename T, typename Method>
		static shared_ptr<ZLNetworkRequest::Listener> createListener(T object, Method method) {
			return createListener(new ZLUserDataHolder, object, method);
		}
		template <typename T, typename Method>
		static shared_ptr<ZLNetworkRequest::Listener> createListener(ZLUserData *scope, T object, Method method) {
			shared_ptr<ZLUserDataHolder> scopeData = new ZLUserDataHolder;
			scopeData->addUserData("scope", scope);
			return createListener(scopeData, object, method);
		}

	private:
		typedef void (*Handler)(ZLUserDataHolder &data, const std::string &error);

		class AbstractHandlerHelper : public ZLUserData {
		public:
			virtual void handle(ZLUserDataHolder &data, const std::string &error) = 0;
		};

		template <typename T, typename Method>
		class HandlerHelper : public AbstractHandlerHelper {
		public:
			HandlerHelper(T object, Method method) : myObject(object), myMethod(method) {
			}

			void handle(ZLUserDataHolder &data, const std::string &error) {
				(myObject->*myMethod)(data, error);
			}

		private:
			T myObject;
			Method myMethod;
		};
		static void handleHelper(ZLUserDataHolder &data, const std::string &error);
		static shared_ptr<ZLNetworkRequest::Listener> createListener(shared_ptr<ZLUserDataHolder> data);

		static std::string ourHandlerId;

friend class ZLExecutionHandler;
};



#endif /* __ZLEXECUTIONUTIL_H__ */
