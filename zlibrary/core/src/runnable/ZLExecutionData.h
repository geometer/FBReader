/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLEXECUTIONDATA_H__
#define __ZLEXECUTIONDATA_H__

#include <string>
#include <vector>
#include <set>

#include <shared_ptr.h>

#include <ZLTypeId.h>
#include <ZLUserData.h>

class ZLExecutionData : public ZLUserDataHolder, public ZLObjectWithRTTI {

public:
	typedef std::vector<shared_ptr<ZLExecutionData> > Vector;

public:
	static const ZLTypeId TYPE_ID;

public:
	class Runner {

	protected:
		Runner();
		virtual ~Runner();

	public:
		virtual std::string perform(const Vector &dataVector) const = 0;
		std::string perform(shared_ptr<ZLExecutionData> data) const;
	};

public:
	typedef void (*Handler)(ZLUserDataHolder &data, const std::string &error);
	
	class Listener : public ZLUserData {

	protected:
		Listener();

	public:
		virtual ~Listener();

		void cancelProcess();
		virtual void showPercent(int ready, int full) = 0;
		// if variable named 'error' is empty there is no error
		virtual void finished(const std::string &error = std::string()) { (void)error; }

	private:
		ZLExecutionData *myProcess;

	friend class ZLExecutionData;
	};
	
private:
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
	static shared_ptr<Listener> createListener(shared_ptr<ZLUserDataHolder> data);

public:
	static std::string perform(shared_ptr<ZLExecutionData> data);
	static std::string perform(const Vector &dataVector);

private:
	static std::set<Runner*> ourRunners;
	static std::string ourHandlerId;

protected:
	ZLExecutionData();

public:
	virtual ~ZLExecutionData();
	
	bool hasListener() const;
	void setListener(shared_ptr<Listener> listener);
	void setHandler(Handler handler);
	template <typename T, typename Method>
	inline void setHandler(T object, Method method) {
		addUserData(ourHandlerId, new HandlerHelper<T, Method>(object, method));
		setHandler(handleHelper);
	}
	template <typename T, typename Method>
	static shared_ptr<Listener> createListener(shared_ptr<ZLUserDataHolder> data, T object, Method method) {
		data->addUserData(ourHandlerId, new HandlerHelper<T, Method>(object, method));
		return createListener(data);
	}
	template <typename T, typename Method>
	static shared_ptr<Listener> createListener(T object, Method method) {
		return createListener(new ZLUserDataHolder, object, method);
	}

	virtual void onCancel();
	void setPercent(int ready, int full);
	
protected:
	void finished(const std::string &error);
	shared_ptr<Listener> listener() const;

private:
	shared_ptr<Listener> myListener;
};

#endif /* __ZLEXECUTIONDATA_H__ */
