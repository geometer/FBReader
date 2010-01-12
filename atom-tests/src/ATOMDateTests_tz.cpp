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


#include "Tests.h"

#include "network/atom/ATOMConstructs.h"



bool ATOMDateTestTZ01() {
	// test years
	ATOMDateConstruct date1(0, 0, 0, 0, 0, 0, 0, 2, 20);
	complainIf(date1.getDateTime()     != "0000-00-00T00:00:00+02:20");
	complainIf(date1.getDateTime(true) != "0000-00-00T00:00+02:20");
	ATOMDateConstruct date2(1990, 0, 0, 0, 0, 0, 0, 0, 50);
	complainIf(date2.getDateTime()     != "1990-00-00T00:00:00+00:50");
	complainIf(date2.getDateTime(true) != "1990-00-00T00:00+00:50");
	ATOMDateConstruct date3(1990, 0, 0, 0, 0, 0, 0, 3, 0);
	complainIf(date3.getDateTime()     != "1990-00-00T00:00:00+03:00");
	complainIf(date3.getDateTime(true) != "1990-00-00T00:00+03:00");
	ATOMDateConstruct date4(0, 0, 0, 0, 0, 0, 0, -2, -20);
	complainIf(date4.getDateTime()     != "0000-00-00T00:00:00-02:20");
	complainIf(date4.getDateTime(true) != "0000-00-00T00:00-02:20");
	ATOMDateConstruct date5(0, 0, 0, 0, 0, 0, 0, -2, 20);
	complainIf(date5.getDateTime()     != "0000-00-00T00:00:00-01:40");
	complainIf(date5.getDateTime(true) != "0000-00-00T00:00-01:40");
	ATOMDateConstruct date6(1990, 0, 0, 0, 0, 0, 0, -3, 0);
	complainIf(date6.getDateTime()     != "1990-00-00T00:00:00-03:00");
	complainIf(date6.getDateTime(true) != "1990-00-00T00:00-03:00");
	ATOMDateConstruct date7(1990, 0, 0, 0, 0, 0, 0, 1, -50);
	complainIf(date7.getDateTime()     != "1990-00-00T00:00:00+00:10");
	complainIf(date7.getDateTime(true) != "1990-00-00T00:00+00:10");
	ATOMDateConstruct date8(1990, 0, 0, 0, 0, 0, 0, 0, -20);
	complainIf(date8.getDateTime()     != "1990-00-00T00:00:00-00:20");
	complainIf(date8.getDateTime(true) != "1990-00-00T00:00-00:20");
	return true;
}

bool ATOMDateTestTZ02() {
	// test month, any year
	ATOMDateConstruct date1(1990, 5, 0, 0, 0, 0, 0, 5, 20);
	complainIf(date1.getDateTime()     != "1990-05-00T00:00:00+05:20");
	complainIf(date1.getDateTime(true) != "1990-05-00T00:00+05:20");
	ATOMDateConstruct date2(1997, 11, 0, 0, 0, 0, 0, 2, 0);
	complainIf(date2.getDateTime()     != "1997-11-00T00:00:00+02:00");
	complainIf(date2.getDateTime(true) != "1997-11-00T00:00+02:00");
	ATOMDateConstruct date3(0, 5, 0, 0, 0, 0, 0, 1, 50);
	complainIf(date3.getDateTime()     != "0000-05-00T00:00:00+01:50");
	complainIf(date3.getDateTime(true) != "0000-05-00T00:00+01:50");
	return true;
}

bool ATOMDateTestTZ03() {
	// test hours, any date
	ATOMDateConstruct date6(2003, 12, 15, 10, 0, 0, 0, 1, 0);
	complainIf(date6.getDateTime() != "2003-12-15T10:00:00+01:00");
	complainIf(date6.getDateTime(true) != "2003-12-15T10:00+01:00");
	ATOMDateConstruct date7(0, 12, 15, 11, 0, 0, 0, 3, 30);
	complainIf(date7.getDateTime() != "0000-12-15T11:00:00+03:30");
	complainIf(date7.getDateTime(true) != "0000-12-15T11:00+03:30");
	ATOMDateConstruct date8(2003, 0, 20, 18, 0, 0, 0, 2, 40);
	complainIf(date8.getDateTime() != "2003-00-20T18:00:00+02:40");
	complainIf(date8.getDateTime(true) != "2003-00-20T18:00+02:40");
	ATOMDateConstruct date9(0, 0, 20, 23, 0, 0, 0, -1, -40);
	complainIf(date9.getDateTime() != "0000-00-20T23:00:00-01:40");
	complainIf(date9.getDateTime(true) != "0000-00-20T23:00-01:40");
	ATOMDateConstruct date0(0, 0, 0, 16, 0, 0, 0, -3, 0);
	complainIf(date0.getDateTime() != "0000-00-00T16:00:00-03:00");
	complainIf(date0.getDateTime(true) != "0000-00-00T16:00-03:00");
	return true;
}


