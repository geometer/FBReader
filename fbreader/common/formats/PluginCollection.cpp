#include "FormatPlugin.h"

#include "fb2/FB2Plugin.h"
#include "docbook/DocBookPlugin.h"
#include "html/HtmlPlugin.h"
#include "txt/TxtPlugin.h"

PluginCollection *PluginCollection::ourInstance = 0;

PluginCollection &PluginCollection::instance() {
	if (ourInstance == 0) {
		ourInstance = new PluginCollection();
		ourInstance->myPlugins.push_back(new FB2Plugin());
		ourInstance->myPlugins.push_back(new DocBookPlugin());
		ourInstance->myPlugins.push_back(new HtmlPlugin());
		ourInstance->myPlugins.push_back(new TxtPlugin());
	}
	return *ourInstance;
}

FormatPlugin *PluginCollection::plugin(const std::string &fileName, bool strong) {
	for (std::vector<FormatPlugin*>::iterator it = myPlugins.begin(); it != myPlugins.end(); it++) {
		if ((!strong || (*it)->containsMetaInfo()) && (*it)->acceptsFile(fileName)) {
			return *it;
		}
	}
	return 0;
}
