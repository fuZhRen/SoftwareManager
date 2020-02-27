#include "taskmanager.h"

#include "eventmanager.h"
#include "choosewinwidget.h"

#include <QMutexLocker>
#include <QMessageBox>

TaskManager::TaskManager(QObject *parent)
    : QThread(parent)
    , m_wigId(nullptr)
    , m_currentLoopItemIndex(-1)
    , m_currentLoopItemTimes(-1)
    , m_currentEventItemIndex(-1)
    , m_state(0)
{
    //
}

bool TaskManager::setEventFlow(const EventFlow &eventFlow)
{
    QMutexLocker locker(&m_mutex);
    this->stop();
    m_eventFlow = eventFlow;
    m_wigId = nullptr;
    m_currentLoopItemIndex = -1;
    m_currentLoopItemTimes = -1;
    m_currentEventItemIndex = -1;

    QList<TagWinWidget> listWinWidget = HwndManager::instance()->getWinWidget(m_eventFlow.title);
    if(listWinWidget.length() == 0)
    {
        QMessageBox::information(nullptr, "错误", QString("未找到标题为%1的窗口").arg(m_eventFlow.title));
    }
    else if(listWinWidget.length() == 1)
    {
        m_wigId = listWinWidget.first().hwnd;

        this->start();
        return true;
    }
    else
    {
        ChooseWinWidget chooseWinWidget;
        int index = chooseWinWidget.chooseIndex(listWinWidget);
        if(index == -1)
        {
            QMessageBox::information(nullptr, "错误", QString("未选择窗口").arg(m_eventFlow.title));
        }
        else
        {
            m_wigId = listWinWidget[index].hwnd;

            this->start();
            return true;
        }
    }
    return false;
}

void TaskManager::run()
{
    if(!m_wigId)
    {
        return;
    }
    m_currentEventItem = this->getEventItem();
    while(m_currentEventItem.eventType != ET_NONE)
    {
        EventManager::doEventItem(m_wigId, m_currentEventItem);
        emit sglFinishedTask(m_currentEventItem.type1, m_currentEventItem.type2, m_currentEventItem.type3);
        if(m_state == 0 || m_state == 2)
        {
            return;
        }
        m_currentEventItem = this->getEventItem();
    }
}

const EventItem &TaskManager::getEventItem()
{
    QMutexLocker locker(&m_mutex);
    return this->getNextEventItem();
}

const EventItem &TaskManager::getNextLoopItem()
{
    if(m_currentLoopItemIndex < -1)
    {
        m_currentLoopItemIndex = -1;
    }

    if(m_currentLoopItemIndex < m_eventFlow.listLoopItem.length() - 1)
    {
        m_currentLoopItemIndex++;
        m_currentLoopItemTimes = -1;
        return this->getNextTimes();
    }
    return m_eventItem;
}

const EventItem &TaskManager::getNextTimes()
{
    if(m_currentLoopItemIndex < 0)
    {
        return this->getNextLoopItem();
    }
    if(m_currentLoopItemTimes < -1)
    {
        m_currentLoopItemTimes = -1;
    }

    if(m_currentLoopItemTimes < static_cast<int>(m_eventFlow.listLoopItem[m_currentLoopItemIndex].times))
    {
        m_currentLoopItemTimes++;
        m_currentEventItemIndex = -1;
        return this->getNextEventItem();
    }
    else if(m_eventFlow.listLoopItem[m_currentLoopItemIndex].times == 0)
    {
        m_currentEventItemIndex = -1;
        return this->getNextEventItem();
    }
    return this->getNextLoopItem();
}

const EventItem &TaskManager::getNextEventItem()
{
    if(m_currentLoopItemIndex < 0)
    {
        return this->getNextLoopItem();
    }
    if(m_currentLoopItemTimes < 0)
    {
        return this->getNextTimes();
    }

    if(m_currentEventItemIndex < -1)
    {
        m_currentEventItemIndex = -1;
    }

    if(m_currentEventItemIndex < m_eventFlow.listLoopItem[m_currentLoopItemIndex].listEventItem.length() - 1)
    {
        m_currentEventItemIndex++;
        return m_eventFlow.listLoopItem[m_currentLoopItemIndex].listEventItem[m_currentEventItemIndex];
    }
    else
    {
        return this->getNextTimes();
    }
}

EventFlow TaskManager::getListEventItem() const
{
    return m_eventFlow;
}

void TaskManager::pause()
{
    m_state = 2;
}

void TaskManager::stop()
{
    m_state = 0;

    m_currentLoopItemIndex = -1;
    m_currentLoopItemTimes = -1;
    m_currentEventItemIndex = -1;
}

void TaskManager::start(QThread::Priority priority)
{
    m_state = 1;
    if(!this->isRunning())
    {
        QThread::start(priority);
    }
}
