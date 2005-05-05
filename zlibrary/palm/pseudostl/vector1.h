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
			const iterator &operator + (size_t offset) const STL_SECTION;
			size_t operator - (const iterator &it) const STL_SECTION;
			size_t operator - (const const_iterator &it) const STL_SECTION;

			T &operator * () const STL_SECTION;
			T *operator -> () const STL_SECTION;

		private:
			T *myPtr;
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
			const const_iterator &operator + (size_t offset) const STL_SECTION;
			size_t operator - (const iterator &it) const STL_SECTION;
			size_t operator - (const const_iterator &it) const STL_SECTION;

			const T &operator * () const STL_SECTION;
			const T *operator -> () const STL_SECTION;

		private:
			T *myPtr;
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
		size_t myDataSize;
		size_t myLength;
		T *myData;
	};

	template<typename T>
	vector<T>::const_iterator upper_bound(const vector<T>::const_iterator &start, const vector<T>::const_iterator &end, const T &value) STL_SECTION;
	template<typename T>
	vector<T>::const_iterator lower_bound(const vector<T>::const_iterator &start, const vector<T>::const_iterator &end, const T &value) STL_SECTION;
	template<typename T, typename S>
	void sort(const vector<T>::iterator &start, const vector<T>::iterator &end, const S &comparator) STL_SECTION;

	template<typename T>
	inline vector<T>::vector() {
		myDataSize = 0;
		myLength = 0;
		myData = 0;
	}
	template<typename T>
	inline vector<T>::~vector() {
		if (myData != 0) {
			delete[] myData;
		}
	}

	template<typename T>
	inline vector<T>::iterator::iterator() {
	}
	template<typename T>
	inline vector<T>::iterator::iterator(T *ptr) {
		myPtr = ptr;
	}
	template<typename T>
	inline vector<T>::const_iterator::const_iterator() {
	}
	template<typename T>
	inline vector<T>::const_iterator::const_iterator(const vector<T>::iterator &it) {
		myPtr = it.myPtr;
	}
	template<typename T>
	inline vector<T>::const_iterator::const_iterator(T *ptr) {
		myPtr = ptr;
	}

	template<typename T>
	inline bool vector<T>::iterator::operator == (const vector<T>::iterator &it) const {
		return myPtr == it.myPtr;
	}
	template<typename T>
	inline bool vector<T>::iterator::operator == (const vector<T>::const_iterator &it) const {
		return myPtr == it.myPtr;
	}
	template<typename T>
	inline bool vector<T>::const_iterator::operator == (const vector<T>::iterator &it) const {
		return myPtr == it.myPtr;
	}
	template<typename T>
	inline bool vector<T>::const_iterator::operator == (const vector<T>::const_iterator &it) const {
		return myPtr == it.myPtr;
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
		myPtr++;
		return *this;
	}
	template<typename T>
	inline const vector<T>::iterator &vector<T>::iterator::operator += (size_t offset) {
		myPtr += offset;
		return *this;
	}
	template<typename T>
	inline const vector<T>::iterator &vector<T>::iterator::operator -- (int) {
		myPtr--;
		return *this;
	}
	template<typename T>
	inline const vector<T>::iterator &vector<T>::iterator::operator + (size_t offset) const {
		iterator it = *this;
		it.myPtr += offset;
		return it;
	}
	template<typename T>
	inline size_t vector<T>::iterator::operator - (const vector<T>::iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T>
	inline size_t vector<T>::iterator::operator - (const vector<T>::const_iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T>
	inline const vector<T>::const_iterator &vector<T>::const_iterator::operator ++ (int) {
		myPtr++;
		return *this;
	}
	template<typename T>
	inline const vector<T>::const_iterator &vector<T>::const_iterator::operator += (size_t offset) {
		myPtr += offset;
		return *this;
	}
	template<typename T>
	inline const vector<T>::const_iterator &vector<T>::const_iterator::operator -- (int) {
		myPtr--;
		return *this;
	}
	template<typename T>
	inline const vector<T>::const_iterator &vector<T>::const_iterator::operator + (size_t offset) const {
		const_iterator it = *this;
		it.myPtr += offset;
		return it;
	}
	template<typename T>
	inline size_t vector<T>::const_iterator::operator - (const vector<T>::iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T>
	inline size_t vector<T>::const_iterator::operator - (const vector<T>::const_iterator &it) const {
		return myPtr - it.myPtr;
	}

	template<typename T>
	inline T &vector<T>::iterator::operator * () const {
		return *myPtr;
	}
	template<typename T>
	inline T *vector<T>::iterator::operator -> () const {
		return myPtr;
	}
	template<typename T>
	inline const T &vector<T>::const_iterator::operator * () const {
		return *myPtr;
	}
	template<typename T>
	inline const T *vector<T>::const_iterator::operator -> () const {
		return myPtr;
	}

	template<typename T>
	inline bool vector<T>::empty() const {
		return myLength == 0;
	}
	template<typename T>
	inline size_t vector<T>::size() const {
		return myLength;
	}
	template<typename T>
	inline T& vector<T>::operator[] (size_t index) const {
		return myData[index];
	}
	template<typename T>
	inline T& vector<T>::front() const {
		return myData[0];
	}
	template<typename T>
	inline T& vector<T>::back() const {
		return myData[myLength - 1];
	}

	template<typename T>
	inline vector<T>::iterator vector<T>::begin() const {
		return iterator(myData);
	}
	template<typename T>
	inline vector<T>::iterator vector<T>::end() const {
		return iterator(myData + myLength - 1);
	}

	template<typename T>
	inline void vector<T>::clear() {
		myLength = 0;
	}
	template<typename T>
	inline void vector<T>::reserve(size_t size) {
		if (size > myDataSize) {
			if (size | 0xf) {
				size = (size & (size_t)-0x10) + 0x10;
			}
			myDataSize = size;
			// TODO: use MemPtrResize
			T *d = (T*)MemPtrNew(myDataSize * sizeof(T));
			for (size_t i = 0; i < myLength; i++) {
				d[i] = myData[i];
			}
			if (myData != 0) {
				MemPtrFree(myData);
			}
			myData = d;
		}
	}
	template<typename T>
	inline void vector<T>::push_back(const T &element) {
		reserve(myLength + 1);
		myData[myLength] = element;
		myLength++;
	}
	template<typename T>
	inline void vector<T>::pop_back() {
		myLength--;
	}
	template<typename T>
	inline void vector<T>::insert(const vector<T>::iterator &position, const vector<T>::const_iterator &from, const vector<T>::const_iterator &to) {
		// TODO: implement
	}
	template<typename T>
	inline void vector<T>::swap(vector<T>::vector &v) {
		int l = myLength; myLength = v.myLength; v.myLength = l;
		int ds = myDataSize; myDataSize = v.myDataSize; v.myDataSize = ds;
		T *d = myData; myData = v.myData; v.myData = d;
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
	template<typename T, typename S>
	inline void sort(const vector<T>::iterator &start, const vector<T>::iterator &end, const S &comparator) {
		// TODO: implement
	}
};

#endif /* __VECTOR1_H__ */
