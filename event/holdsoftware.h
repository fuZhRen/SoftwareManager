#ifndef HOLDSOFTWARE_H
#define HOLDSOFTWARE_H

#include <QWidget>

#include <windows.h>
#include <QPushButton>

#include "EventStruct.h"

class HoldSoftWare : public QWidget
{
    Q_OBJECT
public:
    explicit HoldSoftWare(QWidget *parent = nullptr);

    void setWigId(const HWND &currentWigId, const QString &title);

    //开始记录
    void startRecord();
    //结束记录
    void endRecord();

    //新增循环记录
    void addNewLoop(uint times);

signals:

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    virtual void wheelEvent(QWheelEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

    virtual void timerEvent(QTimerEvent *event) override;

    virtual void paintEvent(QPaintEvent *event) override;

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void addSleepTime(EventItem &eventItem);

private:
    QPushButton *m_pTimeButton;

    HWND    m_currentWigId;
    QString m_title;
    //用于更新传过来的界面
    int     m_updateWigTimer;

    QScreen *m_pScreen;
    QPixmap m_pixMap;

    //用于按键等操作间隔计时
    int     m_timekeeping;
    //记录时间毫秒
    unsigned long   m_msecond;
    //记录操作
    EventFlow    m_eventFlow;
    //是否正在记录
    bool    m_isRecord;
};

#endif // HOLDSOFTWARE_H
