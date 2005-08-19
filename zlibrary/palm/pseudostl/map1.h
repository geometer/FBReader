#ifndef __MAP1_H__
#define __MAP1_H__

#include <sys_types.h>
#include <vector>

namespace std {

	template<typename T1, typename T2>
	struct pair {
		pair() STL2_SECTION;
		pair(const T1 &f, const T2 &s) STL2_SECTION;
		pair &operator = (const pair &p) STL2_SECTION;
		T1 first;
		T2 second;
	};

	template<typename T1, typename T2>
	class map {

	public:
		struct const_iterator;

		struct iterator {
			iterator() STL2_SECTION;

			bool operator == (const iterator &it) const STL2_SECTION;
			bool operator == (const const_iterator &it) const STL2_SECTION;
			bool operator != (const iterator &it) const STL2_SECTION;
			bool operator != (const const_iterator &it) const STL2_SECTION;

			const iterator &operator ++ (int) STL2_SECTION;
			const iterator &operator += (size_t offset) STL2_SECTION;
			const iterator &operator -- (int) STL2_SECTION;
			const iterator operator + (size_t offset) const STL2_SECTION;
			const iterator operator - (size_t offset) const STL2_SECTION;
			size_t operator - (const iterator &it) const STL2_SECTION;
			size_t operator - (const const_iterator &it) const STL2_SECTION;

			pair<T1,T2> &operator * () const STL2_SECTION;
			pair<T1,T2> *operator -> () const STL2_SECTION;

		private:
			vector<pair<T1,T2> >::iterator myIter;
			iterator(const vector<pair<T1,T2> >::iterator &iter) STL2_SECTION;

		friend struct const_iterator;
		friend class map;
		};

		struct const_iterator {
			const_iterator() STL2_SECTION;
			const_iterator(const iterator &it) STL2_SECTION;

			bool operator == (const iterator &it) const STL2_SECTION;
			bool operator == (const const_iterator &it) const STL2_SECTION;
			bool operator != (const iterator &it) const STL2_SECTION;
			bool operator != (const const_iterator &it) const STL2_SECTION;

			const const_iterator &operator ++ (int) STL2_SECTION;
			const const_iterator &operator += (size_t offset) STL2_SECTION;
			const const_iterator &operator -- (int) STL2_SECTION;
			const const_iterator operator + (size_t offset) const STL2_SECTION;
			const const_iterator operator - (size_t offset) const STL2_SECTION;
			size_t operator - (const iterator &it) const STL2_SECTION;
			size_t operator - (const const_iterator &it) const STL2_SECTION;

			const pair<T1,T2> &operator * () const STL2_SECTION;
			const pair<T1,T2> *operator -> () const STL2_SECTION;

		private:
			vector<pair<T1,T2> >::const_iterator myIter;
			const_iterator(const vector<pair<T1,T2> >::const_iterator &iter) STL2_SECTION;

		friend struct iterator;
		friend class map;
		};

	public:
		map() STL2_SECTION;
		~map() STL2_SECTION;

		bool empty() const STL2_SECTION;
		size_t size() const STL2_SECTION;
		T2 &operator[] (const T1 &key) STL2_SECTION;
		const T2 &operator[] (const T1 &key) const STL2_SECTION;
		iterator find(const T1 &key) const STL2_SECTION;

		iterator begin() const STL2_SECTION;
		iterator end() const STL2_SECTION;

		void insert(const pair<T1,T2> &p) STL2_SECTION;
		void clear() STL2_SECTION;
		void swap(map &m) STL_SECTION;

	private:
		mutable vector<pair<T1,T2> > myData;
	};

	template<typename T1, typename T2>
	inline pair<T1,T2>::pair() {}

	template<typename T1, typename T2>
	inline pair<T1,T2>::pair(const T1 &f, const T2 &s) : first(f), second(s) {}

	template<typename T1, typename T2>
	inline pair<T1,T2> &pair<T1,T2>::operator = (const pair<T1,T2> &p) {
		first = p.first;
		second = p.second;
		return *this;
	}

