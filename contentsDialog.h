#ifndef CONTENTSDIALOG_H
#define CONTENTSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ContentsDialog;
}

class ContentsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContentsDialog(QWidget *parent = nullptr);
    ~ContentsDialog();
    void setContents(QVector<QString> *titles);

signals:
    void contentSelected(int item);

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_listWidget_itemPressed(QListWidgetItem *item);

    void on_listWidget_itemActivated(QListWidgetItem *item);

private:
    Ui::ContentsDialog *ui;
};

#endif // CONTENTSDIALOG_H
