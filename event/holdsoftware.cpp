#include "holdsoftware.h"

#include "eventmanager.h"

#include <QTimerEvent>
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QDebug>
#include <QMessageBox>

#include <math.h>

#define UPDATE_WIDGET_INTERVAL 100
#define TIMEKEEPING_INTERVAL 10

HoldSoftWare::HoldSoftWare(QWidget *parent)
    : QWidget(parent)
    , m_currentWigId(nullptr)
    , m_updateWigTimer(0)
    , m_timekeeping(0)
    , m_isRecord(false)
{
//    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    m_pTimeButton = new QPushButton(this);
    m_pTimeButton->setText("0");
    m_pTimeButton->setFixedHeight(30);
    m_pTimeButton->setFixedWidth(this->width());
    m_pTimeButton->move(0, this->height() - m_pTimeButton->height());

    m_pScreen = QApplication::screens().first();
}

void HoldSoftWare::setWigId(const HWND &currentWigId, const QString &title)
{
    if(m_currentWigId != currentWigId)
    {
        if(m_isRecord)
        {
            if(QMessageBox::question(nullptr, "提示", "正在进行操作录制，是否关闭录制？") == QMessageBox::No)
            {
                return;
            }
            else if(m_timekeeping != 0)
            {
                this->killTimer(m_timekeeping);
                m_timekeeping = 0;
            }
        }
        m_currentWigId = currentWigId;
        m_title = title;

        m_isRecord = false;
        m_msecond = 0;
        m_pTimeButton->setText(QString::number(m_msecond));
    }
    else if(m_isRecord && m_timekeeping == 0)
    {
        m_timekeeping = this->startTimer(TIMEKEEPING_INTERVAL);
    }

    if(m_currentWigId && m_updateWigTimer == 0)
    {
        m_updateWigTimer = this->startTimer(UPDATE_WIDGET_INTERVAL);
    }
    else if(!m_currentWigId && m_updateWigTimer != 0)
    {
        this->killTimer(m_updateWigTimer);
        m_updateWigTimer = 0;

        this->update();
    }
}

void HoldSoftWare::startRecord()
{
    if(m_currentWigId && m_timekeeping == 0)
    {
        m_msecond = 0;

        m_eventFlow.clear();
        m_eventFlow.listLoopItem.append(LoopItem());

        m_timekeeping = this->startTimer(TIMEKEEPING_INTERVAL);
        m_isRecord = true;
    }
}

void HoldSoftWare::endRecord(const QString &fileName)
{
    if(m_isRecord)
    {
        if(m_timekeeping)
        {
            this->killTimer(m_timekeeping);
            m_timekeeping = 0;
        }

        this->addSleepTime();

        m_msecond = 0;
        m_pTimeButton->setText(QString::number(m_msecond));
        m_isRecord = false;

        m_eventFlow.title = m_title;
        this->clearUpFlow();

        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly))
        {
            QDataStream dataStream(&file);
            m_eventFlow >> dataStream;
            file.close();
            QMessageBox::information(nullptr, "提示", "保存成功。");
        }
        else
        {
            QMessageBox::information(nullptr, "错误", QString("文件'%1'打开失败!").arg(fileName));
        }
    }
    else
    {
        QMessageBox::information(nullptr, "错误", "当前未进行录制！");
    }
}

void HoldSoftWare::addNewLoop(uint times)
{
    if(m_isRecord)
    {
        this->addSleepTime();
        m_eventFlow.listLoopItem.append(LoopItem(times));
    }
}

void HoldSoftWare::mousePressEvent(QMouseEvent *event)
{
    if(m_currentWigId)
    {
        double xPercent = event->x()*1.0/this->width();
        double yPercent = event->y()*1.0/this->height();

        this->addSleepTime();

        EventItem eventItem;
        eventItem.eventType = ET_MOUSE_PRESSED;
        eventItem.mouseKey = MouseKey(event->button(), xPercent, yPercent);

        if(m_isRecord)
        {
            m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        }

        EventManager::backstageDoEventItem(m_currentWigId, eventItem);
    }
}