bool ATOMDateTestTZ04() {
	// test minutes, hours > 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 1, 0, 0, 1, 20);
	complainIf(date1.getDateTime() != "2003-12-15T08:01:00+01:20");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:01+01:20");
	ATOMDateConstruct date2(0, 12, 15, 8, 5, 0, 0, -1, -20);
	complainIf(date2.getDateTime() != "0000-12-15T08:05:00-01:20");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:05-01:20");
	ATOMDateConstruct date3(2003, 0, 20, 8, 15, 0, 0, 1, 20);
	complainIf(date3.getDateTime() != "2003-00-20T08:15:00+01:20");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:15+01:20");
	ATOMDateConstruct date4(0, 0, 20, 8, 27, 0, 0, -2, 20);
	complainIf(date4.getDateTime() != "0000-00-20T08:27:00-01:40");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:27-01:40");
	ATOMDateConstruct date5(0, 0, 0, 8, 27, 0, 0, 1, 20);
	complainIf(date5.getDateTime() != "0000-00-00T08:27:00+01:20");
	complainIf(date5.getDateTime(true) != "0000-00-00T08:27+01:20");
	ATOMDateConstruct date6(2003, 12, 15, 0, 1, 0, 0, 1, 20);
	complainIf(date6.getDateTime() != "2003-12-15T00:01:00+01:20");
	complainIf(date6.getDateTime(true) != "2003-12-15T00:01+01:20");
	ATOMDateConstruct date7(0, 12, 15, 0, 5, 0, 0, 1, 20);
	complainIf(date7.getDateTime() != "0000-12-15T00:05:00+01:20");
	complainIf(date7.getDateTime(true) != "0000-12-15T00:05+01:20");
	ATOMDateConstruct date8(2003, 0, 20, 0, 15, 0, 0, 1, 20);
	complainIf(date8.getDateTime() != "2003-00-20T00:15:00+01:20");
	complainIf(date8.getDateTime(true) != "2003-00-20T00:15+01:20");
	ATOMDateConstruct date9(0, 0, 20, 0, 27, 0, 0, 1, 20);
	complainIf(date9.getDateTime() != "0000-00-20T00:27:00+01:20");
	complainIf(date9.getDateTime(true) != "0000-00-20T00:27+01:20");
	ATOMDateConstruct date0(0, 0, 0, 0, 27, 0, 0, 1, 20);
	complainIf(date0.getDateTime() != "0000-00-00T00:27:00+01:20");
	complainIf(date0.getDateTime(true) != "0000-00-00T00:27+01:20");
	return true;
}


