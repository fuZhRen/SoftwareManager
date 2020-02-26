#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QCursor>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>

#include <QWindow>

#include "event/eventmanager.h"

#define MAIN_WINDW "MAIN_WINDW"
#define RESIZE "RESIZE"

//BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
//BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM lParam);

MainWindow* MainWindow::s_pMainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MainWindow::s_pMainWindow = this;

    m_pHoldSoftWare = new HoldSoftWare;
    m_pHoldSoftWare->hide();
}

MainWindow::~MainWindow()
{
    delete m_pHoldSoftWare;
    delete ui;
}

void MainWindow::on_search_software_clicked()
{
    ui->show_software->clear();
    m_listWinWidget.clear();

    m_listWinWidget = HwndManager::instance()->getWinWidget(ui->search_line_edit->text());
    for(int i = 0; i < m_listWinWidget.length(); ++i)
    {
        ui->show_software->addItem(m_listWinWidget[i].title);
    }
}

void MainWindow::on_mapping_clicked()
{
    if(ui->show_software->currentRow() > -1)
    {
        QString title = ui->show_software->currentItem()->text();
        m_pHoldSoftWare->setWigId(m_listWinWidget[ui->show_software->currentRow()].hwnd, title);
        m_pHoldSoftWare->show();
    }
}

void MainWindow::on_close_mapping_clicked()
{
    m_pHoldSoftWare->setWigId(nullptr, "");
    m_pHoldSoftWare->hide();
}

void MainWindow::on_switchover_save_file_clicked()
{
    if(m_pHoldSoftWare->isHidden())
    {
        QMessageBox::information(nullptr, "提示", "请打开映射界面!");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(nullptr, "", "", "Binary files(*.sv)");
    if(!fileName.isEmpty())
    {
        ui->save_task_file->setText(fileName);
    }
}

void MainWindow::on_begin_record_clicked()
{
    m_pHoldSoftWare->startRecord();
}

void MainWindow::on_end_record_clicked()
{
    m_pHoldSoftWare->endRecord();
}
