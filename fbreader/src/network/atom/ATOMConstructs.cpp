/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "ATOMConstructs.h"

#include <ZLStringUtil.h>

#include <math.h>



const std::string ATOMCommonAttributes::XML_BASE = "xml:base";
const std::string ATOMCommonAttributes::XML_LANG = "xml:lang";


ATOMCommonAttributes::ATOMCommonAttributes() {
}

ATOMCommonAttributes::~ATOMCommonAttributes() {
}

void ATOMCommonAttributes::readAttributes(const std::map<std::string, std::string> &attributes) {
	readAttribute(XML_BASE, attributes);
	readAttribute(XML_LANG, attributes);
}

void ATOMCommonAttributes::readAttribute(const std::string &name, const std::map<std::string, std::string> &attributes) {
	std::map<std::string, std::string>::const_iterator it = attributes.find(name);
	if (it != attributes.end()) {
		myAttributes[name] = it->second;
	}	
}

void ATOMCommonAttributes::setUserData(const std::string &key, const std::string &value) {
	myUserData[key] = value;
}

const std::string ATOMCommonAttributes::userData(const std::string &key) const {
	std::map<std::string,std::string>::const_iterator it = myUserData.find(key);
	return (it != myUserData.end()) ? it->second : std::string();
}

ATOMPersonConstruct::ATOMPersonConstruct() {
}

ATOMPersonConstruct::ATOMPersonConstruct(const std::string &name) : myName(name) {
}


ATOMDateConstruct::ATOMDateConstruct(int year) : 
	myYear(year), myMonth(0), myDay(0), 
	myHour(0), myMinutes(0), mySeconds(0), mySecondFraction(0), myTZHour(0), myTZMinutes(0) {
}

ATOMDateConstruct::ATOMDateConstruct(int year, int month, int day) : 
	myYear(year), myMonth(month), myDay(day), 
	myHour(0), myMinutes(0), mySeconds(0), mySecondFraction(0), myTZHour(0), myTZMinutes(0) {
}

ATOMDateConstruct::ATOMDateConstruct(int year, int month, int day, int hour, int minutes, int seconds) : 
	myYear(year), myMonth(month), myDay(day), 
	myHour(hour), myMinutes(minutes), mySeconds(seconds), mySecondFraction(0), myTZHour(0), myTZMinutes(0) {
}

ATOMDateConstruct::ATOMDateConstruct(int year, int month, int day, int hour, int minutes, int seconds, float sfract) : 
	myYear(year), myMonth(month), myDay(day),  
	myHour(hour), myMinutes(minutes), mySeconds(seconds), mySecondFraction(sfract), myTZHour(0), myTZMinutes(0) {
}

ATOMDateConstruct::ATOMDateConstruct(int year, int month, int day, int hour, int minutes, int seconds, float sfract, int tzhour, int tzminutes) : 
	myYear(year), myMonth(month), myDay(day),  
	myHour(hour), myMinutes(minutes), mySeconds(seconds), mySecondFraction(sfract), myTZHour(tzhour), myTZMinutes(tzminutes) {
}



void ATOMDateConstruct::makeStringLength(std::string &str, int len) {
	const int lendiff = str.length() - len;
	if (lendiff > 0) {
		str = str.substr(lendiff);
	} else {
		str = std::string(-lendiff, '0') + str;
	}
}


