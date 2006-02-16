/*
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

#include "PalmOptions.h"

void PalmOptions::createInstance(const char *name) {
	ourInstance = new PalmOptions(name);
}
	
// TODO: write real implementations

PalmOptions::PalmOptions(const char* /*name*/) {
}

PalmOptions::~PalmOptions() {
}

void PalmOptions::setGroup(const std::string &/*name*/) {
}

void PalmOptions::clearGroup() {
}
	
void PalmOptions::unsetValue(const std::string &/*name*/) {
}

bool PalmOptions::booleanValue(const std::string &/*name*/, bool defaultValue) {
	return defaultValue;
}

void PalmOptions::setValue(const std::string &/*name*/, bool /*value*/) {
}

long PalmOptions::integerValue(const std::string &/*name*/, long defaultValue) {
	return defaultValue;
}

void PalmOptions::setValue(const std::string &/*name*/, long /*value*/) {
}

double PalmOptions::doubleValue(const std::string &/*name*/, double defaultValue) {
	return defaultValue;
}

void PalmOptions::setValue(const std::string &/*name*/, double /*value*/) {
}

std::string PalmOptions::stringValue(const std::string &/*name*/, const std::string &defaultValue) {
	return defaultValue;
}

void PalmOptions::setValue(const std::string &/*name*/, const std::string &/*value*/) {
}
