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

    //后台执行（当前貌似只执行鼠标单击和按下滑动）
    static void backstageDoEventItem(HWND wigId, const EventItem &eventItem);

    //前台执行
    static void frontDoEventItem(const EventItem &eventItem);

private:
#ifdef WIN32
    static void backstageMouseKeyToDWORD(Qt::MouseButton key, bool pressed, UINT &param1, WPARAM &param2);
    static void frontMouseKeyToDWORD(Qt::MouseButton key, bool pressed, DWORD &param);

    static BYTE keyToBYTE(Qt::Key key);
#endif

protected:
    EventManager(){}
};

#endif // EVENTMANAGER_H
