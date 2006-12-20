#ifndef __ADDLINEMODE_H__
#define __ADDLINEMODE_H__

#include "AddObjectMode.h"
#include "../model/Point.h"
#include "../model/Line.h"

class AddLineMode : public AddObjectMode {
	private:
		const LineKind myKind;

	protected:
    ObjectPtr createObject(PointPtr point0, PointPtr point1) const;

	public:
		AddLineMode(DiagramView &view, LineKind kind);
};

#endif /* __ADDLINEMODE_H__ */
