/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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



bool ATOMDateTest01() {
	// test years
	ATOMDateConstruct date1(0, 0, 0);
	complainIf(date1.getDateTime() != "0000-00-00T00:00:00Z");
	complainIf(date1.getDateTime(true) != "0000");
	ATOMDateConstruct date2(1990, 0, 0);
	complainIf(date2.getDateTime() != "1990-00-00T00:00:00Z");
	complainIf(date2.getDateTime(true) != "1990");
	return true;
}

bool ATOMDateTest02() {
	// test month, any year
	ATOMDateConstruct date1(1990, 5, 0);
	complainIf(date1.getDateTime() != "1990-05-00T00:00:00Z");
	complainIf(date1.getDateTime(true) != "1990-05");
	ATOMDateConstruct date2(1997, 11, 0);
	complainIf(date2.getDateTime() != "1997-11-00T00:00:00Z");
	complainIf(date2.getDateTime(true) != "1997-11");
	ATOMDateConstruct date3(0, 5, 0);
	complainIf(date3.getDateTime() != "0000-05-00T00:00:00Z");
	complainIf(date3.getDateTime(true) != "0000-05");
	return true;
}

bool ATOMDateTest03() {
	// test days, year > 0, month > 0
	ATOMDateConstruct date1(2003, 4, 2);
	complainIf(date1.getDateTime() != "2003-04-02T00:00:00Z");
	complainIf(date1.getDateTime(true) != "2003-04-02");
	ATOMDateConstruct date2(2003, 10, 2);
	complainIf(date2.getDateTime() != "2003-10-02T00:00:00Z");
	complainIf(date2.getDateTime(true) != "2003-10-02");
	ATOMDateConstruct date3(2003, 4, 20);
	complainIf(date3.getDateTime() != "2003-04-20T00:00:00Z");
	complainIf(date3.getDateTime(true) != "2003-04-20");
	ATOMDateConstruct date4(2003, 12, 15);
	complainIf(date4.getDateTime() != "2003-12-15T00:00:00Z");
	complainIf(date4.getDateTime(true) != "2003-12-15");
	return true;
}

bool ATOMDateTest04() {
	// test days, year = 0, month > 0
	ATOMDateConstruct date1(0, 4, 2);
	complainIf(date1.getDateTime() != "0000-04-02T00:00:00Z");
	complainIf(date1.getDateTime(true) != "0000-04-02");
	ATOMDateConstruct date2(0, 10, 2);
	complainIf(date2.getDateTime() != "0000-10-02T00:00:00Z");
	complainIf(date2.getDateTime(true) != "0000-10-02");
	ATOMDateConstruct date3(0, 4, 20);
	complainIf(date3.getDateTime() != "0000-04-20T00:00:00Z");
	complainIf(date3.getDateTime(true) != "0000-04-20");
	ATOMDateConstruct date4(0, 12, 15);
	complainIf(date4.getDateTime() != "0000-12-15T00:00:00Z");
	complainIf(date4.getDateTime(true) != "0000-12-15");
	return true;
}

bool ATOMDateTest05() {
	// test days, year > 0, month = 0
	ATOMDateConstruct date1(2003, 0, 2);
	complainIf(date1.getDateTime() != "2003-00-02T00:00:00Z");
	complainIf(date1.getDateTime(true) != "2003-00-02");
	ATOMDateConstruct date2(2003, 0, 2);
	complainIf(date2.getDateTime() != "2003-00-02T00:00:00Z");
	complainIf(date2.getDateTime(true) != "2003-00-02");
	ATOMDateConstruct date3(2003, 0, 20);
	complainIf(date3.getDateTime() != "2003-00-20T00:00:00Z");
	complainIf(date3.getDateTime(true) != "2003-00-20");
	ATOMDateConstruct date4(2003, 0, 15);
	complainIf(date4.getDateTime() != "2003-00-15T00:00:00Z");
	complainIf(date4.getDateTime(true) != "2003-00-15");
	return true;
}

bool ATOMDateTest06() {
	// test days, year = 0, month = 0
	ATOMDateConstruct date1(0, 0, 2);
	complainIf(date1.getDateTime() != "0000-00-02T00:00:00Z");
	complainIf(date1.getDateTime(true) != "0000-00-02");
	ATOMDateConstruct date2(0, 0, 2);
	complainIf(date2.getDateTime() != "0000-00-02T00:00:00Z");
	complainIf(date2.getDateTime(true) != "0000-00-02");
	ATOMDateConstruct date3(0, 0, 20);
	complainIf(date3.getDateTime() != "0000-00-20T00:00:00Z");
	complainIf(date3.getDateTime(true) != "0000-00-20");
	ATOMDateConstruct date4(0, 0, 15);
	complainIf(date4.getDateTime() != "0000-00-15T00:00:00Z");
	complainIf(date4.getDateTime(true) != "0000-00-15");
	return true;
}


