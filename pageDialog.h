#ifndef PAGEDIALOG_H
#define PAGEDIALOG_H

#include <QDialog>

namespace Ui {
class PageDialog;
}

class PageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PageDialog(QWidget *parent = nullptr);
    ~PageDialog();

signals:
    void goToPage(int page);

private slots:
    void on_okButton_clicked();

private:
    Ui::PageDialog *ui;
};

#endif // PAGEDIALOG_H
