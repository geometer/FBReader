#include <QtGui/QKeyEvent>

#include "VolumeKeysCapturer.h"

#include <e32keys.h>

VolumeKeysCapturer::VolumeKeysCapturer(QObject *receiver): myReceiver(receiver) {
    QT_TRAP_THROWING(iInterfaceSelector = CRemConInterfaceSelector::NewL());
    QT_TRAP_THROWING(iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this));
    iInterfaceSelector->OpenTargetL();
}

VolumeKeysCapturer::~VolumeKeysCapturer() {
    delete iInterfaceSelector;
    delete iCoreTarget;
}

void VolumeKeysCapturer::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                        TRemConCoreApiButtonAction aButtonAct)
{
    switch( aOperationId )
    {
    case ERemConCoreApiVolumeUp:
        qApp->postEvent( myReceiver, new QKeyEvent(QEvent::KeyPress, EStdKeyIncVolume , 0, "VolumeUpButton"));
        break;
    case ERemConCoreApiVolumeDown:        
        qApp->postEvent( myReceiver, new QKeyEvent(QEvent::KeyPress, EStdKeyDecVolume , 0, "VolumeDownButton"));
        break;
    default:
        break;
    }
}
