#ifndef CHOOSEWINWIDGET_H
#define CHOOSEWINWIDGET_H

#include <QDialog>

#include "hwndmanager.h"

namespace Ui {
class ChooseWinWidget;
}

class ChooseWinWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseWinWidget(QWidget *parent = nullptr);
    ~ChooseWinWidget();

    int chooseIndex(const QList<TagWinWidget> &listWinWidget);

private slots:
    void on_ok_btn_clicked();

    void on_cancel_btn_clicked();

private:
    Ui::ChooseWinWidget *ui;

    int     m_chooseIndex;
};

#endif // CHOOSEWINWIDGET_H
