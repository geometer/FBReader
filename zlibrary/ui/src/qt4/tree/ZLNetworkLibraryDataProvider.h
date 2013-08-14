#ifndef ZLNETWORKLIBRARYDATAPROVIDER_H
#define ZLNETWORKLIBRARYDATAPROVIDER_H

#include "ZLTreeDataProvider.h"
#include <iostream> //udmv

class ZLNetworkLibraryDataProvider : public ZLTreeDataProvider
{
public:
    ZLNetworkLibraryDataProvider();
    ZLNetworkRequest::Listener *getData(ZLTreeNode *node, bool moreMode);

private:
    ZLQtTreeDialog *getParent() {return myDialog;}
    class ChildrenRequestListener : public ZLNetworkRequest::Listener {
        public:
            ChildrenRequestListener(ZLTreeNode *node, bool moreMode, ZLNetworkLibraryDataProvider *dataProvider);
            void finished(const std::string &error);

        private:
            ZLTreeNode *myNode;
            bool myMoreMode;
            ZLNetworkLibraryDataProvider *myDataProvider;
    };

friend class ChildrenRequestListener;
};


#endif // ZLNETWORKLIBRARYDATAPROVIDER_H
