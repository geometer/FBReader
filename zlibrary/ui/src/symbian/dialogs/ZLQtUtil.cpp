#include <ZLDialogManager.h>

#include "ZLQtUtil.h"

QString qtString(const std::string &text) {
	return QString::fromUtf8(text.c_str());

}

std::string stdString(const QString &text) {
	return  (const char*)(text.toUtf8());
}

QString qtButtonName(const ZLResourceKey &key) {
	if (key.Name.empty()) {
		return QString::null;
	}
	return ::qtString(ZLDialogManager::buttonName(key));
}
