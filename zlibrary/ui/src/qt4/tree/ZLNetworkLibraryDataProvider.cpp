#include "ZLNetworkLibraryDataProvider.h"

ZLNetworkLibraryDataProvider::ZLNetworkLibraryDataProvider(){
}

ZLNetworkRequest::Listener *ZLNetworkLibraryDataProvider::getData(ZLTreeNode *node, bool moreMode){
    return new ChildrenRequestListener(node, moreMode, this);
}

ZLNetworkLibraryDataProvider::ChildrenRequestListener::ChildrenRequestListener(ZLTreeNode *node, bool moreMode, ZLNetworkLibraryDataProvider *dataProvider) :
    myNode(node), myMoreMode(moreMode), myDataProvider(dataProvider) {
}

void ZLNetworkLibraryDataProvider::ChildrenRequestListener::finished(const std::string &error) {
    if (!error.empty()) {
        //TODO show error message?
    }
    if(!myMoreMode){
        myDataProvider->getParent()->onChildrenLoaded(myNode, true, error.empty());
    }else{
        myDataProvider->getParent()->onMoreChildrenLoaded(error.empty());
    }
}