bool ATOMDateTestTZ05() {
	// test seconds, minutes > 0, hours > 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 1, 13, 0, 3, 20);
	complainIf(date1.getDateTime() != "2003-12-15T08:01:13+03:20");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:01:13+03:20");
	ATOMDateConstruct date2(0, 12, 15, 8, 5, 1, 0, 3, 20);
	complainIf(date2.getDateTime() != "0000-12-15T08:05:01+03:20");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:05:01+03:20");
	ATOMDateConstruct date3(2003, 0, 20, 8, 15, 20, 0, 3, 20);
	complainIf(date3.getDateTime() != "2003-00-20T08:15:20+03:20");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:15:20+03:20");
	ATOMDateConstruct date4(0, 0, 20, 8, 27, 27, 0, 3, 20);
	complainIf(date4.getDateTime() != "0000-00-20T08:27:27+03:20");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:27:27+03:20");
	ATOMDateConstruct date5(2003, 12, 15, 0, 1, 17, 0, 3, 20);
	complainIf(date5.getDateTime() != "2003-12-15T00:01:17+03:20");
	complainIf(date5.getDateTime(true) != "2003-12-15T00:01:17+03:20");
	ATOMDateConstruct date6(0, 12, 15, 0, 5, 28, 0, 3, 20);
	complainIf(date6.getDateTime() != "0000-12-15T00:05:28+03:20");
	complainIf(date6.getDateTime(true) != "0000-12-15T00:05:28+03:20");
	ATOMDateConstruct date7(2003, 0, 20, 0, 15, 39, 0, 3, 20);
	complainIf(date7.getDateTime() != "2003-00-20T00:15:39+03:20");
	complainIf(date7.getDateTime(true) != "2003-00-20T00:15:39+03:20");
	ATOMDateConstruct date8(0, 0, 20, 0, 27, 41, 0, 3, 20);
	complainIf(date8.getDateTime() != "0000-00-20T00:27:41+03:20");
	complainIf(date8.getDateTime(true) != "0000-00-20T00:27:41+03:20");
	ATOMDateConstruct date9(2003, 12, 15, 8, 0, 13, 0, 3, 20);
	complainIf(date9.getDateTime() != "2003-12-15T08:00:13+03:20");
	complainIf(date9.getDateTime(true) != "2003-12-15T08:00:13+03:20");
	ATOMDateConstruct date0(0, 12, 15, 8, 0, 1, 0, 3, 20);
	complainIf(date0.getDateTime() != "0000-12-15T08:00:01+03:20");
	complainIf(date0.getDateTime(true) != "0000-12-15T08:00:01+03:20");
	ATOMDateConstruct dateA(2003, 0, 20, 8, 0, 20, 0, 3, 20);
	complainIf(dateA.getDateTime() != "2003-00-20T08:00:20+03:20");
	complainIf(dateA.getDateTime(true) != "2003-00-20T08:00:20+03:20");
	ATOMDateConstruct dateB(0, 0, 20, 8, 0, 27, 0, 3, 20);
	complainIf(dateB.getDateTime() != "0000-00-20T08:00:27+03:20");
	complainIf(dateB.getDateTime(true) != "0000-00-20T08:00:27+03:20");
	ATOMDateConstruct dateC(2003, 12, 15, 0, 0, 17, 0, 3, 20);
	complainIf(dateC.getDateTime() != "2003-12-15T00:00:17+03:20");
	complainIf(dateC.getDateTime(true) != "2003-12-15T00:00:17+03:20");
	ATOMDateConstruct dateD(0, 12, 15, 0, 0, 28, 0, 3, 20);
	complainIf(dateD.getDateTime() != "0000-12-15T00:00:28+03:20");
	complainIf(dateD.getDateTime(true) != "0000-12-15T00:00:28+03:20");
	ATOMDateConstruct dateE(2003, 0, 20, 0, 0, 39, 0, 3, 20);
	complainIf(dateE.getDateTime() != "2003-00-20T00:00:39+03:20");
	complainIf(dateE.getDateTime(true) != "2003-00-20T00:00:39+03:20");
	ATOMDateConstruct dateF(0, 0, 20, 0, 0, 41, 0, 3, 20);
	complainIf(dateF.getDateTime() != "0000-00-20T00:00:41+03:20");
	complainIf(dateF.getDateTime(true) != "0000-00-20T00:00:41+03:20");
	ATOMDateConstruct dateG(0, 0, 0, 0, 0, 9, 0, 3, 20);
	complainIf(dateG.getDateTime() != "0000-00-00T00:00:09+03:20");
	complainIf(dateG.getDateTime(true) != "0000-00-00T00:00:09+03:20");
	return true;
}

bool ATOMDateTestTZ06() {
	// test sfraction, any seconds, minutes > 0, hours > 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 1, 13, 0.1234, 8, 45);
	complainIf(date1.getDateTime() != "2003-12-15T08:01:13.12+08:45");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:01:13.12+08:45");
	ATOMDateConstruct date2(0, 12, 15, 8, 5, 1, 0.1234, 8, 45);
	complainIf(date2.getDateTime() != "0000-12-15T08:05:01.12+08:45");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:05:01.12+08:45");
	ATOMDateConstruct date3(2003, 0, 20, 8, 15, 20, 0.1234, 8, 45);
	complainIf(date3.getDateTime() != "2003-00-20T08:15:20.12+08:45");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:15:20.12+08:45");
	ATOMDateConstruct date4(0, 0, 20, 8, 27, 27, 0.1234, 8, 45);
	complainIf(date4.getDateTime() != "0000-00-20T08:27:27.12+08:45");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:27:27.12+08:45");
	ATOMDateConstruct date5(0, 0, 0, 23, 11, 0, 0.1234, 8, 45);
	complainIf(date5.getDateTime() != "0000-00-00T23:11:00.12+08:45");
	complainIf(date5.getDateTime(true) != "0000-00-00T23:11:00.12+08:45");

	// test sfraction, any seconds, minutes > 0, hours = 0, any date
	ATOMDateConstruct date6(2003, 12, 15, 0, 1, 17, 0.9876, 8, 45);
	complainIf(date6.getDateTime() != "2003-12-15T00:01:17.99+08:45");
	complainIf(date6.getDateTime(true) != "2003-12-15T00:01:17.99+08:45");
	ATOMDateConstruct date7(0, 12, 15, 0, 5, 28, 0.9876, 8, 45);
	complainIf(date7.getDateTime() != "0000-12-15T00:05:28.99+08:45");
	complainIf(date7.getDateTime(true) != "0000-12-15T00:05:28.99+08:45");
	ATOMDateConstruct date8(2003, 0, 20, 0, 15, 39, 0.9876, 8, 45);
	complainIf(date8.getDateTime() != "2003-00-20T00:15:39.99+08:45");
	complainIf(date8.getDateTime(true) != "2003-00-20T00:15:39.99+08:45");
	ATOMDateConstruct date9(0, 0, 20, 0, 27, 41, 0.9876, 8, 45);
	complainIf(date9.getDateTime() != "0000-00-20T00:27:41.99+08:45");
	complainIf(date9.getDateTime(true) != "0000-00-20T00:27:41.99+08:45");
	ATOMDateConstruct date0(0, 0, 0, 0, 11, 0, 0.9876, 8, 45);
	complainIf(date0.getDateTime() != "0000-00-00T00:11:00.99+08:45");
	complainIf(date0.getDateTime(true) != "0000-00-00T00:11:00.99+08:45");
	return true;
}

