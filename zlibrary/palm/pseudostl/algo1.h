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

#ifndef __ALGO1_H__
#define __ALGO1_H__

namespace std {
	template <typename T>
	const T &min(const T &x, const T &y) STL_SECTION;
	template <typename T>
	const T &max(const T &x, const T &y) STL_SECTION;

	template <typename T>
	inline const T &min(const T &x, const T &y) {
		return (x < y) ? x : y;
	}

	template <typename T>
	inline const T &max(const T &x, const T &y) {
		return (x > y) ? x : y;
	}

	template <typename T>
	struct less {
		bool operator() (const T &t1, const T &t2) const {
			return t1 < t2;
		}
	};

	template<typename Iterator, typename T, typename Comparator>
	Iterator upper_bound(Iterator start, Iterator end, const T &value, const Comparator &comparator) STL_SECTION;
	template<typename Iterator, typename T, typename Comparator>
	Iterator lower_bound(Iterator start, Iterator end, const T &value, const Comparator &comparator) STL_SECTION;
	template<typename Iterator, typename T>
	Iterator upper_bound(Iterator start, Iterator end, const T &value) STL_SECTION;
	template<typename Iterator, typename T>
	Iterator lower_bound(Iterator start, Iterator end, const T &value) STL_SECTION;
	template<typename Iterator, typename Comparator>
	void sort(const Iterator &startIterator, const Iterator &endIterator, const Comparator &comparator) STL_SECTION;
	template<typename Iterator>
	void sort(const Iterator &startIterator, const Iterator &endIterator) STL_SECTION;

	template<typename Iterator, typename T, typename Comparator>
	inline Iterator upper_bound(const Iterator &start, const Iterator &end, const T &value, const Comparator &comparator) {
		// TODO: implement
		return end;
	}
	template<typename Iterator, typename T, typename Comparator>
	inline Iterator lower_bound(Iterator start, Iterator end, const T &value, const Comparator &comparator) {
		if (start == end) {
			return end;
		}
		if (!comparator(*start, value)) {
			return start;
		}
		while (end - start > 1) {
			Iterator mid = start + (end - start) / 2;
			if (comparator(*mid, value)) {
				start = mid;
			} else {
				end = mid;
			}
		}
		return comparator(*start, value) ? end : start;
	}

	template<typename Iterator, typename T>
	inline Iterator upper_bound(Iterator start, Iterator end, const T &value) {
		return upper_bound(start, end, value, less<T>());
	}
	template<typename Iterator, typename T>
	inline Iterator lower_bound(Iterator start, Iterator end, const T &value) {
		return lower_bound(start, end, value, less<T>());
	}

	template<typename Iterator, typename Comparator>
	inline void sort(const Iterator &startIterator, const Iterator &endIterator, const Comparator &comparator) {
		// TODO: implement
	}
	template<typename Iterator>
	inline void sort(const Iterator &startIterator, const Iterator &endIterator) {
		// TODO: implement
	}
};

#endif /* __ALGO1_H__ */
