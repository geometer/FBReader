#include <iostream>
#include <string>

#include <zlanguage.h>

int main() {
	std::string language = (const char*)ZLanguage::getSystemLanguageCode().utf8();
	language.erase(2);
	if (language.length() != 2) {
		language = "en";
	}
	std::cout << language;
	return 0;
}
