#ifndef __MAP1_H__
#define __MAP1_H__

#include <sys_types.h>

namespace std {

	template<typename T1, typename T2>
	struct pair {
		pair() STL_SECTION;
		pair(const T1 &f, const T2 &s) STL_SECTION;
		T1 first;
		T2 second;
	};

	template<typename T1, typename T2>
	class map {

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

			pair<T1,T2> &operator * () const STL_SECTION;
			pair<T1,T2> *operator -> () const STL_SECTION;
		/*
		private:
			T *myPtr;
		*/

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

			const pair<T1,T2> &operator * () const STL_SECTION;
			const pair<T1,T2> *operator -> () const STL_SECTION;
	/*
		private:
			T *myPtr;
	*/

		friend struct iterator;
		};

	public:
		map() STL_SECTION;
		~map() STL_SECTION;

		bool empty() const STL_SECTION;
		size_t size() const STL_SECTION;
		T2 &operator[] (const T1 &key) const STL_SECTION;
		iterator find(const T1 &key) const STL_SECTION;
	/*
		T& front() const STL_SECTION;
		T& back() const STL_SECTION;
	*/

		iterator begin() const STL_SECTION;
		iterator end() const STL_SECTION;

		void insert(const pair<T1,T2> &p) STL_SECTION;
	/*
		void clear() STL_SECTION;
		void reserve(size_t size) STL_SECTION;
		void push_back(const T &element) STL_SECTION;
		void insert(const iterator &position, const const_iterator &from, const const_iterator &to) STL_SECTION;
		void swap(map &v) STL_SECTION;
	*/