void HoldSoftWare::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_currentWigId)
    {
        double xPercent = event->x()*1.0/this->width();
        double yPercent = event->y()*1.0/this->height();

        this->addSleepTime();

        EventItem eventItem;
        eventItem.eventType = ET_MOUSE_RELEASED;
        eventItem.mouseKey = MouseKey(event->button(), xPercent, yPercent);

        if(m_isRecord)
        {
            m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        }

        EventManager::backstageDoEventItem(m_currentWigId, eventItem);
    }
}

void HoldSoftWare::mouseMoveEvent(QMouseEvent *event)
{
    if(m_currentWigId)
    {
        double xPercent = event->x()*1.0/this->width();
        double yPercent = event->y()*1.0/this->height();

        this->addSleepTime();

        EventItem eventItem;
        eventItem.eventType = ET_MOUSE_MOVE;
        eventItem.mouseMove = MouseMove(xPercent, yPercent);

        if(m_isRecord)
        {
            m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        }

        EventManager::backstageDoEventItem(m_currentWigId, eventItem);
    }
}

void HoldSoftWare::wheelEvent(QWheelEvent *event)
{
    QWidget::wheelEvent(event);

    if(m_currentWigId)
    {
        double xPercent = event->x()*1.0/this->width();
        double yPercent = event->y()*1.0/this->height();

        this->addSleepTime();

        EventItem eventItem;
        eventItem.eventType = ET_MOUSE_WHEEL;
        eventItem.mouseWheel = MouseWheel(event->delta(), xPercent, yPercent);

        if(m_isRecord)
        {
            m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        }

        EventManager::backstageDoEventItem(m_currentWigId, eventItem);
    }
}

void HoldSoftWare::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);

    if(event->key() == Qt::Key_Alt)
    {
        return;
    }
    if(m_currentWigId)
    {
        this->addSleepTime();

        EventItem eventItem;
        eventItem.eventType = ET_KEY_PRESSED;
        eventItem.key = static_cast<Qt::Key>(event->key());

        if(m_isRecord)
        {
            m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        }

        EventManager::backstageDoEventItem(m_currentWigId, eventItem);
    }
}

void HoldSoftWare::keyReleaseEvent(QKeyEvent *event)
{
    QWidget::keyReleaseEvent(event);

    if(m_currentWigId)
    {
        this->addSleepTime();

        EventItem eventItem;
        eventItem.eventType = ET_KEY_RELEASED;
        eventItem.key = static_cast<Qt::Key>(event->key());

        if(m_isRecord)
        {
            m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        }

        EventManager::backstageDoEventItem(m_currentWigId, eventItem);
    }
}

void HoldSoftWare::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_updateWigTimer)
    {
        m_pixMap = m_pScreen->grabWindow(WId(m_currentWigId)).scaled(this->width(), this->height() - m_pTimeButton->height());
        this->update();
    }
    else if(event->timerId() == m_timekeeping)
    {
        m_msecond += TIMEKEEPING_INTERVAL;
        m_pTimeButton->setText(QString::number(m_msecond));
    }
}

void HoldSoftWare::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if(m_currentWigId)
    {
        QPainter painter(this);
        painter.drawPixmap(this->rect(), m_pixMap);
    }
}

void HoldSoftWare::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);

    if(m_timekeeping != 0)
    {
        this->killTimer(m_updateWigTimer);
        m_updateWigTimer = 0;

        this->killTimer(m_timekeeping);
        m_timekeeping = 0;
    }
}

void HoldSoftWare::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

void HoldSoftWare::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    m_pTimeButton->setFixedWidth(this->width());
    m_pTimeButton->move(0, this->height() - m_pTimeButton->height());
}

void HoldSoftWare::addSleepTime()
{
    if(m_isRecord && m_msecond > 0)
    {
        EventItem eventItem;
        eventItem.eventType = ET_SLEEP;
        eventItem.sleepTime = SleepTime(true, m_msecond);
        m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        m_msecond = 0;
    }
}

void HoldSoftWare::clearUpFlow()
{
    for(int i = 0; i < m_eventFlow.listLoopItem.length(); ++i)
    {
        this->clearUpLoop(m_eventFlow.listLoopItem[i]);
    }
}

