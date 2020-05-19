#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QProcess;
class KeyReceiver;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void keyUpPressed();
    void keyDownPressed();
    void getFocus();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void open();
    void about();
    void readProcessOutput();

    void previousPage();
    void nextPage();
    void showContents();

    void setPage(int page);
    void sendOpen();

    void on_plainTextEdit_cursorPositionChanged();

private:
    Ui::MainWindow *ui;

    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QMenu *goToMenu;
    QMenu *helpMenu;

    QAction *openAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *previousPageAct;
    QAction *nextPageAct;
    QAction *showContentsAct;

    QProcess *m_process;
    KeyReceiver *keyReceiver;

    QString m_filename;
    QString m_text;
    QString m_contents;
    QString m_input;
    int m_curpage;
    int m_numPages;
    int m_curLine;
    bool m_moveBackwards;

    void handleFilenameQuotes(QString file);
    void startProcess();
    void readContents();
    void readPage();
    void moveCursorToLastLine();
    void getCurrentLine();
 };
#endif // MAINWINDOW_H
