/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __SHARED_PTR_H__
#define __SHARED_PTR_H__

class shared_ptr_counter {
	public:
		shared_ptr_counter();
		~shared_ptr_counter();
		
		void addReference();
		bool removeReference();
		void addWeakReference();
		bool removeWeakReference();
		unsigned int counter() const;
		unsigned int weakCounter() const;

	private:
		unsigned int myCounter;
		unsigned int myWeakCounter;
};

template<class T> class shared_ptr_storage {
	template <class V> friend class shared_ptr_storage;
	private:
		shared_ptr_storage(T *pointer, shared_ptr_counter *counter);
		T* myPointer;
		shared_ptr_counter *myCounter;

	public:
		shared_ptr_storage(T *pointer);
		~shared_ptr_storage();

		T* pointer() const;
		T& content() const;
		
		template<class V>
		shared_ptr_storage<V> *staticCast() const {
			if (myCounter->counter() == 0)
				return 0;
			V *ptr = static_cast<V*>(myPointer);
			return new shared_ptr_storage<V>(ptr, myCounter);
		}

		void addReference();
		bool removeReference();
		void addWeakReference();
		bool removeWeakReference();
		unsigned int counter() const;
};

template<class T> class weak_ptr;

template<class T> class shared_ptr {
	friend class weak_ptr<T>;
	template <class V> friend class shared_ptr;
//	template <class V> friend class enable_shared_from_this;
//	template <class V> void __shared_from_this_helper(shared_ptr<V> &storage, V *pointer, ...);

	private:
		shared_ptr_storage<T> *myStorage;

		static shared_ptr_storage<T> *newStorage(T *t);
		void setStorage(shared_ptr_storage<T> *storage);

	public:
		shared_ptr();
		shared_ptr(T *t);
		shared_ptr(const shared_ptr<T> &t);
		shared_ptr(const weak_ptr<T> &t);
		~shared_ptr();

		const shared_ptr<T> &operator = (T *t);
		const shared_ptr<T> &operator = (const shared_ptr<T> &t);
		const shared_ptr<T> &operator = (const weak_ptr<T> &t);
		
		template<class V>
		shared_ptr<V> staticCast() const {
			shared_ptr<V> v;
			v.setStorage(myStorage->staticCast<V>());
			return v;
		}

		T* operator -> () const;
		T& operator * () const;
		bool isNull() const;
		void reset();
		bool operator == (const weak_ptr<T> &t) const;
		bool operator != (const weak_ptr<T> &t) const;
		bool operator < (const weak_ptr<T> &t) const;
		bool operator > (const weak_ptr<T> &t) const;
		bool operator <= (const weak_ptr<T> &t) const;
		bool operator >= (const weak_ptr<T> &t) const;
		bool operator == (const shared_ptr<T> &t) const;
		bool operator != (const shared_ptr<T> &t) const;
		bool operator < (const shared_ptr<T> &t) const;
		bool operator > (const shared_ptr<T> &t) const;
		bool operator <= (const shared_ptr<T> &t) const;
		bool operator >= (const shared_ptr<T> &t) const;
};

template<class T> class weak_ptr {
	friend class shared_ptr<T>;
	private:
		shared_ptr_storage<T> *myStorage;

		void setStorage(shared_ptr_storage<T> *storage);

	public:
		weak_ptr();
		weak_ptr(const shared_ptr<T> &t);
		weak_ptr(const weak_ptr<T> &t);
		~weak_ptr();

		const weak_ptr<T> &operator = (const weak_ptr<T> &t);
		const weak_ptr<T> &operator = (const shared_ptr<T> &t);

		T* operator -> () const;
		T& operator * () const;
		bool isNull() const;
		void reset();

		bool operator == (const weak_ptr<T> &t) const;
		bool operator != (const weak_ptr<T> &t) const;
		bool operator < (const weak_ptr<T> &t) const;
		bool operator > (const weak_ptr<T> &t) const;
		bool operator <= (const weak_ptr<T> &t) const;
		bool operator >= (const weak_ptr<T> &t) const;
		bool operator == (const shared_ptr<T> &t) const;
		bool operator != (const shared_ptr<T> &t) const;
		bool operator < (const shared_ptr<T> &t) const;
		bool operator > (const shared_ptr<T> &t) const;
		bool operator <= (const shared_ptr<T> &t) const;
		bool operator >= (const shared_ptr<T> &t) const;
};

//template<class T> class enable_shared_from_this {
//public:
//	shared_ptr<T> shared_from_this();

//private:
//	weak_ptr<T> myThisWeak;
//};

