/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <abstract/ZLInputStream.h>

#include "XMLRootTester.h"

XMLRootTester::XMLRootTester(const std::string &root) : myRoot(root) {
}

bool XMLRootTester::test(const ZLFile &file) {
  shared_ptr<ZLInputStream> stream = file.inputStream();
  if (stream.isNull() || !stream->open()) {
    return false;
  }
  myResult = false;
  readDocument(stream);
  return myResult;
}

int XMLRootTester::tag(const char *name) {
  return (myRoot == name) ? 1 : 0;
}

void XMLRootTester::startElementHandler(int tag, const char**) {
  myResult = tag == 1;
	interrupt();
}

void XMLRootTester::endElementHandler(int) {
}

void XMLRootTester::characterDataHandler(const char*, int) {
}