bool ATOMDateTest07() {
	// test hours, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 0, 0);
	complainIf(date1.getDateTime() != "2003-12-15T08:00:00Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:00Z");
	ATOMDateConstruct date2(0, 12, 15, 8, 0, 0);
	complainIf(date2.getDateTime() != "0000-12-15T08:00:00Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:00Z");
	ATOMDateConstruct date3(2003, 0, 20, 8, 0, 0);
	complainIf(date3.getDateTime() != "2003-00-20T08:00:00Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:00Z");
	ATOMDateConstruct date4(0, 0, 20, 8, 0, 0);
	complainIf(date4.getDateTime() != "0000-00-20T08:00:00Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:00Z");
	ATOMDateConstruct date5(0, 0, 0, 8, 0, 0);
	complainIf(date5.getDateTime() != "0000-00-00T08:00:00Z");
	complainIf(date5.getDateTime(true) != "0000-00-00T08:00Z");
	ATOMDateConstruct date6(2003, 12, 15, 10, 0, 0);
	complainIf(date6.getDateTime() != "2003-12-15T10:00:00Z");
	complainIf(date6.getDateTime(true) != "2003-12-15T10:00Z");
	ATOMDateConstruct date7(0, 12, 15, 11, 0, 0);
	complainIf(date7.getDateTime() != "0000-12-15T11:00:00Z");
	complainIf(date7.getDateTime(true) != "0000-12-15T11:00Z");
	ATOMDateConstruct date8(2003, 0, 20, 18, 0, 0);
	complainIf(date8.getDateTime() != "2003-00-20T18:00:00Z");
	complainIf(date8.getDateTime(true) != "2003-00-20T18:00Z");
	ATOMDateConstruct date9(0, 0, 20, 23, 0, 0);
	complainIf(date9.getDateTime() != "0000-00-20T23:00:00Z");
	complainIf(date9.getDateTime(true) != "0000-00-20T23:00Z");
	ATOMDateConstruct date0(0, 0, 0, 16, 0, 0);
	complainIf(date0.getDateTime() != "0000-00-00T16:00:00Z");
	complainIf(date0.getDateTime(true) != "0000-00-00T16:00Z");
	return true;
}


bool ATOMDateTest08() {
	// test minutes, hours > 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 1, 0);
	complainIf(date1.getDateTime() != "2003-12-15T08:01:00Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:01Z");
	ATOMDateConstruct date2(0, 12, 15, 8, 5, 0);
	complainIf(date2.getDateTime() != "0000-12-15T08:05:00Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:05Z");
	ATOMDateConstruct date3(2003, 0, 20, 8, 15, 0);
	complainIf(date3.getDateTime() != "2003-00-20T08:15:00Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:15Z");
	ATOMDateConstruct date4(0, 0, 20, 8, 27, 0);
	complainIf(date4.getDateTime() != "0000-00-20T08:27:00Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:27Z");
	ATOMDateConstruct date5(2003, 12, 15, 10, 36, 0);
	complainIf(date5.getDateTime() != "2003-12-15T10:36:00Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T10:36Z");
	ATOMDateConstruct date6(0, 12, 15, 11, 48, 0);
	complainIf(date6.getDateTime() != "0000-12-15T11:48:00Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T11:48Z");
	ATOMDateConstruct date7(2003, 0, 20, 18, 59, 0);
	complainIf(date7.getDateTime() != "2003-00-20T18:59:00Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T18:59Z");
	ATOMDateConstruct date8(0, 0, 20, 23, 11, 0);
	complainIf(date8.getDateTime() != "0000-00-20T23:11:00Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T23:11Z");
	ATOMDateConstruct date9(0, 0, 0, 8, 27, 0);
	complainIf(date9.getDateTime() != "0000-00-00T08:27:00Z");
	complainIf(date9.getDateTime(true) != "0000-00-00T08:27Z");
	ATOMDateConstruct date0(0, 0, 0, 23, 11, 0);
	complainIf(date0.getDateTime() != "0000-00-00T23:11:00Z");
	complainIf(date0.getDateTime(true) != "0000-00-00T23:11Z");
	return true;
}

