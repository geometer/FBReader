#ifndef RSSCATALOGITEM_H
#define RSSCATALOGITEM_H

#include <ZLExecutionUtil.h>

#include "../NetworkItems.h"
#include "../NetworkOperationData.h"

class OPDSLink;
class RSSLink;

class RSSCatalogItem : public NetworkCatalogItem{
public:
    RSSCatalogItem(
            const OPDSLink &link,
            const std::string &title,
            const std::string &summary,
            const UrlInfoCollection &urlByType,
            AccessibilityType accessibility = ALWAYS,
            int flags = FLAGS_DEFAULT
     );

public:
    std::string loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener = 0);
    bool supportsResumeLoading();
    std::string resumeLoading(List &children, shared_ptr<ZLNetworkRequest::Listener> listener = 0);

private:
    NetworkOperationData myLoadingState;
};

#endif // RSSCATALOGITEM_H
