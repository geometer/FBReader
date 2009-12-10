/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __TESTS_H__
#define __TESTS_H__


#include <iostream>
#include <vector>
#include <string>

/*
 * TestFun returns: 
 *     true  - if test passed;
 *     false - if test failed.
 */
typedef bool (*TestFun)();

struct TestElement {
	TestFun fun;
	const char *name;
};


#define DECLARE_TESTCASE(name) \
extern TestElement name[]
	
#define BEGIN_TESTCASE(name) \
TestElement name[] = {

#define TEST_ELEMENT(name) \
	{name, #name},

#define END_TESTCASE \
	{0, 0} \
}

#define __STRINGIFY(X) #X
#define __TOSTRING(X) __STRINGIFY(X)
#define complainIf(exp)     if (exp) throw __FILE__ ":" __TOSTRING(__LINE__) ": " #exp

#endif /* __TESTS_H__ */

