/*
 * Copyright (C) 2009-2011 Geometer Plus <contact@geometerplus.com>
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


#include "Tests.h"

#include "network/atom/ATOMConstructs.h"


#define parseString(str, patt) _parseString(__TOSTRING(__LINE__), str, patt)

void _parseString(std::string lineNum, const std::string &str, std::string patt) {
	ATOMDateConstruct date(9999, 99, 99, 99, 99, 99, 0.99, 99, 99);
	if (!ATOMDateConstruct::parse(str, date)) {
		throw (lineNum + ": can't parse: " + str);
	}
	if (patt.empty()) {
		patt = str;
	}
	if (date.getDateTime() != patt) {
		throw (lineNum + ": failed to parse: " + str + "\n" + date.getDateTime() + " != " + patt);
	}
}


bool ATOMDateParseTest01() {
	std::string patt = "0000-00-00T00:00:00Z";
	parseString("0000-00-00T00:00:00.00+00:00", patt);
	parseString("0000-00-00T00:00:00.00-00:00", patt);
	parseString("0000-00-00T00:00:00.00Z", patt);
	parseString("0000-00-00T00:00:00+00:00", patt);
	parseString("0000-00-00T00:00:00-00:00", patt);
	parseString("0000-00-00T00:00:00Z", patt);
	parseString("0000-00-00T00:00+00:00", patt);
	parseString("0000-00-00T00:00-00:00", patt);
	parseString("0000-00-00T00:00Z", patt);
	parseString("0000-00-00", patt);
	parseString("0000-00", patt);
	parseString("0000", patt);
	return true;
}

bool ATOMDateParseTest02() {
	std::string patt = "0000-00-00T00:00:00+00:15";
	parseString("0000-00-00T00:00:00.00+00:15", patt);
	parseString("0000-00-00T00:00:00+00:15", patt);
	parseString("0000-00-00T00:00+00:15", patt);

	patt = "0000-00-00T00:00:00+03:15";
	parseString("0000-00-00T00:00:00.00+03:15", patt);
	parseString("0000-00-00T00:00:00+03:15", patt);
	parseString("0000-00-00T00:00+03:15", patt);

	patt = "0000-00-00T00:00:00+03:00";
	parseString("0000-00-00T00:00:00.00+03:00", patt);
	parseString("0000-00-00T00:00:00+03:00", patt);
	parseString("0000-00-00T00:00+03:00", patt);

	patt = "0000-00-00T00:00:00-00:15";
	parseString("0000-00-00T00:00:00.00-00:15", patt);
	parseString("0000-00-00T00:00:00-00:15", patt);
	parseString("0000-00-00T00:00-00:15", patt);

	patt = "0000-00-00T00:00:00-03:15";
	parseString("0000-00-00T00:00:00.00-03:15", patt);
	parseString("0000-00-00T00:00:00-03:15", patt);
	parseString("0000-00-00T00:00-03:15", patt);

	patt = "0000-00-00T00:00:00-03:00";
	parseString("0000-00-00T00:00:00.00-03:00", patt);
	parseString("0000-00-00T00:00:00-03:00", patt);
	parseString("0000-00-00T00:00-03:00", patt);

	return true;
}

bool ATOMDateParseTest03() {
	std::string patt = "0000-00-00T00:00:00.35Z";
	parseString("0000-00-00T00:00:00.35+00:00", patt);
	parseString("0000-00-00T00:00:00.35-00:00", patt);
	parseString("0000-00-00T00:00:00.35Z", patt);
	parseString("0000-00-00T00:00:00.351+00:00", patt);
	parseString("0000-00-00T00:00:00.351-00:00", patt);
	parseString("0000-00-00T00:00:00.351Z", patt);
	parseString("0000-00-00T00:00:00.348+00:00", patt);
	parseString("0000-00-00T00:00:00.348-00:00", patt);
	parseString("0000-00-00T00:00:00.348Z", patt);
	parseString("0000-00-00T00:00:00.3510234829873+00:00", patt);
	parseString("0000-00-00T00:00:00.3510234829873-00:00", patt);
	parseString("0000-00-00T00:00:00.3510234829873Z", patt);
	parseString("0000-00-00T00:00:00.3490234829873+00:00", patt);
	parseString("0000-00-00T00:00:00.3490234829873-00:00", patt);
	parseString("0000-00-00T00:00:00.3490234829873Z", patt);

	patt = "0000-00-00T00:00:00.35+01:23";
	parseString("0000-00-00T00:00:00.35+01:23", patt);
	parseString("0000-00-00T00:00:00.351+01:23", patt);
	parseString("0000-00-00T00:00:00.348+01:23", patt);
	parseString("0000-00-00T00:00:00.3510234829873+01:23", patt);
	parseString("0000-00-00T00:00:00.3490234829873+01:23", patt);

	patt = "0000-00-00T00:00:00.35-01:23";
	parseString("0000-00-00T00:00:00.35-01:23", patt);
	parseString("0000-00-00T00:00:00.351-01:23", patt);
	parseString("0000-00-00T00:00:00.348-01:23", patt);
	parseString("0000-00-00T00:00:00.3510234829873-01:23", patt);
	parseString("0000-00-00T00:00:00.3490234829873-01:23", patt);
	return true;
}

bool ATOMDateParseTest04() {
	std::string patt = "0000-00-00T00:00:47Z";
	parseString("0000-00-00T00:00:47.00+00:00", patt);
	parseString("0000-00-00T00:00:47.00-00:00", patt);
	parseString("0000-00-00T00:00:47.00Z", patt);
	parseString("0000-00-00T00:00:47+00:00", patt);
	parseString("0000-00-00T00:00:47-00:00", patt);
	parseString("0000-00-00T00:00:47Z", patt);
	
	patt = "0000-00-00T00:00:47.94Z";
	parseString("0000-00-00T00:00:47.94+00:00", patt);
	parseString("0000-00-00T00:00:47.94-00:00", patt);
	parseString("0000-00-00T00:00:47.94Z", patt);
	
	patt = "0000-00-00T00:00:47+01:56";
	parseString("0000-00-00T00:00:47.00+01:56", patt);
	parseString("0000-00-00T00:00:47+01:56", patt);

	patt = "0000-00-00T00:00:47-01:56";
	parseString("0000-00-00T00:00:47.00-01:56", patt);
	parseString("0000-00-00T00:00:47-01:56", patt);
	
	patt = "0000-00-00T00:00:47.94+01:56";
	parseString("0000-00-00T00:00:47.94+01:56", patt);

	patt = "0000-00-00T00:00:47.94-01:56";
	parseString("0000-00-00T00:00:47.94-01:56", patt);

	return true;
}

bool ATOMDateParseTest05() {
	std::string patt = "0000-00-00T00:13:00Z";
	parseString("0000-00-00T00:13:00.00+00:00", patt);
	parseString("0000-00-00T00:13:00.00-00:00", patt);
	parseString("0000-00-00T00:13:00.00Z", patt);
	parseString("0000-00-00T00:13:00+00:00", patt);
	parseString("0000-00-00T00:13:00-00:00", patt);
	parseString("0000-00-00T00:13:00Z", patt);
	parseString("0000-00-00T00:13+00:00", patt);
	parseString("0000-00-00T00:13-00:00", patt);
	parseString("0000-00-00T00:13Z", patt);
	
	patt = "0000-00-00T00:13:56Z";
	parseString("0000-00-00T00:13:56.00+00:00", patt);
	parseString("0000-00-00T00:13:56.00-00:00", patt);
	parseString("0000-00-00T00:13:56.00Z", patt);
	parseString("0000-00-00T00:13:56+00:00", patt);
	parseString("0000-00-00T00:13:56-00:00", patt);
	parseString("0000-00-00T00:13:56Z", patt);
	
	patt = "0000-00-00T00:13:00.48Z";
	parseString("0000-00-00T00:13:00.48+00:00", patt);
	parseString("0000-00-00T00:13:00.48-00:00", patt);
	parseString("0000-00-00T00:13:00.48Z", patt);

	patt = "0000-00-00T00:13:00+12:34";
	parseString("0000-00-00T00:13:00.00+12:34", patt);
	parseString("0000-00-00T00:13:00+12:34", patt);
	parseString("0000-00-00T00:13+12:34", patt);

	patt = "0000-00-00T00:13:00-12:34";
	parseString("0000-00-00T00:13:00.00-12:34", patt);
	parseString("0000-00-00T00:13:00-12:34", patt);
	parseString("0000-00-00T00:13-12:34", patt);

	patt = "0000-00-00T00:13:56.48Z";
	parseString("0000-00-00T00:13:56.48+00:00", patt);
	parseString("0000-00-00T00:13:56.48-00:00", patt);
	parseString("0000-00-00T00:13:56.48Z", patt);

	patt = "0000-00-00T00:13:56+12:34";
	parseString("0000-00-00T00:13:56.00+12:34", patt);
	parseString("0000-00-00T00:13:56+12:34", patt);

	patt = "0000-00-00T00:13:56-12:34";
	parseString("0000-00-00T00:13:56.00-12:34", patt);
	parseString("0000-00-00T00:13:56-12:34", patt);

	patt = "0000-00-00T00:13:00.48+12:34";
	parseString("0000-00-00T00:13:00.48+12:34", patt);

	patt = "0000-00-00T00:13:00.48-12:34";
	parseString("0000-00-00T00:13:00.48-12:34", patt);

	patt = "0000-00-00T00:13:56.48+12:34";
	parseString("0000-00-00T00:13:56.48+12:34", patt);

	patt = "0000-00-00T00:13:56.48-12:34";
	parseString("0000-00-00T00:13:56.48-12:34", patt);

	return true;
}


bool ATOMDateParseTest06() {
	std::string patt = "0000-00-00T18:00:00Z";
	parseString("0000-00-00T18:00:00.00+00:00", patt);
	parseString("0000-00-00T18:00:00.00-00:00", patt);
	parseString("0000-00-00T18:00:00.00Z", patt);
	parseString("0000-00-00T18:00:00+00:00", patt);
	parseString("0000-00-00T18:00:00-00:00", patt);
	parseString("0000-00-00T18:00:00Z", patt);
	parseString("0000-00-00T18:00+00:00", patt);
	parseString("0000-00-00T18:00-00:00", patt);
	parseString("0000-00-00T18:00Z", patt);
	
	patt = "0000-00-00T18:00:56Z";
	parseString("0000-00-00T18:00:56.00+00:00", patt);
	parseString("0000-00-00T18:00:56.00-00:00", patt);
	parseString("0000-00-00T18:00:56.00Z", patt);
	parseString("0000-00-00T18:00:56+00:00", patt);
	parseString("0000-00-00T18:00:56-00:00", patt);
	parseString("0000-00-00T18:00:56Z", patt);
	
	patt = "0000-00-00T18:00:00.48Z";
	parseString("0000-00-00T18:00:00.48+00:00", patt);
	parseString("0000-00-00T18:00:00.48-00:00", patt);
	parseString("0000-00-00T18:00:00.48Z", patt);

	patt = "0000-00-00T18:00:00+12:34";
	parseString("0000-00-00T18:00:00.00+12:34", patt);
	parseString("0000-00-00T18:00:00+12:34", patt);
	parseString("0000-00-00T18:00+12:34", patt);

	patt = "0000-00-00T18:00:00-12:34";
	parseString("0000-00-00T18:00:00.00-12:34", patt);
	parseString("0000-00-00T18:00:00-12:34", patt);
	parseString("0000-00-00T18:00-12:34", patt);

	patt = "0000-00-00T18:00:56.48Z";
	parseString("0000-00-00T18:00:56.48+00:00", patt);
	parseString("0000-00-00T18:00:56.48-00:00", patt);
	parseString("0000-00-00T18:00:56.48Z", patt);

	patt = "0000-00-00T18:00:56+12:34";
	parseString("0000-00-00T18:00:56.00+12:34", patt);
	parseString("0000-00-00T18:00:56+12:34", patt);

	patt = "0000-00-00T18:00:56-12:34";
	parseString("0000-00-00T18:00:56.00-12:34", patt);
	parseString("0000-00-00T18:00:56-12:34", patt);

	patt = "0000-00-00T18:00:00.48+12:34";
	parseString("0000-00-00T18:00:00.48+12:34", patt);

	patt = "0000-00-00T18:00:00.48-12:34";
	parseString("0000-00-00T18:00:00.48-12:34", patt);

	patt = "0000-00-00T18:00:56.48+12:34";
	parseString("0000-00-00T18:00:56.48+12:34", patt);

	patt = "0000-00-00T18:00:56.48-12:34";
	parseString("0000-00-00T18:00:56.48-12:34", patt);

	return true;
}

bool ATOMDateParseTest07() {
	std::string patt = "0000-00-00T18:36:00Z";
	parseString("0000-00-00T18:36:00.00+00:00", patt);
	parseString("0000-00-00T18:36:00.00-00:00", patt);
	parseString("0000-00-00T18:36:00.00Z", patt);
	parseString("0000-00-00T18:36:00+00:00", patt);
	parseString("0000-00-00T18:36:00-00:00", patt);
	parseString("0000-00-00T18:36:00Z", patt);
	parseString("0000-00-00T18:36+00:00", patt);
	parseString("0000-00-00T18:36-00:00", patt);
	parseString("0000-00-00T18:36Z", patt);
	
	patt = "0000-00-00T18:36:56Z";
	parseString("0000-00-00T18:36:56.00+00:00", patt);
	parseString("0000-00-00T18:36:56.00-00:00", patt);
	parseString("0000-00-00T18:36:56.00Z", patt);
	parseString("0000-00-00T18:36:56+00:00", patt);
	parseString("0000-00-00T18:36:56-00:00", patt);
	parseString("0000-00-00T18:36:56Z", patt);
	
	patt = "0000-00-00T18:36:00.48Z";
	parseString("0000-00-00T18:36:00.48+00:00", patt);
	parseString("0000-00-00T18:36:00.48-00:00", patt);
	parseString("0000-00-00T18:36:00.48Z", patt);

	patt = "0000-00-00T18:36:00+12:34";
	parseString("0000-00-00T18:36:00.00+12:34", patt);
	parseString("0000-00-00T18:36:00+12:34", patt);
	parseString("0000-00-00T18:36+12:34", patt);

	patt = "0000-00-00T18:36:00-12:34";
	parseString("0000-00-00T18:36:00.00-12:34", patt);
	parseString("0000-00-00T18:36:00-12:34", patt);
	parseString("0000-00-00T18:36-12:34", patt);

	patt = "0000-00-00T18:36:56.48Z";
	parseString("0000-00-00T18:36:56.48+00:00", patt);
	parseString("0000-00-00T18:36:56.48-00:00", patt);
	parseString("0000-00-00T18:36:56.48Z", patt);

	patt = "0000-00-00T18:36:56+12:34";
	parseString("0000-00-00T18:36:56.00+12:34", patt);
	parseString("0000-00-00T18:36:56+12:34", patt);

	patt = "0000-00-00T18:36:56-12:34";
	parseString("0000-00-00T18:36:56.00-12:34", patt);
	parseString("0000-00-00T18:36:56-12:34", patt);

	patt = "0000-00-00T18:36:00.48+12:34";
	parseString("0000-00-00T18:36:00.48+12:34", patt);

	patt = "0000-00-00T18:36:00.48-12:34";
	parseString("0000-00-00T18:36:00.48-12:34", patt);

	patt = "0000-00-00T18:36:56.48+12:34";
	parseString("0000-00-00T18:36:56.48+12:34", patt);

	patt = "0000-00-00T18:36:56.48-12:34";
	parseString("0000-00-00T18:36:56.48-12:34", patt);

	return true;
}


bool ATOMDateParseTest08() {
	std::string patt = "2003-09-18T00:00:00Z";
	parseString("2003-09-18T00:00:00.00+00:00", patt);
	parseString("2003-09-18T00:00:00.00-00:00", patt);
	parseString("2003-09-18T00:00:00.00Z", patt);
	parseString("2003-09-18T00:00:00+00:00", patt);
	parseString("2003-09-18T00:00:00-00:00", patt);
	parseString("2003-09-18T00:00:00Z", patt);
	parseString("2003-09-18T00:00+00:00", patt);
	parseString("2003-09-18T00:00-00:00", patt);
	parseString("2003-09-18T00:00Z", patt);
	parseString("2003-09-18", patt);

	patt = "2003-09-18T12:45:00Z";
	parseString("2003-09-18T12:45:00.00+00:00", patt);
	parseString("2003-09-18T12:45:00.00-00:00", patt);
	parseString("2003-09-18T12:45:00.00Z", patt);
	parseString("2003-09-18T12:45:00+00:00", patt);
	parseString("2003-09-18T12:45:00-00:00", patt);
	parseString("2003-09-18T12:45:00Z", patt);
	parseString("2003-09-18T12:45+00:00", patt);
	parseString("2003-09-18T12:45-00:00", patt);
	parseString("2003-09-18T12:45Z", patt);

	patt = "2003-09-18T00:00:34Z";
	parseString("2003-09-18T00:00:34.00+00:00", patt);
	parseString("2003-09-18T00:00:34.00-00:00", patt);
	parseString("2003-09-18T00:00:34.00Z", patt);
	parseString("2003-09-18T00:00:34+00:00", patt);
	parseString("2003-09-18T00:00:34-00:00", patt);
	parseString("2003-09-18T00:00:34Z", patt);

	patt = "2003-09-18T12:45:34Z";
	parseString("2003-09-18T12:45:34.00+00:00", patt);
	parseString("2003-09-18T12:45:34.00-00:00", patt);
	parseString("2003-09-18T12:45:34.00Z", patt);
	parseString("2003-09-18T12:45:34+00:00", patt);
	parseString("2003-09-18T12:45:34-00:00", patt);
	parseString("2003-09-18T12:45:34Z", patt);

	patt = "2003-09-18T00:00:00.93Z";
	parseString("2003-09-18T00:00:00.93+00:00", patt);
	parseString("2003-09-18T00:00:00.93-00:00", patt);
	parseString("2003-09-18T00:00:00.93Z", patt);

	patt = "2003-09-18T12:45:00.93Z";
	parseString("2003-09-18T12:45:00.93+00:00", patt);
	parseString("2003-09-18T12:45:00.93-00:00", patt);
	parseString("2003-09-18T12:45:00.93Z", patt);

	patt = "2003-09-18T00:00:34.93Z";
	parseString("2003-09-18T00:00:34.93+00:00", patt);
	parseString("2003-09-18T00:00:34.93-00:00", patt);
	parseString("2003-09-18T00:00:34.93Z", patt);

	patt = "2003-09-18T12:45:34.93Z";
	parseString("2003-09-18T12:45:34.93+00:00", patt);
	parseString("2003-09-18T12:45:34.93-00:00", patt);
	parseString("2003-09-18T12:45:34.93Z", patt);

	patt = "2003-09-18T00:00:00+12:34";
	parseString("2003-09-18T00:00:00.00+12:34", patt);
	parseString("2003-09-18T00:00:00+12:34", patt);
	parseString("2003-09-18T00:00+12:34", patt);

	patt = "2003-09-18T12:45:00+12:34";
	parseString("2003-09-18T12:45:00.00+12:34", patt);
	parseString("2003-09-18T12:45:00+12:34", patt);
	parseString("2003-09-18T12:45+12:34", patt);

	patt = "2003-09-18T00:00:34+12:34";
	parseString("2003-09-18T00:00:34.00+12:34", patt);
	parseString("2003-09-18T00:00:34+12:34", patt);

	patt = "2003-09-18T12:45:34+12:34";
	parseString("2003-09-18T12:45:34.00+12:34", patt);
	parseString("2003-09-18T12:45:34+12:34", patt);

	patt = "2003-09-18T00:00:00.93+12:34";
	parseString("2003-09-18T00:00:00.93+12:34", patt);

	patt = "2003-09-18T12:45:00.93+12:34";
	parseString("2003-09-18T12:45:00.93+12:34", patt);

	patt = "2003-09-18T00:00:34.93+12:34";
	parseString("2003-09-18T00:00:34.93+12:34", patt);

	patt = "2003-09-18T12:45:34.93+12:34";
	parseString("2003-09-18T12:45:34.93+12:34", patt);

	patt = "2003-09-18T00:00:00-12:34";
	parseString("2003-09-18T00:00:00.00-12:34", patt);
	parseString("2003-09-18T00:00:00-12:34", patt);
	parseString("2003-09-18T00:00-12:34", patt);

	patt = "2003-09-18T12:45:00-12:34";
	parseString("2003-09-18T12:45:00.00-12:34", patt);
	parseString("2003-09-18T12:45:00-12:34", patt);
	parseString("2003-09-18T12:45-12:34", patt);

	patt = "2003-09-18T00:00:34-12:34";
	parseString("2003-09-18T00:00:34.00-12:34", patt);
	parseString("2003-09-18T00:00:34-12:34", patt);

	patt = "2003-09-18T12:45:34-12:34";
	parseString("2003-09-18T12:45:34.00-12:34", patt);
	parseString("2003-09-18T12:45:34-12:34", patt);

	patt = "2003-09-18T00:00:00.93-12:34";
	parseString("2003-09-18T00:00:00.93-12:34", patt);

	patt = "2003-09-18T12:45:00.93-12:34";
	parseString("2003-09-18T12:45:00.93-12:34", patt);

	patt = "2003-09-18T00:00:34.93-12:34";
	parseString("2003-09-18T00:00:34.93-12:34", patt);

	patt = "2003-09-18T12:45:34.93-12:34";
	parseString("2003-09-18T12:45:34.93-12:34", patt);

	return true;
}

bool ATOMDateParseTest09() {
	std::string patt = "2009-12-00T00:00:00Z";
	parseString("2009-12-00T00:00:00.00+00:00", patt);
	parseString("2009-12-00T00:00:00.00-00:00", patt);
	parseString("2009-12-00T00:00:00.00Z", patt);
	parseString("2009-12-00T00:00:00+00:00", patt);
	parseString("2009-12-00T00:00:00-00:00", patt);
	parseString("2009-12-00T00:00:00Z", patt);
	parseString("2009-12-00T00:00+00:00", patt);
	parseString("2009-12-00T00:00-00:00", patt);
	parseString("2009-12-00T00:00Z", patt);
	parseString("2009-12-00", patt);
	parseString("2009-12", patt);
	return true;
}

bool ATOMDateParseTest10() {
	std::string patt = "2007-00-00T00:00:00Z";
	parseString("2007-00-00T00:00:00.00+00:00", patt);
	parseString("2007-00-00T00:00:00.00-00:00", patt);
	parseString("2007-00-00T00:00:00.00Z", patt);
	parseString("2007-00-00T00:00:00+00:00", patt);
	parseString("2007-00-00T00:00:00-00:00", patt);
	parseString("2007-00-00T00:00:00Z", patt);
	parseString("2007-00-00T00:00+00:00", patt);
	parseString("2007-00-00T00:00-00:00", patt);
	parseString("2007-00-00T00:00Z", patt);
	parseString("2007-00-00", patt);
	parseString("2007-00", patt);
	parseString("2007", patt);
	return true;
}


BEGIN_TESTCASE(atomdateparsetestcase)
	TEST_ELEMENT(ATOMDateParseTest01)
	TEST_ELEMENT(ATOMDateParseTest02)
	TEST_ELEMENT(ATOMDateParseTest03)
	TEST_ELEMENT(ATOMDateParseTest04)
	TEST_ELEMENT(ATOMDateParseTest05)
	TEST_ELEMENT(ATOMDateParseTest06)
	TEST_ELEMENT(ATOMDateParseTest07)
	TEST_ELEMENT(ATOMDateParseTest08)
	TEST_ELEMENT(ATOMDateParseTest09)
	TEST_ELEMENT(ATOMDateParseTest10)
END_TESTCASE;

