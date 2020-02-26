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

    void setListEventItem(const ListEventItem &listEventItem);

    void addEventItem(const EventItem &eventItem);

    ListEventItem getListEventItem() const;

signals:
    void sglFinishedTask(QString type1, QString type2, QString type3);

protected:
    virtual void run() override;

private:
    EventItem getFirstEventItem();

private:
    QMutex        m_mutex;
    ListEventItem m_listEventItem;

    EventItem     m_currentEventItem;
    bool          m_currentEventFinished;
};

#endif // TASKMANAGER_H
