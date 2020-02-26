#include "choosewinwidget.h"
#include "ui_choosewinwidget.h"

ChooseWinWidget::ChooseWinWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseWinWidget)
  , m_chooseIndex(-1)
{
    ui->setupUi(this);
}

ChooseWinWidget::~ChooseWinWidget()
{
    delete ui;
}

int ChooseWinWidget::chooseIndex(const QList<TagWinWidget> &listWinWidget)
{
    ui->listWidget->clear();
    m_chooseIndex = -1;
    for(int i = 0; i < listWinWidget.length(); ++i)
    {
        ui->listWidget->addItem(listWinWidget[i].title);
    }
    if(listWinWidget.length() > 0)
    {
        ui->listWidget->setCurrentRow(0);
        m_chooseIndex = 0;
    }
    this->exec();
    return m_chooseIndex;
}

void ChooseWinWidget::on_ok_btn_clicked()
{
    m_chooseIndex = ui->listWidget->currentRow();
    this->close();
}

void ChooseWinWidget::on_cancel_btn_clicked()
{
    m_chooseIndex = -1;
    this->close();
}