	template<typename T1, typename T2>
	inline map<T1,T2>::map() {
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::~map() {
	}

	template<typename T1, typename T2>
	inline map<T1,T2>::iterator::iterator() {
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::iterator::iterator(const vector<pair<T1,T2> >::iterator &iter) : myIter(iter) {
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::const_iterator::const_iterator() {
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::const_iterator::const_iterator(const map<T1,T2>::iterator &it) : myIter(it.myIter) {
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::const_iterator::const_iterator(const vector<pair<T1,T2> >::const_iterator &iter) : myIter(iter) {
	}

	template<typename T1, typename T2>
	inline bool map<T1,T2>::iterator::operator == (const map<T1,T2>::iterator &it) const {
		return myIter == it.myIter;
	}
	template<typename T1, typename T2>
	inline bool map<T1,T2>::iterator::operator == (const map<T1,T2>::const_iterator &it) const {
		return myIter == it.myIter;
	}
	template<typename T1, typename T2>
	inline bool map<T1,T2>::const_iterator::operator == (const map<T1,T2>::iterator &it) const {
		return myIter == it.myIter;
	}
	template<typename T1, typename T2>
	inline bool map<T1,T2>::const_iterator::operator == (const map<T1,T2>::const_iterator &it) const {
		return myIter == it.myIter;
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

	template<typename T1, typename T2>
	inline const map<T1,T2>::iterator &map<T1,T2>::iterator::operator ++ (int) {
		myIter++;
		return *this;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::iterator &map<T1,T2>::iterator::operator += (size_t offset) {
		myIter += offset;
		return *this;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::iterator &map<T1,T2>::iterator::operator -- (int) {
		myIter--;
		return *this;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::iterator map<T1,T2>::iterator::operator + (size_t offset) const {
		iterator it = *this;
		it.myIter += offset;
		return it;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::iterator map<T1,T2>::iterator::operator - (size_t offset) const {
		iterator it = *this;
		it.myIter -= offset;
		return it;
	}
	template<typename T1, typename T2>
	inline size_t map<T1,T2>::iterator::operator - (const map<T1,T2>::iterator &it) const {
		return myIter - it.myIter;
	}
	template<typename T1, typename T2>
	inline size_t map<T1,T2>::iterator::operator - (const map<T1,T2>::const_iterator &it) const {
		return myIter - it.myIter;
	}
	template<typename T1, typename T2>
	inline const map<T1,T2>::const_iterator &map<T1,T2>::const_iterator::operator ++ (int) {
		myIter++;
		return *this;
	}
	/*
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
	inline const map<T1,T2>::const_iterator map<T1,T2>::const_iterator::operator + (size_t offset) const {
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
		return *myIter;
	}
	template<typename T1, typename T2>
	inline pair<T1,T2> *map<T1,T2>::iterator::operator -> () const {
		// TODO: !!!
		return &*myIter;
	}
	template<typename T1, typename T2>
	inline const pair<T1,T2> &map<T1,T2>::const_iterator::operator * () const {
		return *myIter;
	}
	template<typename T1, typename T2>
	inline const pair<T1,T2> *map<T1,T2>::const_iterator::operator -> () const {
		// TODO: !!!
		return &*myIter;
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
	*/
	template<typename T1, typename T2>
	size_t map<T1,T2>::size() const {
		return end() - begin();
	}
	/*
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
		return iterator(myData.begin());
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::iterator map<T1,T2>::end() const {
		return iterator(myData.end());
	}

	template<typename T1, typename T2>
	inline T2 &map<T1,T2>::operator[] (const T1 &key) {
		iterator it = find(key);
		if (it == end()) {
			insert(pair<T1,T2>(key, T2()));
			it = end() - 1;
		}
		return it->second;
	}
	template<typename T1, typename T2>
	inline const T2 &map<T1,T2>::operator[] (const T1 &key) const {
		iterator it = find(key);
		if (it == end()) {
			insert(pair<T1,T2>(key, T2()));
			it = end() - 1;
		}
		return it->second;
	}
	template<typename T1, typename T2>
	inline map<T1,T2>::iterator map<T1,T2>::find(const T1 &key) const {
		iterator it = begin();
		for (; it != end(); it++) {
			if (it->first == key) {
				return it;
			}
		}
		return it;
	}

	template<typename T1, typename T2>
	inline void map<T1,T2>::insert(const pair<T1,T2> &p) {
		myData.push_back(p);
	}
	template<typename T1, typename T2>
	inline void map<T1,T2>::clear() {
		myData.clear();
	}
	template<typename T1, typename T2>
	void map<T1,T2>::swap(map<T1,T2> &m) {
		myData.swap(m.myData);
	}
	/*
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
