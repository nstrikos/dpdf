#include "pageDialog.h"
#include "ui_pageDialog.h"

PageDialog::PageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PageDialog)
{
    ui->setupUi(this);
}

PageDialog::~PageDialog()
{
    delete ui;
}

void PageDialog::on_okButton_clicked()
{
    int i = ui->lineEdit->text().toInt();
    emit goToPage(i);
    hide();
}
