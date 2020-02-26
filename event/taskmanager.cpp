#include "taskmanager.h"

#include "eventmanager.h"

#include <QMutexLocker>

TaskManager::TaskManager(QObject *parent)
    : QThread(parent)
    , m_currentEventFinished(true)
{
    //
}

void TaskManager::setListEventItem(const ListEventItem &listEventItem)
{
    QMutexLocker locker(&m_mutex);
    this->quit();
    m_listEventItem = listEventItem;
    this->start();
}

void TaskManager::addEventItem(const EventItem &eventItem)
{
    QMutexLocker locker(&m_mutex);
    m_listEventItem.append(eventItem);
    if(!this->isRunning())
    {
        this->start();
    }
}

void TaskManager::run()
{
    m_currentEventItem = this->getFirstEventItem();
    m_currentEventFinished = false;
    while(m_currentEventItem.eventType != ET_NONE)
    {
        EventManager::doEventItem(m_currentEventItem);
        emit sglFinishedTask(m_currentEventItem.type1, m_currentEventItem.type2, m_currentEventItem.type3);
        m_currentEventFinished = true;
        m_currentEventItem = this->getFirstEventItem();
        m_currentEventFinished = false;
    }
    m_currentEventFinished = true;
}

EventItem TaskManager::getFirstEventItem()
{
    QMutexLocker locker(&m_mutex);
    if(m_listEventItem.length() > 0)
    {
        return m_listEventItem.takeFirst();
    }
    return EventItem();
}

ListEventItem TaskManager::getListEventItem() const
{
    return m_listEventItem;
}
