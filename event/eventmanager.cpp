#include "eventmanager.h"

#include <QPoint>
#include <QCursor>
#include <QThread>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QWidget>
#include <QProcess>
#include <math.h>

void EventManager::screenShort(HWND wigId, const QString &fileName)
{
    QScreen *screen = QApplication::primaryScreen();
    screen->grabWindow(WId(wigId)).save(fileName);
}

void EventManager::backstageDoEventItem(HWND wigId, const EventItem &eventItem)
{
    switch(eventItem.eventType)
    {
    case ET_MOUSE_PRESSED:
    {
        UINT param1; WPARAM param2;
        EventManager::backstageMouseKeyToDWORD(eventItem.mouseKey.mouseButton, true, param1, param2);
        LPRECT lpRect = new RECT;
        if(GetWindowRect(wigId, lpRect))
        {
            int x = static_cast<int>(round((lpRect->right - lpRect->left)*eventItem.mouseKey.xValue));
            int y = static_cast<int>(round((lpRect->bottom - lpRect->top)*eventItem.mouseKey.yValue));
            SendMessageA(wigId, param1, param2, MAKELPARAM(x, y));
        }
    }
        break;
    case ET_MOUSE_RELEASED:
    {
        UINT param1; WPARAM param2;
        EventManager::backstageMouseKeyToDWORD(eventItem.mouseKey.mouseButton, false, param1, param2);
        LPRECT lpRect = new RECT;
        if(GetWindowRect(wigId, lpRect))
        {
            int x = static_cast<int>(round((lpRect->right - lpRect->left)*eventItem.mouseKey.xValue));
            int y = static_cast<int>(round((lpRect->bottom - lpRect->top)*eventItem.mouseKey.yValue));
            SendMessageA(wigId, param1, param2, MAKELPARAM(x, y));
            SendMessageA(wigId, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
        }
        delete lpRect;
    }
        break;
    case ET_MOUSE_CLICKED:
    {
        UINT param1, param3; WPARAM param2;
        EventManager::backstageMouseKeyToDWORD(eventItem.mouseKey.mouseButton, true, param1, param2);
        EventManager::backstageMouseKeyToDWORD(eventItem.mouseKey.mouseButton, false, param3, param2);
        LPRECT lpRect = new RECT;
        if(GetWindowRect(wigId, lpRect))
        {
            int x = static_cast<int>(round((lpRect->right - lpRect->left)*eventItem.mouseKey.xValue));
            int y = static_cast<int>(round((lpRect->bottom - lpRect->top)*eventItem.mouseKey.yValue));
            SendMessageA(wigId, param1|param3, param2, MAKELPARAM(x, y));
            SendMessageA(wigId, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
        }
    }
        break;
    case ET_MOUSE_MOVE:
    {
        LPRECT lpRect = new RECT;
        if(GetWindowRect(wigId, lpRect))
        {
            int x = static_cast<int>(round((lpRect->right - lpRect->left)*eventItem.mouseMove.xValue));
            int y = static_cast<int>(round((lpRect->bottom - lpRect->top)*eventItem.mouseMove.yValue));
            SendMessageA(wigId, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(x, y));
        }
        delete lpRect;
    }
        break;
    case ET_MOUSE_WHEEL:
    {
        LPRECT lpRect = new RECT;
        if(GetWindowRect(wigId, lpRect))
        {
            int x = static_cast<int>(round((lpRect->right - lpRect->left)*eventItem.mouseWheel.xValue));
            int y = static_cast<int>(round((lpRect->bottom - lpRect->top)*eventItem.mouseWheel.yValue));
            ::SendMessageA(wigId, WM_MOUSEWHEEL, MAKEWPARAM(0, eventItem.mouseWheel.wheelValue), MAKELPARAM(x, y));
        }
        delete  lpRect;
    }
        break;
    case ET_KEY_PRESSED:
    {
        SendMessageA(wigId, WM_SYSKEYDOWN,
                     EventManager::keyToBYTE(eventItem.key),
                     0);
    }
        break;
    case ET_KEY_RELEASED:
    {
        SendMessageA(wigId, WM_SYSKEYUP,
                     EventManager::keyToBYTE(eventItem.key),
                     0);
    }
        break;
    case ET_KEY_CLICKED:
    {
        SendMessageA(wigId, WM_SYSKEYDOWN|WM_SYSKEYUP,
                     EventManager::keyToBYTE(eventItem.key),
                     0);
    }
        break;
    case ET_WIDGET_SHORT:
    {
        EventManager::screenShort(wigId
                                  , QString(eventItem.widgetShort.baseName)
                                         + "."
                                         + QString(eventItem.widgetShort.suffix));
    }
        break;
    case ET_SLEEP:
        if(eventItem.sleepTime.unitIsMs)
        {
            QThread::msleep(static_cast<unsigned long>(eventItem.sleepTime.sleepTime));
        }
        else
        {
            QThread::sleep(static_cast<unsigned long>(eventItem.sleepTime.sleepTime));
        }
        break;
    case ET_OPEN_SOFTWARE:
    {
        QProcess process;
        process.start(eventItem.openSoftware.path);
        process.waitForStarted();
    }
        break;
    default:
        break;
    }
}

void EventManager::frontDoEventItem(const EventItem &eventItem)
{
    switch(eventItem.eventType)
    {
    case ET_MOUSE_PRESSED:
    {
        DWORD param;
        EventManager::frontMouseKeyToDWORD(eventItem.mouseKey.mouseButton, true, param);
        mouse_event(param, static_cast<DWORD>(eventItem.mouseKey.xValue)
                    , static_cast<DWORD>(eventItem.mouseKey.yValue), 0, 0);
    }
        break;
    case ET_MOUSE_RELEASED:
    {
        DWORD param;
        EventManager::frontMouseKeyToDWORD(eventItem.mouseKey.mouseButton, false, param);
        mouse_event(param, static_cast<DWORD>(eventItem.mouseKey.xValue)
                    , static_cast<DWORD>(eventItem.mouseKey.yValue), 0, 0);
    }
        break;
    case ET_MOUSE_CLICKED:
    {
        DWORD param1, param2;
        EventManager::frontMouseKeyToDWORD(eventItem.mouseKey.mouseButton, true, param1);
        EventManager::frontMouseKeyToDWORD(eventItem.mouseKey.mouseButton, false, param2);
        mouse_event(param1|param2, static_cast<DWORD>(eventItem.mouseKey.xValue)
                    , static_cast<DWORD>(eventItem.mouseKey.yValue), 0, 0);
    }
        break;
    case ET_MOUSE_MOVE:
    {
        mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE
                    , static_cast<DWORD>(eventItem.mouseMove.xValue)
                    , static_cast<DWORD>(eventItem.mouseMove.yValue)
                    , 0, 0);
    }
        break;
    case ET_MOUSE_WHEEL:
    {
        mouse_event(MOUSEEVENTF_WHEEL, static_cast<DWORD>(eventItem.mouseWheel.xValue)
                    , static_cast<DWORD>(eventItem.mouseWheel.yValue)
                    , static_cast<DWORD>(eventItem.mouseWheel.wheelValue), 0);
    }
        break;
    case ET_KEY_PRESSED:
    {
        keybd_event(EventManager::keyToBYTE(eventItem.key), 0, 0, 0);
    }
        break;
    case ET_KEY_RELEASED:
    {
        keybd_event(EventManager::keyToBYTE(eventItem.key), 0, KEYEVENTF_KEYUP, 0);
    }
        break;
    case ET_KEY_CLICKED:
    {
        keybd_event(EventManager::keyToBYTE(eventItem.key), 0, 0, 0);
        keybd_event(EventManager::keyToBYTE(eventItem.key), 0, KEYEVENTF_KEYUP, 0);
    }
        break;
    case ET_SLEEP:
        if(eventItem.sleepTime.unitIsMs)
        {
            QThread::msleep(static_cast<unsigned long>(eventItem.sleepTime.sleepTime));
        }
        else
        {
            QThread::sleep(static_cast<unsigned long>(eventItem.sleepTime.sleepTime));
        }
        break;
    case ET_OPEN_SOFTWARE:
    {
        QProcess process;
        process.start(eventItem.openSoftware.path);
        process.waitForStarted();
    }
        break;
    default:
        break;
    }
}

#ifdef WIN32
void EventManager::backstageMouseKeyToDWORD(Qt::MouseButton key, bool pressed, UINT &param1, WPARAM &param2)
{
    switch(key)
    {
    case Qt::LeftButton:
    {
        param2 = MK_LBUTTON;
        if(pressed)
        {
            param1 = WM_LBUTTONDOWN;
        }
        else
        {
            param1 = WM_LBUTTONUP;
        }
    }
        break;
    case Qt::RightButton:
    {
        param2 = MK_RBUTTON;
        if(pressed)
        {
            param1 = WM_RBUTTONDOWN;
        }
        else
        {
            param1 = WM_RBUTTONUP;
        }
    }
        break;
    case Qt::MidButton:
    {
        param2 = MK_MBUTTON;
        if(pressed)
        {
            param1 = WM_MBUTTONDOWN;
        }
        else
        {
            param1 = WM_MBUTTONUP;
        }
        break;
    }
    default:
        break;
    }
}

void EventManager::frontMouseKeyToDWORD(Qt::MouseButton key, bool pressed, DWORD &param)
{
    switch(key)
    {
    case Qt::LeftButton:
    {
        if(pressed)
        {
            param = MOUSEEVENTF_LEFTDOWN;
        }
        else
        {
            param = MOUSEEVENTF_LEFTUP;
        }
    }
        break;
    case Qt::RightButton:
    {
        if(pressed)
        {
            param = MOUSEEVENTF_RIGHTDOWN;
        }
        else
        {
            param = MOUSEEVENTF_RIGHTUP;
        }
    }
        break;
    case Qt::MidButton:
    {
        if(pressed)
        {
            param = MOUSEEVENTF_MIDDLEDOWN;
        }
        else
        {
            param = MOUSEEVENTF_MIDDLEUP;
        }
        break;
    }
    default:
        break;
    }
}
#endif

#ifdef WIN32
BYTE EventManager::keyToBYTE(Qt::Key key)
{
    if(key >= Qt::Key_0 && key <= Qt::Key_9)
    {
        return static_cast<BYTE>(key);
    }
    else if(key >= Qt::Key_A && key <= Qt::Key_Z)
    {
        return static_cast<BYTE>(key);
    }
    else if(key >= Qt::Key_F1 && key <= Qt::Key_F24)
    {
        uchar off = static_cast<BYTE>(key - Qt::Key_F1);
        return VK_F1 + off;
    }
    else if(key == Qt::Key_Tab)
    {
        return VK_TAB;
    }
    else if(key == Qt::Key_Backspace)
    {
        return VK_BACK;
    }
    else if(key == Qt::Key_Return)
    {
        return VK_RETURN;
    }
    else if(key >= Qt::Key_Left && key <= Qt::Key_Down)
    {
        uchar off = static_cast<BYTE>(key - Qt::Key_Left);
        return VK_LEFT + off;
    }
    else if(key == Qt::Key_Shift)
    {
        return VK_SHIFT;
    }
    else if(key == Qt::Key_Control)
    {
        return VK_CONTROL;
    }
    else if(key == Qt::Key_Alt)
    {
        return VK_MENU;
    }
    else if(key == Qt::Key_Meta)
    {
        return VK_LWIN;
    }
    else if(key == Qt::Key_Insert)
    {
        return VK_INSERT;
    }
    else if(key == Qt::Key_Delete)
    {
        return VK_DELETE;
    }
    else if(key == Qt::Key_Home)
    {
        return VK_HOME;
    }
    else if(key == Qt::Key_End)
    {
        return VK_END;
    }
    else if(key == Qt::Key_PageUp)
    {
        return VK_PRIOR;
    }
    else if(key == Qt::Key_Down)
    {
        return VK_NEXT;
    }
    else if(key == Qt::Key_CapsLock)
    {
        return VK_CAPITAL;
    }
    else if(key == Qt::Key_NumLock)
    {
        return VK_NUMLOCK;
    }
    else if(key == Qt::Key_Space)
    {
        return VK_SPACE;
    }
    else if(key == Qt::Key_Escape)
    {
        return VK_ESCAPE;
    }
    return static_cast<BYTE>(key);
}
#endif
