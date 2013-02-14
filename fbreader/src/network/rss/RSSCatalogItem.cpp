#include "RSSCatalogItem.h"

#include <ZLNetworkRequest.h>
#include <ZLNetworkManager.h>

#include "../opds/OPDSLink.h"
#include "../opds/OPDSXMLParser.h"
#include "../opds/NetworkOPDSFeedReader.h"

#include "RSSXMLParser.h"
#include "RSSLink.h"

#include "../NetworkOperationData.h"

#include <iostream> //udmv

RSSCatalogItem::RSSCatalogItem(const OPDSLink &link, const std::string &title, const std::string &summary,
                               const UrlInfoCollection &urlByType, AccessibilityType accessibility, int flags)
                                : NetworkCatalogItem(link, title, summary, urlByType, accessibility, flags), myLoadingState(Link) {
}

class RSSCatalogItemRunnable : public ZLNetworkRequest::Listener {
public:
    RSSCatalogItemRunnable(shared_ptr<ZLNetworkRequest> request, NetworkItem::List &children, NetworkOperationData &data, shared_ptr<ZLNetworkRequest::Listener> listener) :
        myChildren(children), myLoadingData(data), myListener(listener) {
        request->setListener(this);
        ZLNetworkManager::Instance().performAsync(request);
    }
    void finished(const std::string &error) {
        myChildren.insert(myChildren.end(), myLoadingData.Items.begin(), myLoadingData.Items.end());
        myListener->finished(error);
    }
    void setUIStatus(bool enabled) {
        myListener->setUIStatus(enabled); //to hide refreshing while authentication dialog
    }

private:
    NetworkItem::List &myChildren;
    NetworkOperationData &myLoadingData;
    shared_ptr<ZLNetworkRequest::Listener> myListener;
};

std::string RSSCatalogItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
    myLoadingState.clear();
    std::cout << "RSSCatalogItem::loadChildren" << std::endl;
    shared_ptr<ZLNetworkRequest> request = ((RSSLink&)Link).createNetworkRequest(getCatalogUrl(), myLoadingState);
    new RSSCatalogItemRunnable(request, children, myLoadingState, listener);
    return std::string();
}

bool RSSCatalogItem::supportsResumeLoading() {
    return true;
}

std::string RSSCatalogItem::resumeLoading(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
    shared_ptr<ZLNetworkRequest> request = myLoadingState.resume();
    if (request.isNull()) {
        listener->finished();
        return std::string();
    }
    new RSSCatalogItemRunnable(request, children, myLoadingState, listener);
    return std::string();
}
