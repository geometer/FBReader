#ifndef __ZLQTKEYUTIL_H__
#define __ZLQTKEYUTIL_H__

#include "../../../../core/src/util/ZLKeyUtil.h"

class QKeyEvent;

class ZLQtKeyUtil : public ZLKeyUtil {

public:
	static std::string keyName(QKeyEvent *event);
};

#endif /* __ZLQTKEYUTIL_H__ */
