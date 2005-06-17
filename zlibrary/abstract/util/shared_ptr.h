#ifndef __SHARED_PTR_H__
#define __SHARED_PTR_H__

#ifdef __PTR_DEBUG__
#include <iostream>
#endif /* __PTR_DEBUG__ */

template<class T> class shared_ptr_storage {
	private:
		unsigned int myCounter;
		unsigned int myWeakCounter;
		T* myPointer;

	public:
		shared_ptr_storage(T *pointer) UTIL_SECTION;
		~shared_ptr_storage() UTIL_SECTION;

		T* pointer() const UTIL_SECTION;
		T& content() const UTIL_SECTION;

		void addReference() UTIL_SECTION;
		void removeReference() UTIL_SECTION;
		void addWeakReference() UTIL_SECTION;
		void removeWeakReference() UTIL_SECTION;
		unsigned int counter() const UTIL_SECTION;
};

template<class T> class weak_ptr;

template<class T> class shared_ptr {
	friend class weak_ptr<T>;

	private:
		shared_ptr_storage<T> *myStorage;

		shared_ptr_storage<T> *newStorage(T *t) UTIL_SECTION;
		void attachStorage(shared_ptr_storage<T> *storage) UTIL_SECTION;
		void detachStorage() UTIL_SECTION;

	public:
		shared_ptr() UTIL_SECTION;
		shared_ptr(T *t) UTIL_SECTION;
		shared_ptr(const shared_ptr<T> &t) UTIL_SECTION;
		shared_ptr(const weak_ptr<T> &t) UTIL_SECTION;
		~shared_ptr() UTIL_SECTION;

		const shared_ptr<T> &operator = (T *t) UTIL_SECTION;
		const shared_ptr<T> &operator = (const shared_ptr<T> &t) UTIL_SECTION;
		const shared_ptr<T> &operator = (const weak_ptr<T> &t) UTIL_SECTION;

		T* operator -> () const UTIL_SECTION;
		T& operator * () const UTIL_SECTION;
		bool isNull() const UTIL_SECTION;
		void reset() UTIL_SECTION;
		bool operator == (const weak_ptr<T> &t) const UTIL_SECTION;
		bool operator != (const weak_ptr<T> &t) const UTIL_SECTION;
		bool operator < (const weak_ptr<T> &t) const UTIL_SECTION;
		bool operator > (const weak_ptr<T> &t) const UTIL_SECTION;
		bool operator <= (const weak_ptr<T> &t) const UTIL_SECTION;
		bool operator >= (const weak_ptr<T> &t) const UTIL_SECTION;
		bool operator == (const shared_ptr<T> &t) const UTIL_SECTION;
		bool operator != (const shared_ptr<T> &t) const UTIL_SECTION;
		bool operator < (const shared_ptr<T> &t) const UTIL_SECTION;
		bool operator > (const shared_ptr<T> &t) const UTIL_SECTION;
		bool operator <= (const shared_ptr<T> &t) const UTIL_SECTION;
		bool operator >= (const shared_ptr<T> &t) const UTIL_SECTION;
};

template<class T> class weak_ptr {
	friend class shared_ptr<T>;
	private:
		shared_ptr_storage<T> *myStorage;

		void attachStorage(shared_ptr_storage<T> *storage) {
			myStorage = storage;
			if (myStorage != 0) {
				myStorage->addWeakReference();
			}
		}
		void detachStorage() {
			if (myStorage != 0) {
				myStorage->removeWeakReference();
				if (myStorage->counter() == 0) {
					delete myStorage;
				}
			}
		}

	public:
		weak_ptr() {
			myStorage = 0;
		}
		weak_ptr(const shared_ptr<T> &t) {
			attachStorage(t.myStorage);
		}
		weak_ptr(const weak_ptr<T> &t) {
			if (!t.isNull()) {
				attachStorage(t.myStorage);
			} else {
				attachStorage(0);
			}
		}
		~weak_ptr() {
			detachStorage();
		}

		const weak_ptr<T> &operator = (const weak_ptr<T> &t) {
			if (&t != this) {
				detachStorage();
				if (!t.isNull()) {
					attachStorage(t.myStorage);
				} else {
					attachStorage(0);
				}
			}
			return *this;
		}
		const weak_ptr<T> &operator = (const shared_ptr<T> &t) {
			detachStorage();
			attachStorage(t.myStorage);
			return *this;
		}

		T* operator -> () const {
			return (myStorage == 0) ? 0 : myStorage->pointer();
		}
		T& operator * () const {
			return myStorage->content();
		}
		bool isNull() const {
			return (myStorage == 0) || (myStorage->pointer() == 0);
		}
		void reset() {
			detachStorage();
			attachStorage(0);
		}
		bool operator == (const weak_ptr<T> &t) const {
			return operator -> () == t.operator -> ();
		}
		bool operator != (const weak_ptr<T> &t) const {
			return !operator == (t);
		}
		bool operator < (const weak_ptr<T> &t) const {
			return operator -> () < t.operator -> ();
		}
		bool operator > (const weak_ptr<T> &t) const {
			return t.operator < (*this);
		}
		bool operator <= (const weak_ptr<T> &t) const {
			return !t.operator < (*this);
		}
		bool operator >= (const weak_ptr<T> &t) const {
			return !operator < (t);
		}
		bool operator == (const shared_ptr<T> &t) const {
			return operator -> () == t.operator -> ();
		}
		bool operator != (const shared_ptr<T> &t) const {
			return !operator == (t);
		}
		bool operator < (const shared_ptr<T> &t) const {
			return operator -> () < t.operator -> ();
		}
		bool operator > (const shared_ptr<T> &t) const {
			return t.operator < (*this);
		}
		bool operator <= (const shared_ptr<T> &t) const {
			return !t.operator < (*this);
		}
		bool operator >= (const shared_ptr<T> &t) const {
			return !operator < (t);
		}
};

