#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>

class QProcess;
class QTimer;

class ProcessManager : public QObject
{
        Q_OBJECT

public:
    ProcessManager(QProcess &process);
    ~ProcessManager();

private slots:
    void start();

private:
    QProcess &m_process;
    QTimer *m_timer;
};

#endif // PROCESSMANAGER_H
