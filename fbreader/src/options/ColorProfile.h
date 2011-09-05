#ifndef __COLORPROFILE_H__
#define __COLORPROFILE_H__

#include <string>
#include <map>

#include <shared_ptr.h>

#include <ZLColor.h>
#include <ZLOptions.h>
#include <ZLTextStyle.h>

class ColorProfile {
public:
	static const std::string DAY;
	static const std::string NIGHT;
public:
	ColorProfile(std::string name);
public:
	shared_ptr<ZLColorOption> BackgroundColorOption;
	shared_ptr<ZLColorOption> RegularTextColorOption;
	ZLColorOption &colorOption(const std::string &style);
private:
	shared_ptr<ZLColorOption> createOption(std::string profileName, std::string optionName, int r, int g, int b);
private:
	std::map<std::string,shared_ptr<ZLColorOption> > myColorOptions;

};

#endif // __COLORPROFILE_H__
