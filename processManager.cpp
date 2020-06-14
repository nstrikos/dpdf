#include "processManager.h"

#include <QProcess>
#include <QTimer>

ProcessManager::ProcessManager(QProcess &process) :
    m_process(process)
{
    start();
    m_timer = new QTimer();
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &ProcessManager::start);
    m_timer->start();
}

ProcessManager::~ProcessManager()
{
    delete m_timer;
}

void ProcessManager::start()
{
    if (m_process.pid() != 0)
        return;

    QString command;
    QStringList arguments;


#ifdef Q_OS_LINUX
    command = "java";
    arguments << "-Dfile.encoding=UTF-8";
    arguments << "-jar";
    arguments << "/home/nick/dpdf.jar";
#endif

#ifdef Q_OS_WIN
    command = "C:\\Users\\Nick\\projects\\jdk-12.0.1\\bin\\java.exe";
    arguments << "-Dfile.encoding=UTF-8";
    arguments << "-jar";
    arguments << "C:\\Users\\Nick\\Desktop\\dpdf\\lib\\dpdf.jar";
#endif

    m_process.start(command.toUtf8(), arguments);
}