template<class T>
inline shared_ptr_storage<T>::shared_ptr_storage(T *pointer) {
	myPointer = pointer;
	myCounter = 0;
	myWeakCounter = 0;
#ifdef __PTR_DEBUG__
	std::cerr << "new storage " << ((myPointer == 0) ? "0" : "not 0") << "\n";
#endif /* __PTR_DEBUG__ */
}
template<class T>
inline shared_ptr_storage<T>::~shared_ptr_storage() {
#ifdef __PTR_DEBUG__
	std::cerr << "delete storage " << ((myPointer == 0) ? "0" : "not 0") << "\n";
#endif /* __PTR_DEBUG__ */
}
template<class T>
inline T* shared_ptr_storage<T>::pointer() const {
	return myPointer;
}
template<class T>
inline T& shared_ptr_storage<T>::content() const {
	return *myPointer;
}
template<class T>
inline void shared_ptr_storage<T>::addReference() {
#ifdef __PTR_DEBUG__
	std::cerr << "add reference\n";
#endif /* __PTR_DEBUG__ */
	myCounter++;
}
template<class T>
inline void shared_ptr_storage<T>::removeReference() {
#ifdef __PTR_DEBUG__
	std::cerr << "remove reference\n";
#endif /* __PTR_DEBUG__ */
	myCounter--;
	if (myCounter == 0) {
		T* ptr = myPointer;
		myPointer = 0;
		delete ptr;
	}
}
template<class T>
inline void shared_ptr_storage<T>::addWeakReference() {
	myWeakCounter++;
}
template<class T>
inline void shared_ptr_storage<T>::removeWeakReference() {
	myWeakCounter--;
}
template<class T>
inline unsigned int shared_ptr_storage<T>::counter() const {
	return myCounter + myWeakCounter;
}

template<class T>
inline shared_ptr_storage<T> *shared_ptr<T>::newStorage(T *t) {
	return (t == 0) ? 0 : new shared_ptr_storage<T>(t);
}
template<class T>
inline void shared_ptr<T>::attachStorage(shared_ptr_storage<T> *storage) {
	myStorage = storage;
	if (myStorage != 0) {
		myStorage->addReference();
	}
}
template<class T>
inline void shared_ptr<T>::detachStorage() {
	if (myStorage != 0) {
		myStorage->removeReference();
		if (myStorage->counter() == 0) {
			delete myStorage;
		}
	}
}

template<class T>
inline shared_ptr<T>::shared_ptr() {
	myStorage = 0;
}
template<class T>
inline shared_ptr<T>::shared_ptr(T *t) {
	attachStorage(newStorage(t));
}
template<class T>
inline shared_ptr<T>::shared_ptr(const shared_ptr<T> &t) {
	attachStorage(t.myStorage);
}
template<class T>
inline shared_ptr<T>::shared_ptr(const weak_ptr<T> &t) {
	if (!t.isNull()) {
		attachStorage(t.myStorage);
	} else {
		attachStorage(0);
	}
}
template<class T>
inline shared_ptr<T>::~shared_ptr() {
	detachStorage();
}
template<class T>
inline const shared_ptr<T> &shared_ptr<T>::operator = (T *t) {
	detachStorage();
	attachStorage(newStorage(t));
	return *this;
}
template<class T>
inline const shared_ptr<T> &shared_ptr<T>::operator = (const shared_ptr<T> &t) {
	if (&t != this) {
		detachStorage();
		attachStorage(t.myStorage);
	}
	return *this;
}
template<class T>
inline const shared_ptr<T> &shared_ptr<T>::operator = (const weak_ptr<T> &t) {
	detachStorage();
	if (!t.isNull()) {
		attachStorage(t.myStorage);
	} else {
		attachStorage(0);
	}
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
	detachStorage();
	attachStorage(0);
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

#endif /* __SHARED_PTR_H__ */
