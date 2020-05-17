#ifndef KEYRECEIVER_H
#define KEYRECEIVER_H

#include <QObject>
#include "mainwindow.h"

//Helper class to get key events from mainWindow

class KeyReceiver: public QObject
{
    Q_OBJECT

public:
    KeyReceiver(MainWindow *window);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    MainWindow *m_window;
};

#endif // KEYRECEIVER_H
