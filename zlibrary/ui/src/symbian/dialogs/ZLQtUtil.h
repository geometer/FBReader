#ifndef __ZLQTUTIL_H__
#define __ZLQTUTIL_H__


#include <string>

#include <QtCore/QString>

#include <ZLResource.h>

QString qtString(const std::string &text);
std::string stdString(const QString &text);
QString qtButtonName(const ZLResourceKey &key);

#endif /* __ZLQTUTIL_H__ */
