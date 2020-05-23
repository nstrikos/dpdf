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
    void clear();

signals:
    void contentSelected(int item);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_listWidget_itemActivated(QListWidgetItem *item);
    void on_closeButton_clicked();

private:
    Ui::ContentsDialog *ui;
};

#endif // CONTENTSDIALOG_H
