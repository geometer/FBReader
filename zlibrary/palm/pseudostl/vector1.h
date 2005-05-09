#ifndef __VECTOR1_H__
#define __VECTOR1_H__

#include <sys_types.h>
#include <Core/System/MemoryMgr.h>

namespace std {

	template<typename T>
	class vector {

	public:
		struct const_iterator;

		struct iterator {
			iterator() STL_SECTION;
			bool operator == (const iterator &it) const STL_SECTION;
			bool operator == (const const_iterator &it) const STL_SECTION;
			bool operator != (const iterator &it) const STL_SECTION;
			bool operator != (const const_iterator &it) const STL_SECTION;

			const iterator &operator ++ (int) STL_SECTION;
			const iterator &operator += (size_t offset) STL_SECTION;
			const iterator &operator -- (int) STL_SECTION;
			const iterator &operator -= (size_t offset) STL_SECTION;
			const iterator operator + (size_t offset) const STL_SECTION;
			size_t operator - (const iterator &it) const STL_SECTION;
			size_t operator - (const const_iterator &it) const STL_SECTION;

			T &operator * () const STL_SECTION;
			T *operator -> () const STL_SECTION;

		private:
			T *__myPtr;
			iterator(T *ptr) STL_SECTION;

		friend struct const_iterator;
		friend class vector;
		};

		struct const_iterator {
			const_iterator() STL_SECTION;
			const_iterator(const iterator &it) STL_SECTION;

			bool operator == (const iterator &it) const STL_SECTION;
			bool operator == (const const_iterator &it) const STL_SECTION;
			bool operator != (const iterator &it) const STL_SECTION;
			bool operator != (const const_iterator &it) const STL_SECTION;

			const const_iterator &operator ++ (int) STL_SECTION;
			const const_iterator &operator += (size_t offset) STL_SECTION;
			const const_iterator &operator -- (int) STL_SECTION;
			const const_iterator operator + (size_t offset) const STL_SECTION;
			size_t operator - (const iterator &it) const STL_SECTION;
			size_t operator - (const const_iterator &it) const STL_SECTION;

			const T &operator * () const STL_SECTION;
			const T *operator -> () const STL_SECTION;

		private:
			T *__myPtr;
			const_iterator(T *ptr) STL_SECTION;

		friend struct iterator;
		friend class vector;
		};

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
	inline vector<T>::iterator::iterator() {
	}
	template<typename T>
	inline vector<T>::iterator::iterator(T *ptr) {
		__myPtr = ptr;
	}
	template<typename T>
	inline vector<T>::const_iterator::const_iterator() {
	}
	template<typename T>
	inline vector<T>::const_iterator::const_iterator(const vector<T>::iterator &it) {
		__myPtr = it.__myPtr;
	}
	template<typename T>
	inline vector<T>::const_iterator::const_iterator(T *ptr) {
		__myPtr = ptr;
	}

	template<typename T>
	inline bool vector<T>::iterator::operator == (const vector<T>::iterator &it) const {
		return __myPtr == it.__myPtr;
	}
	template<typename T>
	inline bool vector<T>::iterator::operator == (const vector<T>::const_iterator &it) const {
		return __myPtr == it.__myPtr;
	}
	template<typename T>
	inline bool vector<T>::const_iterator::operator == (const vector<T>::iterator &it) const {
		return __myPtr == it.__myPtr;
	}
	template<typename T>
	inline bool vector<T>::const_iterator::operator == (const vector<T>::const_iterator &it) const {
		return __myPtr == it.__myPtr;
	}

	template<typename T>
	inline bool vector<T>::iterator::operator != (const vector<T>::iterator &it) const {
		return !operator == (it);
	}
	template<typename T>
	inline bool vector<T>::iterator::operator != (const vector<T>::const_iterator &it) const {
		return !operator == (it);
	}
	template<typename T>
	inline bool vector<T>::const_iterator::operator != (const vector<T>::iterator &it) const {
		return !operator == (it);
	}
	template<typename T>
	inline bool vector<T>::const_iterator::operator != (const vector<T>::const_iterator &it) const {
		return !operator == (it);
	}

