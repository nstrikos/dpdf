#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QProcess;
class ProcessManager;
class KeyReceiver;
class ContentsDialog;
class PageDialog;
class QMediaPlayer;
class QMediaPlaylist;

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
    void openRecentFile();

    void about();
    void readProcessOutput();

    void previousPage();
    void nextPage();
    void showContents();
    void contentSelected(int i);
    void setPage(int page);
    void sendOpen();
    void goToPage();
    void showPosition();
    void addBookmark();

    void on_plainTextEdit_cursorPositionChanged();

private:
    Ui::MainWindow *ui;

    void createActions();
    void createMenus();
    void readSettings();
    void writeSettings();

    QMenu *fileMenu;
    QMenu *goToMenu;
    QMenu *helpMenu;

    QAction *openAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *previousPageAct;
    QAction *nextPageAct;
    QAction *showContentsAct;
    QAction *goToPageAct;
    QAction *positionAct;
    QAction *bookmarkAct;

    QProcess *m_process;
    ProcessManager *m_processManager;
    KeyReceiver *keyReceiver;
    ContentsDialog *contentsDialog;
    PageDialog *pageDialog;

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    bool m_loading;

    QString m_filename;
    QString m_text;
    QString m_contents;
    QString m_input;
    int m_curpage;
    int m_numPages;
    int m_curLine;
    int m_savedLine;
    bool m_moveBackwards;
    QVector<QString> m_titles;
    QVector<int> m_pages;
    QString m_position;
    bool m_loadingDocument;

    void openFile(QString filename);

    void handleFilenameQuotes(QString file);
    void readContents(QString input);
    void readPage();
    void moveCursorToLastLine();
    void getCurrentLine();
    void goToLine(int line);
    void sendQuit();

    void updateRecentFileActions();
    enum {MaxRecentFiles = 15};
    QStringList recentFiles;
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;
    QString strippedName(const QString &fileName);    
};
#endif // MAINWINDOW_H