void HoldSoftWare::clearUpLoop(LoopItem &loopItem)
{
    ListEventItem::Iterator itr = loopItem.listEventItem.begin();
    ListEventItem::Iterator endItr = loopItem.listEventItem.end();

    while(itr != endItr)
    {
        if(itr->eventType == ET_MOUSE_PRESSED)
        {
            this->clearMousePressed(itr, endItr, loopItem.listEventItem);
        }
        else if(itr->eventType == ET_KEY_PRESSED)
        {
            this->clearKeyPressed(itr, endItr, loopItem.listEventItem);
        }
        ++itr;
    }

    itr = loopItem.listEventItem.begin();
    endItr = loopItem.listEventItem.end();
    while(itr != endItr)
    {
        if(itr->eventType == ET_SLEEP
                && itr->sleepTime.unitIsMs == true
                && itr->sleepTime.sleepTime > 1000)
        {
            itr->sleepTime.unitIsMs = false;
            itr->sleepTime.sleepTime = static_cast<qint64>(ceil(itr->sleepTime.sleepTime/1000.0));
        }
        ++itr;
    }
}

void HoldSoftWare::clearMousePressed(ListEventItem::Iterator &pressedItr, ListEventItem::Iterator &endItr, ListEventItem &listEventItem)
{
    qint64 sleepTime = 0;
    double moveXPercent = 0, moveYPercent = 0;
    ListEventItem::Iterator moveItr = pressedItr;
    ++moveItr;
    while(moveItr != endItr)
    {
        if(moveItr->eventType == ET_SLEEP)
        {
            sleepTime += moveItr->sleepTime.sleepTime;
        }
        else if(moveItr->eventType == ET_MOUSE_MOVE)
        {
            moveXPercent = moveItr->mouseMove.xPercent;
            moveYPercent = moveItr->mouseMove.yPercent;
        }
        else if(moveItr->eventType == ET_MOUSE_RELEASED
                && moveItr->mouseKey.mouseButton == pressedItr->mouseKey.mouseButton)
        {

            if(sleepTime < 300
                    && this->pixelInterval(pressedItr->mouseKey.xPercent, pressedItr->mouseKey.yPercent
                                           , moveItr->mouseKey.xPercent, moveItr->mouseKey.yPercent) < 4)
            {
                pressedItr->eventType = ET_MOUSE_CLICKED;
                ++moveItr;
            }
            else
            {
                ListEventItem::Iterator sleepItr = moveItr;
                --sleepItr;
                if(sleepItr->eventType == ET_SLEEP)
                {
                    sleepTime -= (*sleepItr).sleepTime.sleepTime;
                }
                else
                {
                    ++sleepItr;
                }

                EventItem eventItem;
                eventItem.eventType = ET_SLEEP;
                if(sleepTime > 1000)
                {
                    eventItem.sleepTime = SleepTime(false, static_cast<qint64>(ceil(sleepTime/1000.0)));
                }
                else if(sleepTime > 0)
                {
                    eventItem.sleepTime = SleepTime(true, sleepTime);
                }
                ListEventItem::Iterator instertSleepItr = listEventItem.insert(sleepItr, eventItem);
                sleepItr++;

                eventItem.eventType = ET_MOUSE_MOVE;
                eventItem.mouseMove = MouseMove(moveXPercent, moveYPercent);
                listEventItem.insert(sleepItr, eventItem);

                moveItr = instertSleepItr;
            }

            ListEventItem::Iterator removeBegin = pressedItr;
            ++removeBegin;
            listEventItem.erase(removeBegin, moveItr);

            break;
        }
        ++moveItr;
    }
}

void HoldSoftWare::clearKeyPressed(ListEventItem::Iterator &pressedItr, ListEventItem::Iterator &endItr, ListEventItem &listEventItem)
{
    ListEventItem::Iterator moveItr = pressedItr;
    ++moveItr;
    while(moveItr != endItr)
    {
        if(moveItr->eventType == ET_KEY_PRESSED)
        {
            if(moveItr->key == pressedItr->key)
            {
                pressedItr->eventType = ET_KEY_CLICKED;
                ++moveItr;

                ListEventItem::Iterator removeBegin = pressedItr;
                ++removeBegin;
                listEventItem.erase(removeBegin, moveItr);
            }
            break;
        }
        ++moveItr;
    }
}

double HoldSoftWare::pixelInterval(double xPercent1, double yPercent1, double xPercent2, double yPercent2)
{
    return sqrt(pow(this->width()*(xPercent1 - xPercent2), 2) + pow(this->height()*(yPercent1 - yPercent2), 2));
}