inline shared_ptr_counter::shared_ptr_counter() : myCounter(0), myWeakCounter(0) {
}
inline shared_ptr_counter::~shared_ptr_counter() {
}
inline void shared_ptr_counter::addReference() {
	myCounter++;
}
inline bool shared_ptr_counter::removeReference() {
	return (--myCounter) == 0;
}
inline void shared_ptr_counter::addWeakReference() {
	myWeakCounter++;
}
inline bool shared_ptr_counter::removeWeakReference() {
	return (--myWeakCounter) == 0;
}
inline unsigned int shared_ptr_counter::counter() const {
	return myCounter;
}
inline unsigned int shared_ptr_counter::weakCounter() const {
	return myWeakCounter;
}
template<class T>
inline shared_ptr_storage<T>::shared_ptr_storage(T *pointer) : myPointer(pointer), myCounter(new shared_ptr_counter) {
}
template<class T>
inline shared_ptr_storage<T>::shared_ptr_storage(T *pointer, shared_ptr_counter *counter) : myPointer(pointer), myCounter(counter) {
}
template<class T>
inline shared_ptr_storage<T>::~shared_ptr_storage() {
	if (myCounter->counter() + myCounter->weakCounter() == 0)
		delete myCounter;
}
template<class T>
inline T* shared_ptr_storage<T>::pointer() const {
	return myCounter->counter() == 0 ? 0 : myPointer;
}
template<class T>
inline T& shared_ptr_storage<T>::content() const {
	return *pointer();
}
template<class T>
inline void shared_ptr_storage<T>::addReference() {
	myCounter->addReference();
}
template<class T>
inline bool shared_ptr_storage<T>::removeReference() {
	if (myCounter->removeReference()) {
		char checkIfValid[sizeof(T) ? 1 : -1];
		(void) checkIfValid;
		T* ptr = myPointer;
		myPointer = 0;
		delete ptr;
		return counter() == 0;
	}
	return false;
}
template<class T>
inline void shared_ptr_storage<T>::addWeakReference() {
	myCounter->addWeakReference();
}
template<class T>
inline bool shared_ptr_storage<T>::removeWeakReference() {
	myCounter->removeWeakReference();
	return counter() == 0;
}
template<class T>
inline unsigned int shared_ptr_storage<T>::counter() const {
	return myCounter->counter() + myCounter->weakCounter();
}

//template<class T>
//shared_ptr<T> enable_shared_from_this<T>::shared_from_this() {
//	shared_ptr<T> shared = myThisWeak;
//	if (shared.isNull()) {
//		shared.setStorage(shared.newStorage(this));
//		myThisWeak = shared;
//	}
//	return shared;
//}

//template<class T>
//void __shared_from_this_helper(shared_ptr<T> &storage, T *, enable_shared_from_this<T> *pointer) {
//	storage = pointer->shared_from_this();
//}
//template<class T>
//void __shared_from_this_helper(shared_ptr<T> &storage, T *pointer, ...) {
//	storage.setStorage(storage.newStorage(pointer));
//}

template<class T>
inline shared_ptr_storage<T> *shared_ptr<T>::newStorage(T *t) {
	return (t == 0) ? 0 : new shared_ptr_storage<T>(t);
}
template<class T>
void shared_ptr<T>::setStorage(shared_ptr_storage<T> *storage) {
	if (myStorage == storage)
		return;
	if (myStorage && myStorage->removeReference()) {
		delete myStorage;
	}
	myStorage = storage;
	if (myStorage) {
		myStorage->addReference();
	}
}

template<class T>
inline shared_ptr<T>::shared_ptr() : myStorage(0) {
}
template<class T>
inline shared_ptr<T>::shared_ptr(T *t) : myStorage(0) {
	setStorage(newStorage(t));
//	*this = __shared_from_this_helper(t);
//	__shared_from_this_helper(*this, t, t);
}
template<class T>
inline shared_ptr<T>::shared_ptr(const shared_ptr<T> &t) : myStorage(0) {
	setStorage(t.myStorage);
}
template<class T>
inline shared_ptr<T>::shared_ptr(const weak_ptr<T> &t) : myStorage(0) {
	if (!t.isNull())
		setStorage(t.myStorage);
}
template<class T>
inline shared_ptr<T>::~shared_ptr() {
	setStorage(0);
}
template<class T>
inline const shared_ptr<T> &shared_ptr<T>::operator = (T *t) {
	setStorage(newStorage(t));
	return *this;
}
template<class T>
inline const shared_ptr<T> &shared_ptr<T>::operator = (const shared_ptr<T> &t) {
	setStorage(t.myStorage);
	return *this;
}
template<class T>
inline const shared_ptr<T> &shared_ptr<T>::operator = (const weak_ptr<T> &t) {
	setStorage(t.isNull() ? 0 : t.myStorage);
	return *this;
}

