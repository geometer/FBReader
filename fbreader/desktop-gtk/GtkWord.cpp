#include "GtkWord.h"

GtkWord::GtkWord(const char *utf8String, int len, int startOffset) : Word(startOffset) {
	myUtf8Contents.append(utf8String, len);
	ZLUnicodeUtil::utf8ToUcs2(myUcs2Contents, myUtf8Contents, -1);
}
