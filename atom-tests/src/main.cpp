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


DECLARE_TESTCASE(atomdatetestcase);
DECLARE_TESTCASE(atomdatetztestcase);
DECLARE_TESTCASE(atomdateparsetestcase);

TestElement *testsarray[] = {
	atomdatetestcase,
	atomdatetztestcase,
	atomdateparsetestcase,
	0
};

static const std::string TESTSINIT_MASK = "*TestsInit";


bool compareMask(const std::string &str, const std::string &mask) {
	unsigned pos = mask.find_first_of(".*");
	if (pos == std::string::npos) {
		return str.compare(mask) == 0;
	}
	if (pos != 0 && str.substr(0, pos).compare( mask.substr(0, pos) ) != 0) {
		return false;
	}
	const char mch = mask[pos];
	const std::string newmask = mask.substr(pos + 1);
	if (mch == '.') {
		if (str.length() <= pos) {
			return false;
		}
		return compareMask(str.substr(pos + 1), newmask);
	} else { // mch == '*'
		if (newmask.length() == 0) {
			return true;
		}
		for (unsigned i = pos; i < str.size(); ++i) {
			const bool res = compareMask(str.substr(i), newmask);
			if (res) {
				return true;
			}
		}
		return false;
	}
}


bool startFun(TestFun fun, std::string &message) {
	bool res = true;
	try {
		res = fun();
	} catch (const char *ex) {
		message = ex;
		res = false;
	} catch (std::string &ex) {
		message = ex;
		res = false;
	}
	return res;
}


bool runTest(TestElement *test) {
	std::cout << "RUNNING: " << test->name << " ... ";
	std::cout.flush();

	std::string msg;
	bool res = startFun(test->fun, msg);

	if (res) {
		std::cout << "OK" << std::endl;
	} else {
		std::cout << "FAILED";
		if (msg.length() > 0) {
			std::cout << ": " << msg;
		}
		std::cout << std::endl;
		return false;
	}
	return true;
}

bool runInit(TestElement *init) {
	std::cout << "INITIALIZATION: " << init->name << " ... ";
	std::cout.flush();

	std::string msg;
	bool res = startFun(init->fun, msg);

	if (res) {
		std::cout << "DONE" << std::endl;
	} else {
		std::cout << "FAILED";
		if (msg.length() > 0) {
			std::cout << ": " << msg;
		}
		std::cout << std::endl;
		return false;
	}
	return true;
}


int main(int argc, char **argv) {

	std::vector<std::string> args;
	args.assign(argv + 1, argv + argc);

	TestElement **p = testsarray;
	
	bool alltests = true;
	if (argc > 1) {
		alltests = false;
	}
	
	int status = 0;
	while (*p != 0) {
		TestElement *el = *p++;

		bool initres = true;
		TestElement *initel = el;
		while (initel->fun != 0) {
			if (compareMask(initel->name, TESTSINIT_MASK)) {
				initres = runInit(initel);
				break;
			}
			++initel;
		}
		
		if (!initres) {
			continue;
		}

		while (el->fun != 0) {
			TestElement *curr = el++;

			if (curr == initel) {
				continue;
			}

			bool canrun = alltests;
			if (!alltests) {
				std::string testname(curr->name);
				for (unsigned i = 0; i < args.size(); ++i) {
					if (compareMask(testname, args[i])) {
						canrun = true;
						break;
					}
				}
			}

			if (!canrun) {
				continue;
			}

			if (!runTest(curr)) {
				status = 1;
			}
		}
	}

	return status;
}