	template<typename T>
	inline const vector<T>::iterator &vector<T>::iterator::operator ++ (int) {
		__myPtr++;
		return *this;
	}
	template<typename T>
	inline const vector<T>::iterator &vector<T>::iterator::operator += (size_t offset) {
		__myPtr += offset;
		return *this;
	}
	template<typename T>
	inline const vector<T>::iterator &vector<T>::iterator::operator -= (size_t offset) {
		__myPtr -= offset;
		return *this;
	}
	template<typename T>
	inline const vector<T>::iterator &vector<T>::iterator::operator -- (int) {
		__myPtr--;
		return *this;
	}
	template<typename T>
	inline const vector<T>::iterator vector<T>::iterator::operator + (size_t offset) const {
		iterator it = *this;
		it.__myPtr += offset;
		return it;
	}
	template<typename T>
	inline size_t vector<T>::iterator::operator - (const vector<T>::iterator &it) const {
		return __myPtr - it.__myPtr;
	}
	template<typename T>
	inline size_t vector<T>::iterator::operator - (const vector<T>::const_iterator &it) const {
		return __myPtr - it.__myPtr;
	}
	template<typename T>
	inline const vector<T>::const_iterator &vector<T>::const_iterator::operator ++ (int) {
		__myPtr++;
		return *this;
	}
	template<typename T>
	inline const vector<T>::const_iterator &vector<T>::const_iterator::operator += (size_t offset) {
		__myPtr += offset;
		return *this;
	}
	template<typename T>
	inline const vector<T>::const_iterator &vector<T>::const_iterator::operator -- (int) {
		__myPtr--;
		return *this;
	}
	template<typename T>
	inline const vector<T>::const_iterator vector<T>::const_iterator::operator + (size_t offset) const {
		const_iterator it = *this;
		it.__myPtr += offset;
		return it;
	}
	template<typename T>
	inline size_t vector<T>::const_iterator::operator - (const vector<T>::iterator &it) const {
		return __myPtr - it.__myPtr;
	}
	template<typename T>
	inline size_t vector<T>::const_iterator::operator - (const vector<T>::const_iterator &it) const {
		return __myPtr - it.__myPtr;
	}

	template<typename T>
	inline T &vector<T>::iterator::operator * () const {
		return *__myPtr;
	}
	template<typename T>
	inline T *vector<T>::iterator::operator -> () const {
		return __myPtr;
	}
	template<typename T>
	inline const T &vector<T>::const_iterator::operator * () const {
		return *__myPtr;
	}
	template<typename T>
	inline const T *vector<T>::const_iterator::operator -> () const {
		return __myPtr;
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
		return iterator(__myData);
	}
	template<typename T>
	inline vector<T>::iterator vector<T>::end() const {
		return iterator(__myData + __myLength);
	}

	template<typename T>
	inline void vector<T>::clear() {
		__myLength = 0;
	}
	template<typename T>
	inline void vector<T>::reserve(size_t size) {
		if (size > __myDataSize) {
			if (size | 0xf) {
				size = (size & (size_t)-0x10) + 0x10;
			}
			__myDataSize = size;
			// TODO: use MemPtrResize
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
	/*
	template<typename T>
	void construct_copy(T &dst, const T &src) STL_SECTION;
	inline void construct_copy(unsigned short &dst, const unsigned short &src) {
		dst = src;
	}
	template<typename T>
	inline void construct_copy(T &dst, const T &src) {
		/ *
		T *copy = new T(src);
		MemMove(&dst, copy, sizeof(T));
		MemPtrFree(copy);
		* /
		dst(src);
	}
	*/
	template<typename T>
	inline void vector<T>::push_back(const T &element) {
		reserve(__myLength + 1);
		//__myData[__myLength] = element;
		//construct_copy(__myData[__myLength], element);
		T *copy = new T(element);
		MemMove(__myData + __myLength, copy, sizeof(T));
		MemPtrFree(copy);
		__myLength++;
	}
	template<typename T>
	inline void vector<T>::pop_back() {
		__myLength--;
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
