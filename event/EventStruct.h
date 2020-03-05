#ifndef EVENTSTRUCT_H
#define EVENTSTRUCT_H

#include <QObject>
#include <QList>
#include <QDataStream>

enum EventType
{
    ET_NONE,
    ET_MOUSE_PRESSED,
    ET_MOUSE_RELEASED,
    ET_MOUSE_CLICKED,
    ET_MOUSE_MOVE,
    ET_MOUSE_WHEEL,
    ET_KEY_PRESSED,
    ET_KEY_RELEASED,
    ET_KEY_CLICKED,
    ET_WIDGET_SHORT,
    ET_SLEEP,
    ET_OPEN_SOFTWARE,
};

typedef struct ST_MouseKey
{
    Qt::MouseButton mouseButton;
    /**
     * @brief xValue: x值
     * 后台操作时，表示位置比例
     * 前台操作时，表示具体数值
     */
    double xValue;
    /**
     * @brief yValue: y值
     * 后台操作时，表示位置比例
     * 前台操作时，表示具体数值
     */
    double yValue;

    ST_MouseKey(Qt::MouseButton _mouseButton = Qt::LeftButton)
    {
        mouseButton = _mouseButton;
        xValue = 0;
        yValue = 0;
    }
    ST_MouseKey(Qt::MouseButton _mouseButton
                , double _xValue, double _yValue)
    {
        mouseButton = _mouseButton;
        xValue = _xValue;
        yValue = _yValue;
    }

    QDataStream& operator<<(QDataStream &stream)
    {
        int mouseButton;
        stream >> mouseButton >> this->xValue >> this->yValue;
        this->mouseButton = static_cast<Qt::MouseButton>(mouseButton);
        return stream;
    }
    QDataStream& operator>>(QDataStream &stream)
    {
        stream << this->mouseButton << this->xValue << this->yValue;
        return stream;
    }

}MouseKey;

typedef  struct ST_MouseMove
{
    /**
     * @brief xValue: x值
     * 后台操作时，表示位置比例
     * 前台操作时，表示具体数值
     */
    double xValue;
    /**
     * @brief yValue: y值
     * 后台操作时，表示位置比例
     * 前台操作时，表示具体数值
     */
    double yValue;

    ST_MouseMove()
    {
        xValue = 0;
        yValue = 0;
    }
    ST_MouseMove(double _xValue, double _yValue)
    {
        xValue = _xValue;
        yValue = _yValue;
    }

    QDataStream& operator<<(QDataStream &stream)
    {
        stream >> this->xValue >> this->yValue;
        return stream;
    }
    QDataStream& operator>>(QDataStream &stream)
    {
        stream << this->xValue << this->yValue;
        return stream;
    }
}MouseMove;

typedef struct ST_MouseWheel
{
    int    wheelValue;   //滚动值
    /**
     * @brief xValue: x值
     * 后台操作时，表示位置比例
     * 前台操作时，表示具体数值
     */
    double xValue;
    /**
     * @brief yValue: y值
     * 后台操作时，表示位置比例
     * 前台操作时，表示具体数值
     */
    double yValue;

    ST_MouseWheel()
    {
        wheelValue = 120;
        xValue = 0;
        yValue = 0;
    }
    ST_MouseWheel(int _wheelValue, double _xValue, double _yValue)
    {
        wheelValue = _wheelValue;
        xValue = _xValue;
        yValue = _yValue;
    }

    QDataStream& operator<<(QDataStream &stream)
    {
        stream >> this->wheelValue >> this->xValue >> this->yValue;
        return stream;
    }
    QDataStream& operator>>(QDataStream &stream)
    {
        stream << this->wheelValue << this->xValue << this->yValue;
        return stream;
    }
}MouseWheel;

typedef struct ST_WidgetShort
{
    char baseName[259];
    char suffix[10];

    ST_WidgetShort()
    {
        strcpy(suffix, "jpg\0");
    }
    ST_WidgetShort(const QString &_baseName, const QString &_suffix)
    {
        sprintf(baseName, _baseName.toLocal8Bit().data());
        sprintf(suffix, _suffix.toLocal8Bit().data());
    }

    QDataStream& operator<<(QDataStream &stream)
    {
        QString baseName, suffix;
        stream >> baseName >> suffix;
        sprintf(this->baseName, baseName.toLocal8Bit().data());
        sprintf(this->suffix, suffix.toLocal8Bit().data());
        return stream;
    }
    QDataStream& operator>>(QDataStream &stream)
    {
        stream << QString(this->baseName) << QString(this->suffix);
        return stream;
    }
}WidgetShort;

typedef struct ST_SleepTime
{
    bool    unitIsMs; //true-毫秒为单位 false-秒为单位
    qint64 sleepTime;

    ST_SleepTime()
    {
        unitIsMs = true;
        sleepTime = 0;
    }
    ST_SleepTime(bool _unitIsMs, qint64 _sleepTime)
    {
        unitIsMs = _unitIsMs;
        sleepTime = _sleepTime;
    }

    QDataStream& operator<<(QDataStream &stream)
    {
        stream >> this->unitIsMs >> this->sleepTime;
        return stream;
    }
    QDataStream& operator>>(QDataStream &stream)
    {
        stream << this->unitIsMs << this->sleepTime;
        return stream;
    }
}SleepTime;

