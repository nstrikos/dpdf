#include "pageDialog.h"
#include "ui_pageDialog.h"

#include <QMessageBox>

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
    bool textIsInteger;
    int i = ui->lineEdit->text().toInt(&textIsInteger);

    ui->lineEdit->clear();

    if (textIsInteger) {
        emit goToPage(i);
        hide();
    } else {
        QMessageBox msgBox;
        QString text = tr("You must insert a number");
        msgBox.setText(text);
        msgBox.setAccessibleName(text);
        msgBox.exec();
    }
}
