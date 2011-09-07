#include "FBReader.h"
#include "../options/FBOptions.h"
#include "SwitchProfileAction.h"

SwitchProfileAction::SwitchProfileAction(const std::string& profileName): myProfileName(profileName) { }

bool SwitchProfileAction::isVisible() const {
	return !(myProfileName == FBOptions::Instance().getColorProfileName());
}

void SwitchProfileAction::run() {
	FBOptions::Instance().setColorProfileName(myProfileName);
	FBReader::Instance().refreshWindow();
}
