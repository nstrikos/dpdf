#include "keyReceiver.h"

#include <QEvent>
#include <QKeyEvent>

KeyReceiver::KeyReceiver(MainWindow *window)
{
    m_window = window;
}

bool KeyReceiver::eventFilter(QObject* obj, QEvent* event)
{
    Q_UNUSED(obj);

    if (event->type() == QEvent::KeyPress) {
        m_window->getFocus();

        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if ( (key->key() == Qt::Key_Up) )
            m_window->keyUpPressed();
        else if (key->key() == Qt::Key_Down)
            m_window->keyDownPressed();
    }

    return false;
}