template<class T>
inline T* shared_ptr<T>::operator -> () const {
	return (myStorage == 0) ? 0 : myStorage->pointer();
}
template<class T>
inline T& shared_ptr<T>::operator * () const {
	return myStorage->content();
}
template<class T>
inline bool shared_ptr<T>::isNull() const {
	return myStorage == 0;
}
template<class T>
inline void shared_ptr<T>::reset() {
	setStorage(0);
}
template<class T>
inline bool shared_ptr<T>::operator == (const weak_ptr<T> &t) const {
	return operator -> () == t.operator -> ();
}
template<class T>
inline bool shared_ptr<T>::operator != (const weak_ptr<T> &t) const {
	return !operator == (t);
}
template<class T>
inline bool shared_ptr<T>::operator < (const weak_ptr<T> &t) const {
	return operator -> () < t.operator -> ();
}
template<class T>
inline bool shared_ptr<T>::operator > (const weak_ptr<T> &t) const {
	return t.operator < (*this);
}
template<class T>
inline bool shared_ptr<T>::operator <= (const weak_ptr<T> &t) const {
	return !t.operator < (*this);
}
template<class T>
inline bool shared_ptr<T>::operator >= (const weak_ptr<T> &t) const {
	return !operator < (t);
}
template<class T>
inline bool shared_ptr<T>::operator == (const shared_ptr<T> &t) const {
	return operator -> () == t.operator -> ();
}
template<class T>
inline bool shared_ptr<T>::operator != (const shared_ptr<T> &t) const {
	return !operator == (t);
}
template<class T>
inline bool shared_ptr<T>::operator < (const shared_ptr<T> &t) const {
	return operator -> () < t.operator -> ();
}
template<class T>
inline bool shared_ptr<T>::operator > (const shared_ptr<T> &t) const {
	return t.operator < (*this);
}
template<class T>
inline bool shared_ptr<T>::operator <= (const shared_ptr<T> &t) const {
	return !t.operator < (*this);
}
template<class T>
inline bool shared_ptr<T>::operator >= (const shared_ptr<T> &t) const {
	return !operator < (t);
}

template<class T>
void weak_ptr<T>::setStorage(shared_ptr_storage<T> *storage) {
	if (myStorage == storage)
		return;
	if (myStorage && myStorage->removeWeakReference()) {
		delete myStorage;
	}
	myStorage = storage;
	if (myStorage) {
		myStorage->addWeakReference();
	}
}

template<class T>
inline weak_ptr<T>::weak_ptr() : myStorage(0) {
}
template<class T>
inline weak_ptr<T>::weak_ptr(const shared_ptr<T> &t) : myStorage(0) {
	setStorage(t.myStorage);
}
template<class T>
inline weak_ptr<T>::weak_ptr(const weak_ptr<T> &t) : myStorage(0) {
	if (!t.isNull()) {
		setStorage(t.myStorage);
	}
}
template<class T>
inline weak_ptr<T>::~weak_ptr() {
	setStorage(0);
}

template<class T>
inline const weak_ptr<T> &weak_ptr<T>::operator = (const weak_ptr<T> &t) {
	setStorage(t.isNull() ? 0 : t.myStorage);
	return *this;
}
template<class T>
inline const weak_ptr<T> &weak_ptr<T>::operator = (const shared_ptr<T> &t) {
	setStorage(t.myStorage);
	return *this;
}

template<class T>
inline T* weak_ptr<T>::operator -> () const {
	return (myStorage == 0) ? 0 : myStorage->pointer();
}
template<class T>
inline T& weak_ptr<T>::operator * () const {
	return myStorage->content();
}
template<class T>
inline bool weak_ptr<T>::isNull() const {
	return (myStorage == 0) || (myStorage->pointer() == 0);
}
template<class T>
inline void weak_ptr<T>::reset() {
	setStorage(0);
}
template<class T>
inline bool weak_ptr<T>::operator == (const weak_ptr<T> &t) const {
	return operator -> () == t.operator -> ();
}
template<class T>
inline bool weak_ptr<T>::operator != (const weak_ptr<T> &t) const {
	return !operator == (t);
}
template<class T>
inline bool weak_ptr<T>::operator < (const weak_ptr<T> &t) const {
	return operator -> () < t.operator -> ();
}
template<class T>
inline bool weak_ptr<T>::operator > (const weak_ptr<T> &t) const {
	return t.operator < (*this);
}
template<class T>
inline bool weak_ptr<T>::operator <= (const weak_ptr<T> &t) const {
	return !t.operator < (*this);
}
template<class T>
inline bool weak_ptr<T>::operator >= (const weak_ptr<T> &t) const {
	return !operator < (t);
}
template<class T>
inline bool weak_ptr<T>::operator == (const shared_ptr<T> &t) const {
	return operator -> () == t.operator -> ();
}
template<class T>
inline bool weak_ptr<T>::operator != (const shared_ptr<T> &t) const {
	return !operator == (t);
}
template<class T>
inline bool weak_ptr<T>::operator < (const shared_ptr<T> &t) const {
	return operator -> () < t.operator -> ();
}
template<class T>
inline bool weak_ptr<T>::operator > (const shared_ptr<T> &t) const {
	return t.operator < (*this);
}
template<class T>
inline bool weak_ptr<T>::operator <= (const shared_ptr<T> &t) const {
	return !t.operator < (*this);
}
template<class T>
inline bool weak_ptr<T>::operator >= (const shared_ptr<T> &t) const {
	return !operator < (t);
}

#endif /* __SHARED_PTR_H__ */