bool ATOMDateTest09() {
	// test minutes, hours = 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 0, 1, 0);
	complainIf(date1.getDateTime() != "2003-12-15T00:01:00Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T00:01Z");
	ATOMDateConstruct date2(0, 12, 15, 0, 5, 0);
	complainIf(date2.getDateTime() != "0000-12-15T00:05:00Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T00:05Z");
	ATOMDateConstruct date3(2003, 0, 20, 0, 15, 0);
	complainIf(date3.getDateTime() != "2003-00-20T00:15:00Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T00:15Z");
	ATOMDateConstruct date4(0, 0, 20, 0, 27, 0);
	complainIf(date4.getDateTime() != "0000-00-20T00:27:00Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T00:27Z");
	ATOMDateConstruct date5(2003, 12, 15, 0, 36, 0);
	complainIf(date5.getDateTime() != "2003-12-15T00:36:00Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T00:36Z");
	ATOMDateConstruct date6(0, 12, 15, 0, 48, 0);
	complainIf(date6.getDateTime() != "0000-12-15T00:48:00Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T00:48Z");
	ATOMDateConstruct date7(2003, 0, 20, 0, 59, 0);
	complainIf(date7.getDateTime() != "2003-00-20T00:59:00Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T00:59Z");
	ATOMDateConstruct date8(0, 0, 20, 0, 11, 0);
	complainIf(date8.getDateTime() != "0000-00-20T00:11:00Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T00:11Z");
	ATOMDateConstruct date9(0, 0, 0, 0, 27, 0);
	complainIf(date9.getDateTime() != "0000-00-00T00:27:00Z");
	complainIf(date9.getDateTime(true) != "0000-00-00T00:27Z");
	ATOMDateConstruct date0(0, 0, 0, 0, 11, 0);
	complainIf(date0.getDateTime() != "0000-00-00T00:11:00Z");
	complainIf(date0.getDateTime(true) != "0000-00-00T00:11Z");
	return true;
}


bool ATOMDateTest10() {
	// test seconds, minutes > 0, hours > 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 1, 13);
	complainIf(date1.getDateTime() != "2003-12-15T08:01:13Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:01:13Z");
	ATOMDateConstruct date2(0, 12, 15, 8, 5, 1);
	complainIf(date2.getDateTime() != "0000-12-15T08:05:01Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:05:01Z");
	ATOMDateConstruct date3(2003, 0, 20, 8, 15, 20);
	complainIf(date3.getDateTime() != "2003-00-20T08:15:20Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:15:20Z");
	ATOMDateConstruct date4(0, 0, 20, 8, 27, 27);
	complainIf(date4.getDateTime() != "0000-00-20T08:27:27Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:27:27Z");
	ATOMDateConstruct date5(2003, 12, 15, 10, 36, 38);
	complainIf(date5.getDateTime() != "2003-12-15T10:36:38Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T10:36:38Z");
	ATOMDateConstruct date6(0, 12, 15, 11, 48, 46);
	complainIf(date6.getDateTime() != "0000-12-15T11:48:46Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T11:48:46Z");
	ATOMDateConstruct date7(2003, 0, 20, 18, 59, 59);
	complainIf(date7.getDateTime() != "2003-00-20T18:59:59Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T18:59:59Z");
	ATOMDateConstruct date8(0, 0, 20, 23, 11, 50);
	complainIf(date8.getDateTime() != "0000-00-20T23:11:50Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T23:11:50Z");
	return true;
}

bool ATOMDateTest11() {
	// test seconds, minutes > 0, hours = 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 0, 1, 17);
	complainIf(date1.getDateTime() != "2003-12-15T00:01:17Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T00:01:17Z");
	ATOMDateConstruct date2(0, 12, 15, 0, 5, 28);
	complainIf(date2.getDateTime() != "0000-12-15T00:05:28Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T00:05:28Z");
	ATOMDateConstruct date3(2003, 0, 20, 0, 15, 39);
	complainIf(date3.getDateTime() != "2003-00-20T00:15:39Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T00:15:39Z");
	ATOMDateConstruct date4(0, 0, 20, 0, 27, 41);
	complainIf(date4.getDateTime() != "0000-00-20T00:27:41Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T00:27:41Z");
	ATOMDateConstruct date5(2003, 12, 15, 0, 36, 58);
	complainIf(date5.getDateTime() != "2003-12-15T00:36:58Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T00:36:58Z");
	ATOMDateConstruct date6(0, 12, 15, 0, 48, 1);
	complainIf(date6.getDateTime() != "0000-12-15T00:48:01Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T00:48:01Z");
	ATOMDateConstruct date7(2003, 0, 20, 0, 59, 4);
	complainIf(date7.getDateTime() != "2003-00-20T00:59:04Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T00:59:04Z");
	ATOMDateConstruct date8(0, 0, 20, 0, 11, 9);
	complainIf(date8.getDateTime() != "0000-00-20T00:11:09Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T00:11:09Z");
	return true;
}

