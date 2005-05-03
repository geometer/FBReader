#ifndef __ALGO1_H__
#define __ALGO1_H__

namespace std {
	template <typename T>
	const T &min(const T &x, const T &y) STL_SECTION;

	template <typename T>
	inline const T &min(const T &x, const T &y) {
		return (x < y) ? x : y;
	}
};

#endif /* __ALGO1_H__ */