std::string ATOMDateConstruct::getDateTime(bool brief) const {
	std::string timezone = "Z";
	if (myTZMinutes != 0 || myTZHour != 0) {
		int tzminnum = myTZMinutes;
		int tzhournum = myTZHour;
		char sign;
		if (tzhournum == 0) {
			sign = (tzminnum >= 0) ? '+' : '-';
		} else {
			sign = (tzhournum > 0) ? '+' : '-';
			if (tzhournum > 0 && tzminnum < 0) {
				--tzhournum;
				tzminnum = 60 + tzminnum;
			} else if (tzhournum < 0 && tzminnum > 0) {
				++tzhournum;
				tzminnum = 60 - tzminnum;
			}
		}
		std::string tzmin, tzhour;
		ZLStringUtil::appendNumber(tzmin, tzminnum < 0 ? -tzminnum : tzminnum);
		ZLStringUtil::appendNumber(tzhour, tzhournum < 0 ? -tzhournum : tzhournum);
		makeStringLength(tzmin, 2);
		makeStringLength(tzhour, 2);
		timezone = sign + tzhour + ":" + tzmin;
	}

	std::string time;
	if (mySecondFraction >= 0.01) {
		std::string sfr;
		unsigned int sfrnum = (unsigned int) floor(100 * mySecondFraction + 0.5);
		ZLStringUtil::appendNumber(sfr, sfrnum);
		makeStringLength(sfr, 2);
		time = "." + sfr;
	}
	if (!brief || !time.empty() || mySeconds != 0) {
		std::string sec;
		ZLStringUtil::appendNumber(sec, mySeconds);
		makeStringLength(sec, 2);
		time = ":" + sec + time;
	}
	if (!brief || !time.empty() || myHour != 0 || myMinutes != 0 || timezone != "Z") {
		std::string hour, min;
		ZLStringUtil::appendNumber(hour, myHour);
		ZLStringUtil::appendNumber(min, myMinutes);
		makeStringLength(hour, 2);
		makeStringLength(min, 2);
		time = hour + ":" + min + time;
	}

	std::string date;
	if (!brief || !time.empty() || myDay != 0) {
		std::string day;
		ZLStringUtil::appendNumber(day, myDay);
		makeStringLength(day, 2);
		date = "-" + day;
	}
	if (!brief || !date.empty() || myMonth != 0) {
		std::string month;
		ZLStringUtil::appendNumber(month, myMonth);
		makeStringLength(month, 2);
		date = "-" + month + date;
	}

	std::string year;
	ZLStringUtil::appendNumber(year, myYear);
	makeStringLength(year, 4);
	date = year + date;

	if (!brief || !time.empty()) {
		date = date + "T" + time + timezone;
	}
	return date;
}


bool ATOMDateConstruct::parse(const std::string &str, ATOMDateConstruct &dateTime) {
	dateTime.setYear(0); 
	dateTime.setMonth(0); 
	dateTime.setDay(0); 
	dateTime.setHour(0); 
	dateTime.setMinutes(0); 
	dateTime.setSeconds(0); 
	dateTime.setSecondFraction(0);
	dateTime.setTZHour(0); 
	dateTime.setTZMinutes(0); 
	const int len = str.length();
	if (len != 4 && len != 7 && len != 10 && len != 17 && len != 20 && len < 22) {
		return false;
	}
	int num = 0, sign = 1;
	float fnum = 0.0, fmult = 0.1;
	int start, end, log;
	char ch;
	end = 4; start = 0; log = 0;
	while (start < len) {
		ch = str[start++];
		if (!isdigit(ch)) {
			return false;
		}
		num = 10 * num + ((int) (ch - '0'));
		fnum += fmult * ((int) (ch - '0'));
		fmult *= 0.1;
		if (start == end) {
			switch (log) {
			case 0: dateTime.setYear(num); break;
			case 1: dateTime.setMonth(num); break;
			case 2: dateTime.setDay(num); break;
			case 3: dateTime.setHour(num); break;
			case 4: dateTime.setMinutes(num); break;
			case 5: dateTime.setSeconds(num); break;
			case 6: dateTime.setSecondFraction(fnum); break;
			case 7: dateTime.setTZHour(sign * num); break;
			case 8: dateTime.setTZMinutes(sign * num); break;
			default: return false;
			}
			num = 0; fnum = 0.0; fmult = 0.1;
			if (start == len) return true;
			switch (log) {
			case 0:
			case 1:
				if (str[start++] != '-') return false;
				end = start + 2;
				break;
			case 2:
				if (str[start++] != 'T') return false;
				end = start + 2;
				break;
			case 3:
			case 7:
				if (str[start++] != ':') return false;
				end = start + 2;
				break;
			case 4:
				ch = str[start++];
				if (ch == ':') {
					end = start + 2;
				} else if (ch == '+' || ch == '-') {
					sign = (ch == '-') ? -1 : 1;
					log += 2;
					end = start + 2;
				} else if (ch == 'Z') {
					return true;
				} else return false;
				break;
			case 5:
				ch = str[start++];
				if (ch == '.') {
					end = start;
					while (isdigit(str[++end])) /* NOP */;
				} else if (ch == '+' || ch == '-') {
					sign = (ch == '-') ? -1 : 1;
					log += 1;
					end = start + 2;
				} else if (ch == 'Z') {
					return true;
				} else return false;
				break;
			case 6:
				ch = str[start++];
				if (ch == '+' || ch == '-') {
					sign = (ch == '-') ? -1 : 1;
					end = start + 2;
				} else if (ch == 'Z') {
					return true;
				} else return false;
				break;
			//case 8:
			default: return false;
			}
			++log;
		}
	}
	return false;
}

