#include "taskmanager.h"

#include "eventmanager.h"
#include "choosewinwidget.h"

#include <QMutexLocker>
#include <QMessageBox>

TaskManager::TaskManager(QObject *parent)
    : QThread(parent)
    , m_wigId(nullptr)
    , m_currentLoopItemIndex(-1)
    , m_currentLoopIndex(0)
    , m_currentEventItemIndex(-1)
{
    //
}

bool TaskManager::setEventFlow(const EventFlow &eventFlow)
{
    QMutexLocker locker(&m_mutex);
    this->exit();
    m_eventFlow = eventFlow;
    m_wigId = nullptr;
    m_currentLoopItemIndex = -1;
    m_currentLoopIndex= 0;
    m_currentEventItemIndex = -1;

    QList<TagWinWidget> listWinWidget = HwndManager::instance()->getWinWidget(m_eventFlow.title);
    if(listWinWidget.length() == 0)
    {
        QMessageBox::information(nullptr, "错误", QString("未找到标题为%1的窗口").arg(m_eventFlow.title));
    }
    else if(listWinWidget.length() == 1)
    {
        m_wigId = listWinWidget.first().hwnd;
        this->initLoopInfo();

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
            this->initLoopInfo();

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
    m_currentEventItem = this->getFirstEventItem();
    while(m_currentEventItem.eventType != ET_NONE)
    {
        EventManager::doEventItem(m_wigId, m_currentEventItem);
        emit sglFinishedTask(m_currentEventItem.type1, m_currentEventItem.type2, m_currentEventItem.type3);
        m_currentEventItem = this->getFirstEventItem();
    }
}

EventItem TaskManager::getFirstEventItem()
{
    QMutexLocker locker(&m_mutex);

    //判断LoopItem序号和循环次数
    if(m_currentLoopItemIndex > -1)
    {
        if(m_currentEventItemIndex < m_eventFlow.listLoopItem[m_currentLoopItemIndex].listEventItem.length() - 1) //未开始时
        {
            m_currentEventItemIndex++;
            return m_eventFlow.listLoopItem[m_currentLoopItemIndex].listEventItem[m_currentEventItemIndex];
        }
        else if(m_currentEventItemIndex == m_eventFlow.listLoopItem[m_currentLoopItemIndex].listEventItem.length() - 1)
        {
            if(m_eventFlow.listLoopItem[m_currentLoopItemIndex].times == 0)
            {
                m_currentEventItemIndex = 0;
                return m_eventFlow.listLoopItem[m_currentLoopItemIndex].listEventItem[m_currentEventItemIndex];
            }
            else if(m_currentLoopIndex < m_eventFlow.listLoopItem[m_currentLoopItemIndex].times)
            {
                m_currentLoopIndex++;
                m_currentEventItemIndex = 0;
                return m_eventFlow.listLoopItem[m_currentLoopItemIndex].listEventItem[m_currentEventItemIndex];
            }
            else if(m_currentLoopIndex == m_eventFlow.listLoopItem[m_currentLoopItemIndex].times)
            {
                while(m_currentLoopItemIndex < m_eventFlow.listLoopItem.length())
                {
                    m_currentLoopItemIndex++;
                    m_currentLoopIndex = 1;
                    if(m_eventFlow.listLoopItem[m_currentLoopItemIndex].listEventItem.length() > 0)
                    {
                        m_currentEventItemIndex = 0;
                        return m_eventFlow.listLoopItem[m_currentLoopItemIndex].listEventItem[m_currentEventItemIndex];
                    }
                }
            }
        }
    }

    return EventItem();
}

bool TaskManager::initLoopInfo()
{
    //执行的LoopItem序号更新
    if(m_currentLoopItemIndex < 0)
    {
        if(m_eventFlow.listLoopItem.length() > m_currentLoopItemIndex)
        {
            m_currentLoopItemIndex = 0;
        }
    }

    //执行的LoopItem循环次数序号更新
    if(m_currentLoopItemIndex > -1)
    {
        if(m_currentLoopIndex == 0
                && m_eventFlow.listLoopItem[m_currentLoopItemIndex].times > 0)
        {
            m_currentLoopIndex = 1;
            return true;
        }
        else if(m_eventFlow.listLoopItem[m_currentLoopItemIndex].times == 0)
        {
            m_currentLoopIndex = 0;
            return true;
        }
    }

    return false;
}

EventFlow TaskManager::getListEventItem() const
{
    return m_eventFlow;
}
