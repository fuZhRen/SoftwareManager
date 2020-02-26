#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include "EventStruct.h"

class TaskManager : public QThread
{
    Q_OBJECT
public:
    explicit TaskManager(QObject *parent = nullptr);

    void setEventFlow(const EventFlow &eventFlow);

    EventFlow getListEventItem() const;

signals:
    void sglFinishedTask(QString type1, QString type2, QString type3);

protected:
    virtual void run() override;

private:
    EventItem getFirstEventItem();

    //初始化循环信息，最初开启时
    bool  initLoopInfo();

private:
    QMutex          m_mutex;
    EventFlow       m_eventFlow;
    HWND            m_wigId;

    //当前执行的LoopItem序号
    int             m_currentLoopItemIndex;
    //当前执行的LoopItem循环次数序号
    uint            m_currentLoopIndex;
    //当前执行的EventItem序号
    int             m_currentEventItemIndex;

    EventItem       m_currentEventItem;
};

#endif // TASKMANAGER_H