bool ATOMDateTestTZ07() {
	// test sfraction, any seconds, minutes = 0, hours > 0, any date
	ATOMDateConstruct date1(2003, 12, 15, 8, 0, 13, 0.2926, 8, 45);
	complainIf(date1.getDateTime() != "2003-12-15T08:00:13.29+08:45");
	complainIf(date1.getDateTime(true) != "2003-12-15T08:00:13.29+08:45");
	ATOMDateConstruct date2(0, 12, 15, 8, 0, 1, 0.2926, 8, 45);
	complainIf(date2.getDateTime() != "0000-12-15T08:00:01.29+08:45");
	complainIf(date2.getDateTime(true) != "0000-12-15T08:00:01.29+08:45");
	ATOMDateConstruct date3(2003, 0, 20, 8, 0, 20, 0.2926, 8, 45);
	complainIf(date3.getDateTime() != "2003-00-20T08:00:20.29+08:45");
	complainIf(date3.getDateTime(true) != "2003-00-20T08:00:20.29+08:45");
	ATOMDateConstruct date4(0, 0, 20, 8, 0, 27, 0.2926, 8, 45);
	complainIf(date4.getDateTime() != "0000-00-20T08:00:27.29+08:45");
	complainIf(date4.getDateTime(true) != "0000-00-20T08:00:27.29+08:45");
	ATOMDateConstruct date5(0, 0, 0, 23, 0, 0, 0.2926, 8, 45);
	complainIf(date5.getDateTime() != "0000-00-00T23:00:00.29+08:45");
	complainIf(date5.getDateTime(true) != "0000-00-00T23:00:00.29+08:45");

	// test sfraction, any seconds, minutes = 0, hours = 0, any date
	ATOMDateConstruct date6(2003, 12, 15, 0, 0, 17, 0.4895, 8, 45);
	complainIf(date6.getDateTime() != "2003-12-15T00:00:17.49+08:45");
	complainIf(date6.getDateTime(true) != "2003-12-15T00:00:17.49+08:45");
	ATOMDateConstruct date7(0, 12, 15, 0, 0, 28, 0.4895, 8, 45);
	complainIf(date7.getDateTime() != "0000-12-15T00:00:28.49+08:45");
	complainIf(date7.getDateTime(true) != "0000-12-15T00:00:28.49+08:45");
	ATOMDateConstruct date8(2003, 0, 20, 0, 0, 39, 0.4895, 8, 45);
	complainIf(date8.getDateTime() != "2003-00-20T00:00:39.49+08:45");
	complainIf(date8.getDateTime(true) != "2003-00-20T00:00:39.49+08:45");
	ATOMDateConstruct date9(0, 0, 20, 0, 0, 41, 0.4895, 8, 45);
	complainIf(date9.getDateTime() != "0000-00-20T00:00:41.49+08:45");
	complainIf(date9.getDateTime(true) != "0000-00-20T00:00:41.49+08:45");
	ATOMDateConstruct date0(0, 0, 20, 0, 0, 0, 0.4895, 8, 45);
	complainIf(date0.getDateTime() != "0000-00-20T00:00:00.49+08:45");
	complainIf(date0.getDateTime(true) != "0000-00-20T00:00:00.49+08:45");
	ATOMDateConstruct dateA(0, 0, 0, 0, 0, 0, 0.4895, 8, 45);
	complainIf(dateA.getDateTime() != "0000-00-00T00:00:00.49+08:45");
	complainIf(dateA.getDateTime(true) != "0000-00-00T00:00:00.49+08:45");
	return true;
}


BEGIN_TESTCASE(atomdatetztestcase)
	TEST_ELEMENT(ATOMDateTestTZ01)
	TEST_ELEMENT(ATOMDateTestTZ02)
	TEST_ELEMENT(ATOMDateTestTZ03)
	TEST_ELEMENT(ATOMDateTestTZ04)
	TEST_ELEMENT(ATOMDateTestTZ05)
	TEST_ELEMENT(ATOMDateTestTZ06)
	TEST_ELEMENT(ATOMDateTestTZ07)
END_TESTCASE;

