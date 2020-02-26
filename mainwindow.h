#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <windows.h>

#include "event/holdsoftware.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

    QString filterString();
    void addSoftWare(QString title, HWND hwnd);

private slots:
    /**
     * @brief on_search_software_clicked: 根据名称搜索应用
     */
    void on_search_software_clicked();

    /**
     * @brief on_mapping_clicked: 映射选中软件的界面
     */
    void on_mapping_clicked();
    /**
     * @brief on_close_mapping_clicked: 关闭映射
     */
    void on_close_mapping_clicked();

    /**
     * @brief on_switchover_save_file_clicked: 选择保存的文件路径
     */
    void on_switchover_save_file_clicked();
    /**
     * @brief on_begin_record_clicked: 开始记录
     */
    void on_begin_record_clicked();
    /**
     * @brief on_end_record_clicked:  结束记录
     */
    void on_end_record_clicked();

private:
    void open_yeShenMoNiQi();

private:
    Ui::MainWindow  *ui;
    HoldSoftWare    *m_pHoldSoftWare;
    QList<HWND>     m_listHwnd;

public:
    static MainWindow   *s_pMainWindow;
};
#endif // MAINWINDOW_H
