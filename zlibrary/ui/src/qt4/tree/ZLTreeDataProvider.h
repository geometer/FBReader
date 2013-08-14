#ifndef ZLTREEDATAPROVIDER_H
#define ZLTREEDATAPROVIDER_H

#include <ZLTreeNode.h>
#include <ZLNetworkRequest.h>
#include "../dialogs/ZLQtTreeDialog.h"

class ZLQtTreeDialog;

class ZLTreeDataProvider{
public:
    ZLTreeDataProvider();
    virtual ZLNetworkRequest::Listener *getData(ZLTreeNode *node, bool moreMode) = 0;
    void setParent(ZLQtTreeDialog *dialog);

protected:
    ZLQtTreeDialog *myDialog;
};

#endif // ZLTREEDATAPROVIDER_H
