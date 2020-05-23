#include "contentsDialog.h"
#include "ui_contentsDialog.h"

#include <QKeyEvent>

ContentsDialog::ContentsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContentsDialog)
{
    ui->setupUi(this);
}

ContentsDialog::~ContentsDialog()
{
    delete ui;
}

void ContentsDialog::setContents(QVector<QString> *titles)
{
    ui->listWidget->clear();

    for (int i = 0; i < titles->size(); i++) {
        QListWidgetItem *newItem = new QListWidgetItem(titles->at(i));
        ui->listWidget->addItem(newItem);
    }

    ui->listWidget->setCurrentRow(0);
}

void ContentsDialog::clear()
{
    ui->listWidget->clear();
}

bool ContentsDialog::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (event->type() == QEvent::KeyPress) {

        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if ( (key->key() == Qt::Key_Escape) )
            hide();
    }

    return false;
}

void ContentsDialog::on_listWidget_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);

    emit contentSelected(ui->listWidget->currentRow());
}

void ContentsDialog::on_listWidget_itemActivated(QListWidgetItem *item)
{
    Q_UNUSED(item);

    emit contentSelected(ui->listWidget->currentRow());
}

void ContentsDialog::on_closeButton_clicked()
{
    hide();
}
