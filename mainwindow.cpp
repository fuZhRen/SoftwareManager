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
BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM lParam);

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

QString MainWindow::filterString()
{
    return ui->search_line_edit->text();
}

void MainWindow::addSoftWare(QString title, HWND hwnd)
{
    ui->show_software->addItem(title);
    m_listHwnd.append(hwnd);
}

void MainWindow::open_yeShenMoNiQi()
{
//    m_process = new QProcess(this);
//    QString sProgram = "E:/Program Files/Nox/bin/Nox.exe";
//    m_process->setProgram(sProgram);

//    MainWindow::S_WigId = (HWND)FindWindow(nullptr, L"夜神模拟器");
//    qDebug() << MainWindow::S_WigId;
}

////EnumWindows回调函数，hwnd为发现的顶层窗口
//BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
//{
//	if (GetParent(hWnd) == nullptr && IsWindowVisible(hWnd))  //判断是否顶层窗口并且可见
//	{
//		char WindowTitle[100] = { 0 };
//		::GetWindowText(hWnd, WindowTitle, 100);
//		printf("%s\n", WindowTitle);

//		EnumChildWindows(hWnd, EnumChildWindowsProc, NULL); //获取父窗口的所有子窗口
//		ok = false;
//	}

//	return true;
//}

//EnumChildWindows回调函数，hwnd为指定的父窗口
BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM lParam)
{
    Q_UNUSED(lParam)

    char WindowTitle[100] = { 0 };
    ::GetWindowText(hWnd, LPWSTR(WindowTitle), 100);

    QString title = QString::fromStdWString(LPWSTR(WindowTitle));

    MainWindow *pWindow = MainWindow::s_pMainWindow;
    if(title.contains(pWindow->filterString()))
    {
        pWindow->addSoftWare(title, hWnd);
    }

    return true;
}

void MainWindow::on_search_software_clicked()
{
    ui->show_software->clear();
    m_listHwnd.clear();
    EnumChildWindows(nullptr, EnumChildWindowsProc, 0);
}

void MainWindow::on_mapping_clicked()
{
    if(ui->show_software->currentRow() > -1)
    {
        QString title = ui->show_software->currentItem()->text();
        m_pHoldSoftWare->setWigId(m_listHwnd[ui->show_software->currentRow()], title);
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
