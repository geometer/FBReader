#ifndef __SET1_H__
#define __SET1_H__

#include <sys_types.h>

namespace std {

	template<typename T>
	class set {

	public:
		struct const_iterator;

		struct iterator {
			iterator() STL_SECTION;
			bool operator == (const iterator &it) const STL_SECTION;
			bool operator == (const const_iterator &it) const STL_SECTION;
			bool operator != (const iterator &it) const STL_SECTION;
			bool operator != (const const_iterator &it) const STL_SECTION;

			const iterator &operator ++ (int) STL_SECTION;
			/*
			const iterator &operator += (size_t offset) STL_SECTION;
			const iterator &operator -- (int) STL_SECTION;
			const iterator &operator + (size_t offset) const STL_SECTION;
			size_t operator - (const iterator &it) const STL_SECTION;
			size_t operator - (const const_iterator &it) const STL_SECTION;
			*/

			T &operator * () const STL_SECTION;
			T *operator -> () const STL_SECTION;

		private:
			//T *myPtr;

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
		/*
			const const_iterator &operator += (size_t offset) STL_SECTION;
			const const_iterator &operator -- (int) STL_SECTION;
			const const_iterator &operator + (size_t offset) const STL_SECTION;
			size_t operator - (const iterator &it) const STL_SECTION;
			size_t operator - (const const_iterator &it) const STL_SECTION;
		*/

			const T &operator * () const STL_SECTION;
			const T *operator -> () const STL_SECTION;

		private:
			//T *myPtr;

		friend struct iterator;
		};

	public:
		set() STL_SECTION;
		~set() STL_SECTION;

		bool empty() const STL_SECTION;
		size_t size() const STL_SECTION;

		iterator begin() const STL_SECTION;
		iterator end() const STL_SECTION;

		iterator find(const T &element) STL_SECTION;
		void insert(const T &element) STL_SECTION;
		void insert(const const_iterator &from, const const_iterator &to) STL_SECTION;
		void erase(const T &element) STL_SECTION;
		/*
		void clear() STL_SECTION;
		void reserve(size_t size) STL_SECTION;
		void push_back(const T &element) STL_SECTION;
		void insert(const iterator &position, const const_iterator &from, const const_iterator &to) STL_SECTION;
		void swap(set &v) STL_SECTION;
		*/

	private:
		/*
		size_t myDataSize;
		size_t myLength;
		T *myData;
		*/
	};

	template<typename T>
	inline set<T>::set() {
		//TODO: implement
		//myDataSize = 0;
		//myLength = 0;
		//myData = 0;
	}
	template<typename T>
	inline set<T>::~set() {
		//TODO: implement
		//if (myData != 0) {
		//	delete[] myData;
		//}
	}

	template<typename T>
	inline set<T>::iterator::iterator() {
	}
	template<typename T>
	inline set<T>::const_iterator::const_iterator() {
	}
	template<typename T>
	inline set<T>::const_iterator::const_iterator(const set<T>::iterator &it) {
		//TODO: implement
		//myPtr = it.myPtr;
	}

	template<typename T>
	inline bool set<T>::iterator::operator == (const set<T>::iterator &it) const {
		//TODO: implement
		return true;
	}
	template<typename T>
	inline bool set<T>::iterator::operator == (const set<T>::const_iterator &it) const {
		//TODO: implement
		return true;
	}
	template<typename T>
	inline bool set<T>::const_iterator::operator == (const set<T>::iterator &it) const {
		//TODO: implement
		return true;
	}
	template<typename T>
	inline bool set<T>::const_iterator::operator == (const set<T>::const_iterator &it) const {
		//TODO: implement
		return true;
	}

	template<typename T>
	inline bool set<T>::iterator::operator != (const set<T>::iterator &it) const {
		return !operator == (it);
	}
	template<typename T>
	inline bool set<T>::iterator::operator != (const set<T>::const_iterator &it) const {
		return !operator == (it);
	}
	template<typename T>
	inline bool set<T>::const_iterator::operator != (const set<T>::iterator &it) const {
		return !operator == (it);
	}
	template<typename T>
	inline bool set<T>::const_iterator::operator != (const set<T>::const_iterator &it) const {
		return !operator == (it);
	}