typedef struct ST_OpenSoftware
{
    char        path[259];

    ST_OpenSoftware()
    {
    }

    ST_OpenSoftware(const QString &_path)
    {
        sprintf(path, _path.toLocal8Bit().data());
    }

    QDataStream& operator<<(QDataStream &stream)
    {
        QString _path;
        stream >> _path;
        sprintf(path, _path.toLocal8Bit().data());
        return stream;
    }
    QDataStream& operator>>(QDataStream &stream)
    {
        stream << QString(this->path);
        return stream;
    }
}OpenSoftware;

typedef struct ST_EventItem
{
    EventType   eventType;
    QString     type1;
    QString     type2;
    QString     type3;
    union{
        MouseKey        mouseKey;
        MouseMove       mouseMove;
        MouseWheel      mouseWheel;
        Qt::Key         key;
        WidgetShort     widgetShort;
        SleepTime       sleepTime;
        OpenSoftware    openSoftware;
    };

    ST_EventItem()
    {
        eventType = ET_NONE;
    }

    QDataStream& operator<<(QDataStream &stream)
    {
        int eventType;
        stream >> eventType
                  >> this->type1 >> this->type2 >> this->type3;
        this->eventType = static_cast<EventType>(eventType);
        switch(this->eventType)
        {
        case ET_MOUSE_PRESSED:
        case ET_MOUSE_RELEASED:
        case ET_MOUSE_CLICKED:
            this->mouseKey << stream;
            break;
        case ET_MOUSE_MOVE:
            this->mouseMove << stream;
            break;
        case ET_MOUSE_WHEEL:
            this->mouseWheel << stream;
            break;
        case ET_KEY_PRESSED:
        case ET_KEY_RELEASED:
        case ET_KEY_CLICKED:
        {
            int key;
            stream >> key;
            this->key = static_cast<Qt::Key>(key);
        }
            break;
        case ET_WIDGET_SHORT:
            this->widgetShort << stream;
            break;
        case ET_SLEEP:
            this->sleepTime << stream;
            break;
        case ET_OPEN_SOFTWARE:
            this->openSoftware << stream;
            break;
        default:
            break;
        }
        return stream;
    }
    QDataStream& operator>>(QDataStream &stream)
    {
        stream << this->eventType
                  << this->type1 << this->type2 << this->type3;
        switch(this->eventType)
        {
        case ET_MOUSE_PRESSED:
        case ET_MOUSE_RELEASED:
        case ET_MOUSE_CLICKED:
            this->mouseKey >> stream;
            break;
        case ET_MOUSE_MOVE:
            this->mouseMove >> stream;
            break;
        case ET_MOUSE_WHEEL:
            this->mouseWheel >> stream;
            break;
        case ET_KEY_PRESSED:
        case ET_KEY_RELEASED:
        case ET_KEY_CLICKED:
            stream << this->key;
            break;
        case ET_WIDGET_SHORT:
            this->widgetShort >> stream;
            break;
        case ET_SLEEP:
            this->sleepTime >> stream;
            break;
        case ET_OPEN_SOFTWARE:
            this->openSoftware >> stream;
            break;
        default:
            break;
        }
        return stream;
    }
}EventItem;
typedef QList<EventItem> ListEventItem;

struct LoopItem
{
    uint times; //0时无限循环
    ListEventItem listEventItem;

    LoopItem(uint _times = 1)
    {
        times = _times;
    }

    QDataStream& operator<<(QDataStream &stream)
    {
        int length;
        stream >> times >> length;
        for(int i = 0; i < length; ++i)
        {
            EventItem eventItem;
            eventItem << stream;
            listEventItem.append(eventItem);
        }
        return stream;
    }
    QDataStream& operator>>(QDataStream &stream)
    {
        stream << times << listEventItem.length();
        for(int i = 0; i < listEventItem.length(); ++i)
        {
            listEventItem[i] >> stream;
        }
        return stream;
    }
};

typedef struct ST_EventFlow
{
    QString title;

    QList<LoopItem> listLoopItem;

    QDataStream& operator<<(QDataStream &stream)
    {
        int length;
        stream >> title >> length;
        for(int i = 0; i < length; ++i)
        {
            LoopItem loopItem;
            loopItem << stream;
            listLoopItem.append(loopItem);
        }
        return stream;
    }
    QDataStream& operator>>(QDataStream &stream)
    {
        stream << title << listLoopItem.length();
        for(int i = 0; i < listLoopItem.length(); ++i)
        {
            listLoopItem[i] >> stream;
        }
        return stream;
    }

    void clear()
    {
        title.clear();
        listLoopItem.clear();
    }

}EventFlow;

#endif // EVENTSTRUCT_H