bool ATOMDateTest12() {
	// test seconds, minutes = 0, hours > 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 0, 13);
	complainIf(date1.getDateTime() != "2003-12-15T08:00:13Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:00:13Z");
	ATOMDateConstruct date2(0, 12, 15, 8, 0, 1);
	complainIf(date2.getDateTime() != "0000-12-15T08:00:01Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:00:01Z");
	ATOMDateConstruct date3(2003, 0, 20, 8, 0, 20);
	complainIf(date3.getDateTime() != "2003-00-20T08:00:20Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:00:20Z");
	ATOMDateConstruct date4(0, 0, 20, 8, 0, 27);
	complainIf(date4.getDateTime() != "0000-00-20T08:00:27Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:00:27Z");
	ATOMDateConstruct date5(2003, 12, 15, 10, 0, 38);
	complainIf(date5.getDateTime() != "2003-12-15T10:00:38Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T10:00:38Z");
	ATOMDateConstruct date6(0, 12, 15, 11, 0, 46);
	complainIf(date6.getDateTime() != "0000-12-15T11:00:46Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T11:00:46Z");
	ATOMDateConstruct date7(2003, 0, 20, 18, 0, 59);
	complainIf(date7.getDateTime() != "2003-00-20T18:00:59Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T18:00:59Z");
	ATOMDateConstruct date8(0, 0, 20, 23, 0, 50);
	complainIf(date8.getDateTime() != "0000-00-20T23:00:50Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T23:00:50Z");
	return true;
}

bool ATOMDateTest13() {
	// test seconds, minutes = 0, hours = 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 0, 0, 17);
	complainIf(date1.getDateTime() != "2003-12-15T00:00:17Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T00:00:17Z");
	ATOMDateConstruct date2(0, 12, 15, 0, 0, 28);
	complainIf(date2.getDateTime() != "0000-12-15T00:00:28Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T00:00:28Z");
	ATOMDateConstruct date3(2003, 0, 20, 0, 0, 39);
	complainIf(date3.getDateTime() != "2003-00-20T00:00:39Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T00:00:39Z");
	ATOMDateConstruct date4(0, 0, 20, 0, 0, 41);
	complainIf(date4.getDateTime() != "0000-00-20T00:00:41Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T00:00:41Z");
	ATOMDateConstruct date5(2003, 12, 15, 0, 0, 58);
	complainIf(date5.getDateTime() != "2003-12-15T00:00:58Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T00:00:58Z");
	ATOMDateConstruct date6(0, 12, 15, 0, 0, 1);
	complainIf(date6.getDateTime() != "0000-12-15T00:00:01Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T00:00:01Z");
	ATOMDateConstruct date7(2003, 0, 20, 0, 0, 4);
	complainIf(date7.getDateTime() != "2003-00-20T00:00:04Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T00:00:04Z");
	ATOMDateConstruct date8(0, 0, 20, 0, 0, 9);
	complainIf(date8.getDateTime() != "0000-00-20T00:00:09Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T00:00:09Z");
	return true;
}

bool ATOMDateTest14() {
	// test sfraction, any seconds, minutes > 0, hours > 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 1, 13, 0.1234);
	complainIf(date1.getDateTime() != "2003-12-15T08:01:13.12Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:01:13.12Z");
	ATOMDateConstruct date2(0, 12, 15, 8, 5, 1, 0.1234);
	complainIf(date2.getDateTime() != "0000-12-15T08:05:01.12Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:05:01.12Z");
	ATOMDateConstruct date3(2003, 0, 20, 8, 15, 20, 0.1234);
	complainIf(date3.getDateTime() != "2003-00-20T08:15:20.12Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:15:20.12Z");
	ATOMDateConstruct date4(0, 0, 20, 8, 27, 27, 0.1234);
	complainIf(date4.getDateTime() != "0000-00-20T08:27:27.12Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:27:27.12Z");
	ATOMDateConstruct date5(2003, 12, 15, 10, 36, 0, 0.1234);
	complainIf(date5.getDateTime() != "2003-12-15T10:36:00.12Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T10:36:00.12Z");
	ATOMDateConstruct date6(0, 12, 15, 11, 48, 0, 0.1234);
	complainIf(date6.getDateTime() != "0000-12-15T11:48:00.12Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T11:48:00.12Z");
	ATOMDateConstruct date7(2003, 0, 20, 18, 59, 0, 0.1234);
	complainIf(date7.getDateTime() != "2003-00-20T18:59:00.12Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T18:59:00.12Z");
	ATOMDateConstruct date8(0, 0, 20, 23, 11, 0, 0.1234);
	complainIf(date8.getDateTime() != "0000-00-20T23:11:00.12Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T23:11:00.12Z");
	return true;
}

