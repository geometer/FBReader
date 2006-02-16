/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __QUEUE1_H__
#define __QUEUE1_H__

#include <sys_types.h>

namespace std {

	template<typename T>
	class queue {

		/*
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

		friend struct const_iterator;
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

		friend struct iterator;
		};
	*/

	public:
		queue() STL_SECTION;
		~queue() STL_SECTION;

		bool empty() const STL_SECTION;
		size_t size() const STL_SECTION;
		T& front() const STL_SECTION;

		void push(const T &element) STL_SECTION;
		void pop() STL_SECTION;
		/*
		T& operator[] (size_t index) const STL_SECTION;
		T& back() const STL_SECTION;

		iterator begin() const STL_SECTION;
		iterator end() const STL_SECTION;

		void clear() STL_SECTION;
		void reserve(size_t size) STL_SECTION;
		void push_back(const T &element) STL_SECTION;
		void insert(const iterator &position, const const_iterator &from, const const_iterator &to) STL_SECTION;
		void swap(queue &v) STL_SECTION;
		*/

	private:
		/*
		size_t myDataSize;
		size_t myLength;
		T *myData;
		*/
	};

	template<typename T>
	inline queue<T>::queue() {
		// TODO: implement
		/*
		myDataSize = 0;
		myLength = 0;
		myData = 0;
		*/
	}
	template<typename T>
	inline queue<T>::~queue() {
		// TODO: implement
		/*
		if (myData != 0) {
			delete[] myData;
		}
		*/
	}

	/*
	template<typename T>
	inline queue<T>::iterator::iterator() {
	}
	template<typename T>
	inline queue<T>::const_iterator::const_iterator() {
	}
	template<typename T>
	inline queue<T>::const_iterator::const_iterator(const queue<T>::iterator &it) {
		myPtr = it.myPtr;
	}

	template<typename T>
	inline bool queue<T>::iterator::operator == (const queue<T>::iterator &it) const {
		return myPtr == it.myPtr;
	}
	template<typename T>
	inline bool queue<T>::iterator::operator == (const queue<T>::const_iterator &it) const {
		return myPtr == it.myPtr;
	}
	template<typename T>
	inline bool queue<T>::const_iterator::operator == (const queue<T>::iterator &it) const {
		return myPtr == it.myPtr;
	}
	template<typename T>
	inline bool queue<T>::const_iterator::operator == (const queue<T>::const_iterator &it) const {
		return myPtr == it.myPtr;
	}

	template<typename T>
	inline bool queue<T>::iterator::operator != (const queue<T>::iterator &it) const {
		return !operator == (it);
	}
	template<typename T>
	inline bool queue<T>::iterator::operator != (const queue<T>::const_iterator &it) const {
		return !operator == (it);
	}
	template<typename T>
	inline bool queue<T>::const_iterator::operator != (const queue<T>::iterator &it) const {
		return !operator == (it);
	}
	template<typename T>
	inline bool queue<T>::const_iterator::operator != (const queue<T>::const_iterator &it) const {
		return !operator == (it);
	}

	template<typename T>
	inline const queue<T>::iterator &queue<T>::iterator::operator ++ (int) {
		myPtr++;
		return *this;
	}
	template<typename T>
	inline const queue<T>::iterator &queue<T>::iterator::operator += (size_t offset) {
		myPtr += offset;
		return *this;
	}
	template<typename T>
	inline const queue<T>::iterator &queue<T>::iterator::operator -- (int) {
		myPtr--;
		return *this;
	}
	template<typename T>
	inline const queue<T>::iterator &queue<T>::iterator::operator + (size_t offset) const {
		iterator it = *this;
		it.myPtr += offset;
		return it;
	}
	template<typename T>
	inline size_t queue<T>::iterator::operator - (const queue<T>::iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T>
	inline size_t queue<T>::iterator::operator - (const queue<T>::const_iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T>
	inline const queue<T>::const_iterator &queue<T>::const_iterator::operator ++ (int) {
		myPtr++;
		return *this;
	}
	template<typename T>
	inline const queue<T>::const_iterator &queue<T>::const_iterator::operator += (size_t offset) {
		myPtr += offset;
		return *this;
	}
	template<typename T>
	inline const queue<T>::const_iterator &queue<T>::const_iterator::operator -- (int) {
		myPtr--;
		return *this;
	}
	template<typename T>
	inline const queue<T>::const_iterator &queue<T>::const_iterator::operator + (size_t offset) const {
		const_iterator it = *this;
		it.myPtr += offset;
		return it;
	}
	template<typename T>
	inline size_t queue<T>::const_iterator::operator - (const queue<T>::iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T>
	inline size_t queue<T>::const_iterator::operator - (const queue<T>::const_iterator &it) const {
		return myPtr - it.myPtr;
	}

	template<typename T>
	inline T &queue<T>::iterator::operator * () const {
		return *myPtr;
	}
	template<typename T>
	inline T *queue<T>::iterator::operator -> () const {
		return myPtr;
	}
	template<typename T>
	inline const T &queue<T>::const_iterator::operator * () const {
		return *myPtr;
	}
	template<typename T>
	inline const T *queue<T>::const_iterator::operator -> () const {
		return myPtr;
	}
	*/

	template<typename T>
	bool queue<T>::empty() const {
		// TODO: implement
		return true;
	}
	template<typename T>
	size_t queue<T>::size() const {
		// TODO: implement
		return 0;
	}
	template<typename T>
	T& queue<T>::front() const {
		// TODO: implement
	}
	template<typename T>
	void queue<T>::push(const T &element) {
		// TODO: implement
	}
	template<typename T>
	void queue<T>::pop() {
		// TODO: implement
	}
	/*
	template<typename T>
	T& queue<T>::operator[] (size_t index) const {
		// TODO: implement
		return front();
	}
	template<typename T>
	T& queue<T>::back() const {
		// TODO: implement
		return front();
	}

	template<typename T>
	queue<T>::iterator queue<T>::begin() const {
		// TODO: implement
		return iterator();
	}
	template<typename T>
	queue<T>::iterator queue<T>::end() const {
		// TODO: implement
		return iterator();
	}

	template<typename T>
	inline void queue<T>::clear() {
		// TODO: implement
	}
	template<typename T>
	inline void queue<T>::reserve(size_t size) {
		// TODO: implement
	}
	template<typename T>
	inline void queue<T>::push_back(const T &element) {
		// TODO: implement
	}
	template<typename T>
	inline void queue<T>::insert(const queue<T>::iterator &position, const queue<T>::const_iterator &from, const queue<T>::const_iterator &to) {
		// TODO: implement
	}
	template<typename T>
	inline void queue<T>::swap(queue<T>::queue &v) {
		// TODO: implement
	}

	template<typename T>
	inline queue<T>::const_iterator upper_bound(const queue<T>::const_iterator &start, const queue<T>::const_iterator &end, const T &value) {
		// TODO: implement
		return start;
	}
	template<typename T>
	inline queue<T>::const_iterator lower_bound(const queue<T>::const_iterator &start, const queue<T>::const_iterator &end, const T &value) {
		// TODO: implement
		return start;
	}
	*/
};

#endif /* __QUEUE1_H__ */