	private:
	/*
		size_t myDataSize;
		size_t myLength;
		T *myData;
	*/
	};

	template<typename T1, typename T2>
	inline pair<T1,T2>::pair() {}

	template<typename T1, typename T2>
	inline pair<T1,T2>::pair(const T1 &f, const T2 &s) : first(f), second(s) {}

	template<typename T1, typename T2>
	inline map<T1,T2>::map() {
		// TODO: implement
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::~map() {
		// TODO: implement
	}

	template<typename T1, typename T2>
	inline map<T1,T2>::iterator::iterator() {
		// TODO: implement
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::const_iterator::const_iterator() {
		// TODO: implement
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::const_iterator::const_iterator(const map<T1,T2>::iterator &it) {
		// TODO: implement
	}

	template<typename T1, typename T2>
	inline bool map<T1,T2>::iterator::operator == (const map<T1,T2>::iterator &it) const {
		// TODO: implement
		return true;
	}
	template<typename T1, typename T2>
	inline bool map<T1,T2>::iterator::operator == (const map<T1,T2>::const_iterator &it) const {
		// TODO: implement
		return true;
	}
	template<typename T1, typename T2>
	inline bool map<T1,T2>::const_iterator::operator == (const map<T1,T2>::iterator &it) const {
		// TODO: implement
		return true;
	}
	template<typename T1, typename T2>
	inline bool map<T1,T2>::const_iterator::operator == (const map<T1,T2>::const_iterator &it) const {
		// TODO: implement
		return true;
	}

	template<typename T1, typename T2>
	inline bool map<T1,T2>::iterator::operator != (const map<T1,T2>::iterator &it) const {
		return !operator == (it);
	}
	template<typename T1, typename T2>
	inline bool map<T1,T2>::iterator::operator != (const map<T1,T2>::const_iterator &it) const {
		return !operator == (it);
	}
	template<typename T1, typename T2>
	inline bool map<T1,T2>::const_iterator::operator != (const map<T1,T2>::iterator &it) const {
		return !operator == (it);
	}
	template<typename T1, typename T2>
	inline bool map<T1,T2>::const_iterator::operator != (const map<T1,T2>::const_iterator &it) const {
		return !operator == (it);
	}

	/*
	template<typename T1, typename T2>
	inline const map<T1,T2>::iterator &map<T1,T2>::iterator::operator ++ (int) {
		myPtr++;
		return *this;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::iterator &map<T1,T2>::iterator::operator += (size_t offset) {
		myPtr += offset;
		return *this;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::iterator &map<T1,T2>::iterator::operator -- (int) {
		myPtr--;
		return *this;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::iterator &map<T1,T2>::iterator::operator + (size_t offset) const {
		iterator it = *this;
		it.myPtr += offset;
		return it;
	}
	template<typename T1, typename T2>
	inline size_t map<T1,T2>::iterator::operator - (const map<T1,T2>::iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T1, typename T2>
	inline size_t map<T1,T2>::iterator::operator - (const map<T1,T2>::const_iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::const_iterator &map<T1,T2>::const_iterator::operator ++ (int) {
		myPtr++;
		return *this;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::const_iterator &map<T1,T2>::const_iterator::operator += (size_t offset) {
		myPtr += offset;
		return *this;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::const_iterator &map<T1,T2>::const_iterator::operator -- (int) {
		myPtr--;
		return *this;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::const_iterator &map<T1,T2>::const_iterator::operator + (size_t offset) const {
		const_iterator it = *this;
		it.myPtr += offset;
		return it;
	}
	template<typename T1, typename T2>
	inline size_t map<T1,T2>::const_iterator::operator - (const map<T1,T2>::iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T1, typename T2>
	inline size_t map<T1,T2>::const_iterator::operator - (const map<T1,T2>::const_iterator &it) const {
		return myPtr - it.myPtr;
	}
	*/

	template<typename T1, typename T2>
	inline pair<T1,T2> &map<T1,T2>::iterator::operator * () const {
		// TODO: implement
		return *operator -> ();
	}
	template<typename T1, typename T2>
	inline pair<T1,T2> *map<T1,T2>::iterator::operator -> () const {
		// TODO: implement
		return 0;
	}
	template<typename T1, typename T2>
	inline const pair<T1,T2> &map<T1,T2>::const_iterator::operator * () const {
		// TODO: implement
		return *operator -> ();
	}
	template<typename T1, typename T2>
	inline const pair<T1,T2> *map<T1,T2>::const_iterator::operator -> () const {
		// TODO: implement
		return 0;
	}

	/*
	template<typename T1, typename T2>
	inline const T &map<T1,T2>::const_iterator::operator * () const {
		return *myPtr;
	}
	template<typename T1, typename T2>
	inline const T *map<T1,T2>::const_iterator::operator -> () const {
		return myPtr;
	}

	template<typename T1, typename T2>
	bool map<T1,T2>::empty() const {
		// TODO: implement
		return true;
	}
	template<typename T1, typename T2>
	size_t map<T1,T2>::size() const {
		// TODO: implement
		return 0;
	}
	template<typename T1, typename T2>
	T& map<T1,T2>::operator[] (size_t index) const {
		// TODO: implement
		return front();
	}
	template<typename T1, typename T2>
	T& map<T1,T2>::front() const {
		// TODO: implement
		return *begin();
	}
	template<typename T1, typename T2>
	T& map<T1,T2>::back() const {
		// TODO: implement
		return front();
	}
	*/

	template<typename T1, typename T2>
	inline map<T1,T2>::iterator map<T1,T2>::begin() const {
		// TODO: implement
		return iterator();
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::iterator map<T1,T2>::end() const {
		// TODO: implement
		return iterator();
	}

	template<typename T1, typename T2>
	inline T2 &map<T1,T2>::operator[] (const T1 &key) const {
		// TODO: implement
		return find(key)->second;
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::iterator map<T1,T2>::find(const T1 &key) const {
		// TODO: implement
		return iterator();
	}

	template<typename T1, typename T2>
	inline void map<T1,T2>::insert(const pair<T1,T2> &p) {
		// TODO: implement
	}
	/*
	template<typename T1, typename T2>
	inline void map<T1,T2>::clear() {
		// TODO: implement
	}
	template<typename T1, typename T2>
	inline void map<T1,T2>::reserve(size_t size) {
		// TODO: implement
	}
	template<typename T1, typename T2>
	inline void map<T1,T2>::push_back(const T &element) {
		// TODO: implement
	}
	template<typename T1, typename T2>
	inline void map<T1,T2>::insert(const map<T1,T2>::iterator &position, const map<T1,T2>::const_iterator &from, const map<T1,T2>::const_iterator &to) {
		// TODO: implement
	}
	template<typename T1, typename T2>
	inline void map<T1,T2>::swap(map<T1,T2>::map &v) {
		// TODO: implement
	}

	template<typename T1, typename T2>
	inline map<T1,T2>::const_iterator upper_bound(const map<T1,T2>::const_iterator &start, const map<T1,T2>::const_iterator &end, const T &value) {
		// TODO: implement
		return start;
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::const_iterator lower_bound(const map<T1,T2>::const_iterator &start, const map<T1,T2>::const_iterator &end, const T &value) {
		// TODO: implement
		return start;
	}
	*/
};

#endif /* __MAP1_H__ */