bool ATOMDateTest15() {
	// test sfraction, any seconds, minutes > 0, hours = 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 0, 1, 17, 0.9876);
	complainIf(date1.getDateTime() != "2003-12-15T00:01:17.99Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T00:01:17.99Z");
	ATOMDateConstruct date2(0, 12, 15, 0, 5, 28, 0.9876);
	complainIf(date2.getDateTime() != "0000-12-15T00:05:28.99Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T00:05:28.99Z");
	ATOMDateConstruct date3(2003, 0, 20, 0, 15, 39, 0.9876);
	complainIf(date3.getDateTime() != "2003-00-20T00:15:39.99Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T00:15:39.99Z");
	ATOMDateConstruct date4(0, 0, 20, 0, 27, 41, 0.9876);
	complainIf(date4.getDateTime() != "0000-00-20T00:27:41.99Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T00:27:41.99Z");
	ATOMDateConstruct date5(2003, 12, 15, 0, 36, 0, 0.9876);
	complainIf(date5.getDateTime() != "2003-12-15T00:36:00.99Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T00:36:00.99Z");
	ATOMDateConstruct date6(0, 12, 15, 0, 48, 0, 0.9876);
	complainIf(date6.getDateTime() != "0000-12-15T00:48:00.99Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T00:48:00.99Z");
	ATOMDateConstruct date7(2003, 0, 20, 0, 59, 0, 0.9876);
	complainIf(date7.getDateTime() != "2003-00-20T00:59:00.99Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T00:59:00.99Z");
	ATOMDateConstruct date8(0, 0, 20, 0, 11, 0, 0.9876);
	complainIf(date8.getDateTime() != "0000-00-20T00:11:00.99Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T00:11:00.99Z");
	return true;
}

bool ATOMDateTest16() {
	// test sfraction, any seconds, minutes = 0, hours > 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 0, 13, 0.2926);
	complainIf(date1.getDateTime() != "2003-12-15T08:00:13.29Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:00:13.29Z");
	ATOMDateConstruct date2(0, 12, 15, 8, 0, 1, 0.2926);
	complainIf(date2.getDateTime() != "0000-12-15T08:00:01.29Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:00:01.29Z");
	ATOMDateConstruct date3(2003, 0, 20, 8, 0, 20, 0.2926);
	complainIf(date3.getDateTime() != "2003-00-20T08:00:20.29Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:00:20.29Z");
	ATOMDateConstruct date4(0, 0, 20, 8, 0, 27, 0.2926);
	complainIf(date4.getDateTime() != "0000-00-20T08:00:27.29Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:00:27.29Z");
	ATOMDateConstruct date5(2003, 12, 15, 10, 0, 0, 0.2926);
	complainIf(date5.getDateTime() != "2003-12-15T10:00:00.29Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T10:00:00.29Z");
	ATOMDateConstruct date6(0, 12, 15, 11, 0, 0, 0.2926);
	complainIf(date6.getDateTime() != "0000-12-15T11:00:00.29Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T11:00:00.29Z");
	ATOMDateConstruct date7(2003, 0, 20, 18, 0, 0, 0.2926);
	complainIf(date7.getDateTime() != "2003-00-20T18:00:00.29Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T18:00:00.29Z");
	ATOMDateConstruct date8(0, 0, 20, 23, 0, 0, 0.2926);
	complainIf(date8.getDateTime() != "0000-00-20T23:00:00.29Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T23:00:00.29Z");
	return true;
}