	template<typename T>
	inline const set<T>::iterator &set<T>::iterator::operator ++ (int) {
		// TODO: implement
		return *this;
	}
	template<typename T>
	inline const set<T>::const_iterator &set<T>::const_iterator::operator ++ (int) {
		// TODO: implement
		return *this;
	}
	/*
	template<typename T>
	inline const set<T>::iterator &set<T>::iterator::operator += (size_t offset) {
		myPtr += offset;
		return *this;
	}
	template<typename T>
	inline const set<T>::iterator &set<T>::iterator::operator -- (int) {
		myPtr--;
		return *this;
	}
	template<typename T>
	inline const set<T>::iterator &set<T>::iterator::operator + (size_t offset) const {
		iterator it = *this;
		it.myPtr += offset;
		return it;
	}
	template<typename T>
	inline size_t set<T>::iterator::operator - (const set<T>::iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T>
	inline size_t set<T>::iterator::operator - (const set<T>::const_iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T>
	inline const set<T>::const_iterator &set<T>::const_iterator::operator ++ (int) {
		myPtr++;
		return *this;
	}
	template<typename T>
	inline const set<T>::const_iterator &set<T>::const_iterator::operator += (size_t offset) {
		myPtr += offset;
		return *this;
	}
	template<typename T>
	inline const set<T>::const_iterator &set<T>::const_iterator::operator -- (int) {
		myPtr--;
		return *this;
	}
	template<typename T>
	inline const set<T>::const_iterator &set<T>::const_iterator::operator + (size_t offset) const {
		const_iterator it = *this;
		it.myPtr += offset;
		return it;
	}
	template<typename T>
	inline size_t set<T>::const_iterator::operator - (const set<T>::iterator &it) const {
		return myPtr - it.myPtr;
	}
	template<typename T>
	inline size_t set<T>::const_iterator::operator - (const set<T>::const_iterator &it) const {
		return myPtr - it.myPtr;
	}
*/

	template<typename T>
	inline T &set<T>::iterator::operator * () const {
		//TODO: implement
		return *operator->();
	}
	template<typename T>
	inline T *set<T>::iterator::operator -> () const {
		//TODO: implement
		return 0;
	}
	template<typename T>
	inline const T &set<T>::const_iterator::operator * () const {
		//TODO: implement
		return *operator->();
	}
	template<typename T>
	inline const T *set<T>::const_iterator::operator -> () const {
		//TODO: implement
		return 0;
	}

	template<typename T>
	inline bool set<T>::empty() const {
		// TODO: implement
		return true;
	}
	template<typename T>
	inline size_t set<T>::size() const {
		// TODO: implement
		return 0;
	}

/*
	template<typename T>
	T& set<T>::operator[] (size_t index) const {
		// TODO: implement
		return front();
	}
	template<typename T>
	T& set<T>::front() const {
		// TODO: implement
		return *begin();
	}
	template<typename T>
	T& set<T>::back() const {
		// TODO: implement
		return front();
	}
	*/

	template<typename T>
	set<T>::iterator set<T>::begin() const {
		// TODO: implement
		return iterator();
	}
	template<typename T>
	set<T>::iterator set<T>::end() const {
		// TODO: implement
		return iterator();
	}

	/*
	template<typename T>
	inline void set<T>::clear() {
		// TODO: implement
	}
	template<typename T>
	inline void set<T>::reserve(size_t size) {
		// TODO: implement
	}
	template<typename T>
	inline void set<T>::push_back(const T &element) {
		// TODO: implement
	}
	*/

	template<typename T>
	inline set<T>::iterator set<T>::find(const T &element) {
		// TODO: implement
		return iterator();
	}
	template<typename T>
	inline void set<T>::insert(const T &element) {
		// TODO: implement
	}
	template<typename T>
	inline void set<T>::insert(const set<T>::const_iterator &from, const set<T>::const_iterator &to) {
		// TODO: implement
	}
	template<typename T>
	inline void set<T>::erase(const T &element) {
		// TODO: implement
	}

	/*
	template<typename T>
	inline void set<T>::swap(set<T>::set &v) {
		// TODO: implement
	}

	template<typename T>
	inline set<T>::const_iterator upper_bound(const set<T>::const_iterator &start, const set<T>::const_iterator &end, const T &value) {
		// TODO: implement
		return start;
	}
	template<typename T>
	inline set<T>::const_iterator lower_bound(const set<T>::const_iterator &start, const set<T>::const_iterator &end, const T &value) {
		// TODO: implement
		return start;
	}
	*/
};

#endif /* __SET1_H__ */
