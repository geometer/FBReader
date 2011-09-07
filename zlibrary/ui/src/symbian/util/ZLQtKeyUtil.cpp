#include <cctype>

#include <QtGui/QKeyEvent>

#include <ZLUnicodeUtil.h>

#include "ZLQtKeyUtil.h"

std::string ZLQtKeyUtil::keyName(QKeyEvent *keyEvent) {
	ZLUnicodeUtil::Ucs2String s;
	ZLUnicodeUtil::utf8ToUcs2(s, (const char*)keyEvent->text().toUtf8());
	int unicode = s.empty() ? 0 : s[0];
	return ZLKeyUtil::keyName(unicode, keyEvent->key(), keyEvent->modifiers());
}