bool ATOMDateTest17() {
	// test sfraction, any seconds, minutes = 0, hours = 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 0, 0, 17, 0.4895);
	complainIf(date1.getDateTime() != "2003-12-15T00:00:17.49Z");
	complainIf(date1.getDateTime(true) != "2003-12-15T00:00:17.49Z");
	ATOMDateConstruct date2(0, 12, 15, 0, 0, 28, 0.4895);
	complainIf(date2.getDateTime() != "0000-12-15T00:00:28.49Z");
	complainIf(date2.getDateTime(true) != "0000-12-15T00:00:28.49Z");
	ATOMDateConstruct date3(2003, 0, 20, 0, 0, 39, 0.4895);
	complainIf(date3.getDateTime() != "2003-00-20T00:00:39.49Z");
	complainIf(date3.getDateTime(true) != "2003-00-20T00:00:39.49Z");
	ATOMDateConstruct date4(0, 0, 20, 0, 0, 41, 0.4895);
	complainIf(date4.getDateTime() != "0000-00-20T00:00:41.49Z");
	complainIf(date4.getDateTime(true) != "0000-00-20T00:00:41.49Z");
	ATOMDateConstruct date5(2003, 12, 15, 0, 0, 0, 0.4895);
	complainIf(date5.getDateTime() != "2003-12-15T00:00:00.49Z");
	complainIf(date5.getDateTime(true) != "2003-12-15T00:00:00.49Z");
	ATOMDateConstruct date6(0, 12, 15, 0, 0, 0, 0.4895);
	complainIf(date6.getDateTime() != "0000-12-15T00:00:00.49Z");
	complainIf(date6.getDateTime(true) != "0000-12-15T00:00:00.49Z");
	ATOMDateConstruct date7(2003, 0, 20, 0, 0, 0, 0.4895);
	complainIf(date7.getDateTime() != "2003-00-20T00:00:00.49Z");
	complainIf(date7.getDateTime(true) != "2003-00-20T00:00:00.49Z");
	ATOMDateConstruct date8(0, 0, 20, 0, 0, 0, 0.4895);
	complainIf(date8.getDateTime() != "0000-00-20T00:00:00.49Z");
	complainIf(date8.getDateTime(true) != "0000-00-20T00:00:00.49Z");
	ATOMDateConstruct date9(0, 0, 0, 0, 0, 0, 0.4895);
	complainIf(date9.getDateTime() != "0000-00-00T00:00:00.49Z");
	complainIf(date9.getDateTime(true) != "0000-00-00T00:00:00.49Z");
	return true;
}

bool ATOMDateTest18() {
	ATOMDateConstruct date1(0, 0, 0, 0, 0, 47, 0.94);
	complainIf(date1.getDateTime() != "0000-00-00T00:00:47.94Z");

	ATOMDateConstruct date2(0, 0, 0, 0, 0, 47, 0.945001);
	complainIf(date2.getDateTime() != "0000-00-00T00:00:47.95Z");

	ATOMDateConstruct date3(0, 0, 0, 0, 0, 47, 0.935);
	complainIf(date3.getDateTime() != "0000-00-00T00:00:47.94Z");

	ATOMDateConstruct date4(0, 0, 0, 0, 0, 47, 0.934);
	complainIf(date4.getDateTime() != "0000-00-00T00:00:47.93Z");

	ATOMDateConstruct date5(0, 0, 0, 0, 0, 47, 0.936);
	complainIf(date5.getDateTime() != "0000-00-00T00:00:47.94Z");

	return true;
}



BEGIN_TESTCASE(atomdatetestcase)
	TEST_ELEMENT(ATOMDateTest01)
	TEST_ELEMENT(ATOMDateTest02)
	TEST_ELEMENT(ATOMDateTest03)
	TEST_ELEMENT(ATOMDateTest04)
	TEST_ELEMENT(ATOMDateTest05)
	TEST_ELEMENT(ATOMDateTest06)
	TEST_ELEMENT(ATOMDateTest07)
	TEST_ELEMENT(ATOMDateTest08)
	TEST_ELEMENT(ATOMDateTest09)
	TEST_ELEMENT(ATOMDateTest10)
	TEST_ELEMENT(ATOMDateTest11)
	TEST_ELEMENT(ATOMDateTest12)
	TEST_ELEMENT(ATOMDateTest13)
	TEST_ELEMENT(ATOMDateTest14)
	TEST_ELEMENT(ATOMDateTest15)
	TEST_ELEMENT(ATOMDateTest16)
	TEST_ELEMENT(ATOMDateTest17)
	TEST_ELEMENT(ATOMDateTest18)
END_TESTCASE;

