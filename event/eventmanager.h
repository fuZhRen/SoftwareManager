#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <QObject>

#ifdef WIN32
#include <windows.h>
#endif

#include "EventStruct.h"

class EventManager
{
public:
    static void screenShort(HWND wigId, const QString &fileName);

    static void backstageDoEventItem(HWND wigId, const EventItem &eventItem);

private:
#ifdef WIN32
    static void mouseKeyToDWORD(Qt::MouseButton key, bool pressed, UINT &param1, WPARAM &param2);
    static BYTE keyToBYTE(Qt::Key key);
#endif

protected:
    EventManager(){}
};

#endif // EVENTMANAGER_H
