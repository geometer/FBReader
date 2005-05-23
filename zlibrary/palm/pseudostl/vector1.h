#ifndef __VECTOR1_H__
#define __VECTOR1_H__

#include <sys_types.h>
#include <Core/System/MemoryMgr.h>

#include <new>

namespace std {

	template<typename T>
	class vector {

	public:
		typedef T* iterator;
		typedef const T* const_iterator;

	public:
		vector() STL_SECTION;
		~vector() STL_SECTION;

		bool empty() const STL_SECTION;
		size_t size() const STL_SECTION;
		T& operator[] (size_t index) const STL_SECTION;
		T& front() const STL_SECTION;
		T& back() const STL_SECTION;

		iterator begin() const STL_SECTION;
		iterator end() const STL_SECTION;

		void clear() STL_SECTION;
		void reserve(size_t size) STL_SECTION;
		void push_back(const T &element) STL_SECTION;
		void pop_back() STL_SECTION;
		void insert(const iterator &position, const const_iterator &from, const const_iterator &to) STL_SECTION;
		void swap(vector &v) STL_SECTION;

	private:
		size_t __myDataSize;
		size_t __myLength;
		T *__myData;
	};

	template<typename T>
	vector<T>::const_iterator upper_bound(const vector<T>::const_iterator &start, const vector<T>::const_iterator &end, const T &value) STL_SECTION;
	template<typename T>
	vector<T>::const_iterator lower_bound(const vector<T>::const_iterator &start, const vector<T>::const_iterator &end, const T &value) STL_SECTION;
	template<typename Iterator, typename Comparator>
	void sort(const Iterator &startIterator, const Iterator &endIterator, const Comparator &comparator) STL_SECTION;

	template<typename T>
	inline vector<T>::vector() {
		__myDataSize = 0;
		__myLength = 0;
		__myData = 0;
	}
	template<typename T>
	inline vector<T>::~vector() {
		for (size_t i = 0; i < __myLength; i++) {
			__myData[i].~T();
		}
		if (__myData != 0) {
			MemPtrFree(__myData);
		}
	}

	template<typename T>
	inline bool vector<T>::empty() const {
		return __myLength == 0;
	}
	template<typename T>
	inline size_t vector<T>::size() const {
		return __myLength;
	}
	template<typename T>
	inline T& vector<T>::operator[] (size_t index) const {
		return __myData[index];
	}
	template<typename T>
	inline T& vector<T>::front() const {
		return __myData[0];
	}
	template<typename T>
	inline T& vector<T>::back() const {
		return __myData[__myLength - 1];
	}

	template<typename T>
	inline vector<T>::iterator vector<T>::begin() const {
		return __myData;
	}
	template<typename T>
	inline vector<T>::iterator vector<T>::end() const {
		return __myData + __myLength;
	}

	template<typename T>
	inline void vector<T>::clear() {
		for (size_t i = 0; i < __myLength; i++) {
			__myData[i].~T();
		}
		__myLength = 0;
	}
	template<typename T>
	inline void vector<T>::reserve(size_t size) {
		if (size > __myDataSize) {
			if (__myDataSize == 0) {
				__myDataSize = 0x10;
			}
			while (size > __myDataSize) {
				__myDataSize <<= 1;
			}
			if (__myData == 0) {
				__myData = (T*)MemPtrNew(__myDataSize * sizeof(T));
			} else if (MemPtrResize(__myData, __myDataSize * sizeof(T)) != errNone) {
				T *d = (T*)MemPtrNew(__myDataSize * sizeof(T));
				if (__myLength != 0) {
					MemMove(d, __myData, __myLength * sizeof(T));
				}
				if (__myData != 0) {
					MemPtrFree(__myData);
				}
				__myData = d;
			}
		}
	}
	template<typename T>
	inline void vector<T>::push_back(const T &element) {
		reserve(__myLength + 1);
    new (__myData + __myLength) T(element);
		__myLength++;
	}
	template<typename T>
	inline void vector<T>::pop_back() {
		__myLength--;
		__myData[__myLength].~T();
	}
	template<typename T>
	inline void vector<T>::insert(const vector<T>::iterator &position, const vector<T>::const_iterator &from, const vector<T>::const_iterator &to) {
		// TODO: implement
	}
	template<typename T>
	inline void vector<T>::swap(vector<T>::vector &v) {
		int l = __myLength; __myLength = v.__myLength; v.__myLength = l;
		int ds = __myDataSize; __myDataSize = v.__myDataSize; v.__myDataSize = ds;
		T *d = __myData; __myData = v.__myData; v.__myData = d;
	}

	template<typename T>
	inline vector<T>::const_iterator upper_bound(const vector<T>::const_iterator &start, const vector<T>::const_iterator &end, const T &value) {
		// TODO: implement
		return start;
	}
	template<typename T>
	inline vector<T>::const_iterator lower_bound(const vector<T>::const_iterator &start, const vector<T>::const_iterator &end, const T &value) {
		// TODO: implement
		return start;
	}
	template<typename Iterator, typename Comparator>
	void sort(const Iterator &startIterator, const Iterator &endIterator, const Comparator &comparator) {
		// TODO: implement
	}
};

#endif /* __VECTOR1_H__ */
