#ifndef __SWITCHPROFILEACTION_H__
#define __SWITCHPROFILEACTION_H__

#include <ZLApplication.h>

class SwitchProfileAction : public ZLApplication::Action {
public:
	SwitchProfileAction(const std::string& profileName);
	bool isVisible() const;
	void run();
private:
	std::string myProfileName;

};

#endif /* __SWITCHPROFILEACTION_H__ */
