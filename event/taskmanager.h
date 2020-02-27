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

    bool setEventFlow(const EventFlow &eventFlow);

    EventFlow getListEventItem() const;

    //暂停
    void pause();
    //停止
    void stop();

signals:
    void sglFinishedTask(QString type1, QString type2, QString type3);

public slots:
    void start(QThread::Priority priority = InheritPriority);

protected:
    virtual void run() override;

private:
    //获取执行事件
    const EventItem &getEventItem();

    //获取下一个循环元素
    const EventItem &getNextLoopItem();
    //获取循环元素的下一次循环
    const EventItem &getNextTimes();
    //获取下一个执行事件元素
    const EventItem &getNextEventItem();

private:
    QMutex          m_mutex;
    EventFlow       m_eventFlow;
    HWND            m_wigId;

    //当前执行的LoopItem序号
    int             m_currentLoopItemIndex;
    //当前执行的LoopItem循环次数序号
    int            m_currentLoopItemTimes;
    //当前执行的EventItem序号
    int             m_currentEventItemIndex;

    EventItem       m_currentEventItem;

    //0-停止 1-开始 2-暂停
    int             m_state;

    //用于函数的返回值
    const EventItem m_eventItem;
};

#endif // TASKMANAGER_H
