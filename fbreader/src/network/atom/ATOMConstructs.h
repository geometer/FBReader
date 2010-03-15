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

#ifndef __ATOMCONSTRUCTS_H__
#define __ATOMCONSTRUCTS_H__

#include <map>
#include <string>


class ATOMCommonAttributes {

public:
	static const std::string XML_BASE;
	static const std::string XML_LANG;

public:
	ATOMCommonAttributes();
	virtual ~ATOMCommonAttributes();

public:
	std::map<std::string, std::string> &attributes() { return myAttributes; }
	const std::map<std::string, std::string> &attributes() const { return myAttributes; }

	std::string &base() { return myAttributes[XML_BASE]; }
	std::string &lang() { return myAttributes[XML_LANG]; }

	virtual void readAttributes(const std::map<std::string, std::string> &attributes);

	void setUserData(const std::string &key, const std::string &value);
	const std::string userData(const std::string &key) const;

protected:
	void readAttribute(const std::string &name, const std::map<std::string, std::string> &attributes);

private:
	std::map<std::string, std::string> myAttributes;
	std::map<std::string,std::string> myUserData;
};


class ATOMPersonConstruct : public ATOMCommonAttributes {

public:
	ATOMPersonConstruct();
	ATOMPersonConstruct(const std::string &name);

	const std::string &name() const { return myName; }
	const std::string &uri() const { return myUri; }
	const std::string &email() const { return myEmail; }

	void setName(const std::string &name) { myName = name; }
	void setUri(const std::string &uri) { myUri = uri; }
	void setEmail(const std::string &email) { myEmail = email; }

private:
	 std::string myName;
	 std::string myUri;
	 std::string myEmail;
};


class ATOMDateConstruct : public ATOMCommonAttributes {

public:
	static bool parse(const std::string &str, ATOMDateConstruct &dateTime);

public:
	ATOMDateConstruct(int year);
	ATOMDateConstruct(int year, int month, int day);
	ATOMDateConstruct(int year, int month, int day, int hour, int minutes, int seconds);
	ATOMDateConstruct(int year, int month, int day, int hour, int minutes, int seconds, float sfract);
	ATOMDateConstruct(int year, int month, int day, int hour, int minutes, int seconds, float sfract, int tzhour, int tzminutes);

	std::string getDateTime(bool brief = false) const;

	int year() const { return myYear; }
	int month() const { return myMonth; }
	int day() const { return myDay; }
	int hour() const { return myHour; }
	int minutes() const { return myMinutes; }
	int seconds() const { return mySeconds; }
	float secondFraction() const { return mySecondFraction; }
	int timeZoneHour() const { return myTZHour; }
	int timeZoneMinutes() const { return myTZMinutes; }

	void setYear(int year) { myYear = year; }
	void setMonth(int month) { myMonth = month; }
	void setDay(int day) { myDay = day; }
	void setHour(int hour) { myHour = hour; }
	void setMinutes(int minutes) { myMinutes = minutes; }
	void setSeconds(int seconds) { mySeconds = seconds; }
	void setSecondFraction(float sfract) { mySecondFraction = sfract; }
	void setTZHour(int tzhour) { myTZHour = tzhour; }
	void setTZMinutes(int tzminutes) { myTZMinutes = tzminutes; }

private:
	static void makeStringLength(std::string &str, int len);

private:
	int myYear;
	int myMonth;
	int myDay;
	int myHour;
	int myMinutes;
	int mySeconds;
	float mySecondFraction;
	int myTZHour;
	int myTZMinutes;
};


#endif /* __ATOMCONSTRUCTS_H__ */
