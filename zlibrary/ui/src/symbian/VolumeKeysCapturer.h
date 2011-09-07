#ifndef __VOLUMEKEYSCAPTURER_H__
#define __VOLUMEKEYSCAPTURER_H__

#ifdef SYMBIAN
#include <remconcoreapitargetobserver.h>    // link against RemConCoreApi.lib
#include <remconcoreapitarget.h>            // and
#include <remconinterfaceselector.h>        // RemConInterfaceBase.lib

#include <QCoreApplication>

class VolumeKeysCapturer : public MRemConCoreApiTargetObserver {
public:
    VolumeKeysCapturer(QObject *receiver);
    ~VolumeKeysCapturer();
    virtual void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                            TRemConCoreApiButtonAction aButtonAct);
private:
    CRemConInterfaceSelector* iInterfaceSelector;
    CRemConCoreApiTarget*     iCoreTarget;
    QObject *myReceiver;
};
#endif


#endif /* __VOLUMEKEYSCAPTURER_H__ */

