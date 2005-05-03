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
