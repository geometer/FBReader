#ifndef __LINECOORDS_H__
#define __LINECOORDS_H__

#include <shared_ptr.h>

class PointPtr;
class LinePtr;

class LineCoords {
	private:
		double myXStart, myYStart, myXEnd, myYEnd;

	public:
		LineCoords(LinePtr line, const int width, const int height);
		LineCoords(PointPtr start, PointPtr end);

		const int XStart() const;
		const int YStart() const;
		const int XEnd() const;
		const int YEnd() const;
};

typedef shared_ptr<LineCoords> LineCoordsPtr;

#endif /* __LINECOORDS_H__ */
