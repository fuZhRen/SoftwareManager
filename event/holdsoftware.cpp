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

        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly))
        {
            QDataStream dataStream(&file);
            m_eventFlow >> dataStream;
            file.close();
            QMessageBox::information(nullptr, "错误", "保存成功。");
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
        eventItem.eventType = ET_MOUSE_KEY_PRESSED;
        eventItem.mouseKey = MouseKey(event->button(), xPercent, yPercent);

        if(m_isRecord)
        {
            m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        }

        EventManager::doEventItem(m_currentWigId, eventItem);
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
        eventItem.eventType = ET_MOUSE_KEY_RELEASED;
        eventItem.mouseKey = MouseKey(event->button(), xPercent, yPercent);

        if(m_isRecord)
        {
            m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        }

        EventManager::doEventItem(m_currentWigId, eventItem);
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

        EventManager::doEventItem(m_currentWigId, eventItem);
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

        EventManager::doEventItem(m_currentWigId, eventItem);
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

        EventManager::doEventItem(m_currentWigId, eventItem);
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

        EventManager::doEventItem(m_currentWigId, eventItem);
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
        if(m_msecond < 1000)
        {
            eventItem.sleepTime = SleepTime(true, m_msecond);
        }
        else
        {
            eventItem.sleepTime = SleepTime(false, static_cast<unsigned long>(ceil(m_msecond/1000.0)));
        }
        m_eventFlow.listLoopItem.last().listEventItem.append(eventItem);
        m_msecond = 0;
    }
}
