#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QTextBlock>
#include <QtTest>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include "keyReceiver.h"
#include "contentsDialog.h"
#include "pageDialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();

    m_process = new QProcess;
    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessOutput()));
    startProcess();

    m_text = "";
    m_contents = "";
    m_input = "";

    m_curpage = 1;
    m_curLine = 1;
    m_numPages = 0;
    m_filename = "";
    m_position = "";
    m_moveBackwards = false;

    player = new QMediaPlayer();
    player->setMedia(QUrl(""));
    playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl("qrc:/resources/109663__grunz__success-low.wav"));
    playlist->setCurrentIndex(1);
    player->setPlaylist(playlist);
    m_loading = false;

    m_titles.clear();
    m_pages.clear();

    keyReceiver = new KeyReceiver(this);
    ui->plainTextEdit->installEventFilter(keyReceiver);

    contentsDialog = new ContentsDialog();
    contentsDialog->setModal(true);
    connect(contentsDialog, &ContentsDialog::contentSelected, this, &MainWindow::contentSelected);

    pageDialog = new PageDialog();
    pageDialog->setModal(true);
    connect(pageDialog, &PageDialog::goToPage, this, &MainWindow::setPage);
}

MainWindow::~MainWindow()
{
#ifdef Q_OS_WIN
    sendQuit();
#endif
#ifdef Q_OS_LINUX
    m_process->terminate();
#endif
    m_process->waitForFinished();
    delete m_process;

    delete playlist;
    delete player;

    delete keyReceiver;
    delete contentsDialog;

    delete ui;
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    previousPageAct = new QAction(tr("&Previous page"), this);
    previousPageAct->setShortcut(Qt::Key_F7);
    connect(previousPageAct, &QAction::triggered, this, &MainWindow::previousPage);

    nextPageAct = new QAction(tr("&Next page"), this);
    nextPageAct->setShortcut(Qt::Key_F8);
    connect(nextPageAct, &QAction::triggered, this, &MainWindow::nextPage);

    goToPageAct = new QAction(tr("&Go to page..."), this);
    goToPageAct->setShortcut(Qt::Key_F3);
    connect(goToPageAct, &QAction::triggered, this, &MainWindow::goToPage);

    showContentsAct = new QAction(tr("&Show contents"), this);
    showContentsAct->setShortcut(Qt::Key_F4);
    connect(showContentsAct, &QAction::triggered, this, &MainWindow::showContents);

    positionAct = new QAction(tr("&Show Position"), this);
    positionAct->setShortcut(Qt::Key_F12);
    connect(positionAct, &QAction::triggered, this, &MainWindow::showPosition);

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    goToMenu = menuBar()->addMenu(tr("&Go to"));
    goToMenu->addAction(previousPageAct);
    goToMenu->addAction(nextPageAct);
    goToMenu->addAction(goToPageAct);
    goToMenu->addAction(showContentsAct);
    goToMenu->addAction(positionAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Menu"),
                       tr("The <b>Menu</b> example shows how to create "
                          "menu-bar menus and context menus."));
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(openAct);
    menu.exec(event->globalPos());
}

void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Document"), "",
                                                    tr("PDF document (*.pdf);;All Files (*)"));

    if (filename.isEmpty())
        return;

    //    handleFilenameQuotes(filename);
    m_filename = filename;

    ui->plainTextEdit->clear();
    //    ui->label->clear();
    //    ui->label_2->clear();
    m_contents.clear();

    m_curpage = 1;
    m_numPages = 0;

    m_input = "";
    m_contents = "";
    m_text = "";
    m_position = "";
    m_titles.clear();
    m_pages.clear();
    m_moveBackwards = false;
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    cursor.setPosition(0);

    playlist->setCurrentIndex(1);
    m_loading = true;

    sendOpen();

    //Hack to make screen reader read textEdit
    getFocus();
}

void MainWindow::handleFilenameQuotes(QString file)
{
    QString tmpFilename = "";

    if (!file.contains("\"")) {
        tmpFilename = "\"" + file + "\"";
    } else {
        QStringList list = file.split("\"");
        for (int i = 0; i < list.size(); i++) {
            list[i] = "\"" + list[i] + "\"";
        }

        for (int i = 0; i < list.size(); i++) {
            tmpFilename.append(" " + list.at(i));
        }
    }
    m_filename = tmpFilename;
}

void MainWindow::startProcess()
{
    QString command;

#ifdef Q_OS_LINUX
    //    command = "/usr/lib/jvm/java-11-openjdk-amd64/bin/java -Dfile.encoding=UTF-8 -classpath /home/nick/projects/eclipse-workspace/dpdf/bin:/home/nick/projects/eclipse-workspace/dpdf/src/dpdf/lib/fontbox-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/dpdf/lib/pdfbox-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/dpdf/lib/pdfbox-app-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/dpdf/lib/pdfbox-tools-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/dpdf/lib/preflight-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/dpdf/lib/xmpbox-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/lib/fontbox-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/lib/pdfbox-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/lib/pdfbox-app-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/lib/pdfbox-tools-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/lib/preflight-2.0.19.jar:/home/nick/projects/eclipse-workspace/dpdf/src/lib/xmpbox-2.0.19.jar dpdf.PageReader";
    command = "java -Dfile.encoding=UTF-8 -jar /home/nick/projects/dpdf/lib/dpdf.jar";
#endif

#ifdef Q_OS_WIN
    //    command = "C:\\Users\\Nick\\projects\\jdk-12.0.1\\bin\\javaw.exe -Dfile.encoding=Cp1253 -classpath \"C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\bin;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\fontbox-2.0.19.jar;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\\pdfbox-2.0.19.jar;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\\pdfbox-app-2.0.19.jar;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\\pdfbox-tools-2.0.19.jar;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\\preflight-2.0.19.jar;C:\\Users\\Nick\eclipse-workspace\\pdfbox\\lib\\xmpbox-2.0.19.jar\" pdfbox.PageReader";
    command = "C:\\Users\\Nick\\projects\\jdk-12.0.1\\bin\\java.exe -Dfile.encoding=UTF-8 -jar C:\\Users\\Nick\\Desktop\\dpdf\\lib\\dpdf.jar";
#endif

    //    command += " " + m_filename;
    m_process->terminate();
    m_process->waitForFinished();
    m_process->start(command.toUtf8());
}

void MainWindow::readContents(QString input)
{
    QString text = input.replace("@pageReader finished contents@\n", "");
    QString line = "";
    QString string1 = "@pageReader@";
#ifdef Q_OS_WIN
    QString string2 = "@pageReader end page@\r\n";
#endif
#ifdef Q_OS_LINUX
    QString string2 = "@pageReader end page@\n";
#endif
    QString title = "";
    QString page = "";
    int n, k, l;

    m_titles.clear();
    m_pages.clear();

    QStringList list = text.split(string2);
    for (int i = 0; i < list.size() - 1; i++) { //last string in list is empty, so we don't process it
        line = list.at(i);
        n = line.indexOf(string1);
        title = line.left(n);
        k = string1.size();
        l = line.size();
        page = line.mid(n+k, l - n - k);
        m_titles.append(title + " page " + page);
        m_pages.append(page.toInt());
    }

    contentsDialog->setContents(&m_titles);
}

void MainWindow::readProcessOutput()
{
    QString input = m_process->readAllStandardOutput();
    //    qDebug() << input;
    m_input.append(input);
    if (m_input.contains("@pageReader number of pages@")) {
        int pos = m_input.lastIndexOf("@pageReader number of pages@");
        QString tmp = m_input.left(pos);
        int pos2 = tmp.lastIndexOf("\n");
        m_contents = tmp.left(pos2);
        QString numPages = tmp.right(tmp.size() - pos2);
        //        ui->label_2->setText(numPages);
        m_numPages = numPages.toInt();
        m_input = "";
        setPage(1);
    } else if (m_input.contains("@pageReader end of page@")) {
        readPage();
    } else if (m_input.contains("@pageReader finished contents@")) {
        readContents(m_input);
    }

    getFocus();
    if (m_loading) {
        m_loading = false;
        playlist->setCurrentIndex(1);
        player->play();
    }
}

void MainWindow::readPage()
{
    int pos = m_input.indexOf("@pageReader end of page@");
    m_text = m_input.left(pos);
    m_text = m_text.trimmed();
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setPlainText(m_text);

    if (m_moveBackwards)
        moveCursorToLastLine();
}

void MainWindow::moveCursorToLastLine()
{
    QTextCursor cursor = ui->plainTextEdit->textCursor();
#ifdef Q_OS_LINUX
    cursor.setPosition(m_text.size());
#endif
#ifdef Q_OS_WIN
    cursor.setPosition(ui->plainTextEdit->toPlainText().size());
#endif
    ui->plainTextEdit->setTextCursor(cursor);
}

void MainWindow::previousPage()
{
    //Hack on linux to make screen reader read ext
    QTest::keyClick(ui->plainTextEdit, Qt::Key_Home);
    m_moveBackwards = false;
    m_curpage--;
    if (m_curpage < 1)
        m_curpage = 1;

    setPage(m_curpage);
}

void MainWindow::nextPage()
{
    //Hack on linux to make screen reader read ext
    QTest::keyClick(ui->plainTextEdit, Qt::Key_Home);
    m_moveBackwards = false;
    m_curpage++;
    if (m_curpage > m_numPages) {
        m_curpage = m_numPages;
        return;
    }

    setPage(m_curpage);
}

void MainWindow::showContents()
{
    m_input = "";
    contentsDialog->clear();

    //send page number to process
    QString str1 = "contents\n";

    QByteArray ba = str1.toLocal8Bit();
    const char *c_str2 = ba.data();
    m_process->write(c_str2);
    contentsDialog->show();
}

void MainWindow::contentSelected(int i)
{
    setPage(m_pages.at(i));
}

void MainWindow::setPage(int page)
{
    if (page > 0 && page <= m_numPages) {
        //        ui->label->setText(QString::number(page));

        m_input = "";
        m_curpage = page;

        //send page number to process
        QString str1 = "page " + QString::number(page) + "\n";

        QByteArray ba = str1.toLocal8Bit();
        const char *c_str2 = ba.data();
        m_process->write(c_str2);
    } else {
        QMessageBox msgBox;
        QString text = tr("You must select a page between 1 and ") + QString::number(m_numPages);
        msgBox.setText(text);
        msgBox.setAccessibleName(text);
        msgBox.exec();
    }
}

void MainWindow::sendOpen()
{
    m_input = "";

    //send page number to process
    QString str1 = "open " + m_filename + "\n";

    QByteArray ba = str1.toLocal8Bit();
    const char *c_str2 = ba.data();
    m_process->write(c_str2);
}

void MainWindow::goToPage()
{
    pageDialog->show();
}

void MainWindow::showPosition()
{
    if (m_position != "") {
        QMessageBox msgBox;
        msgBox.setText(m_position);
        msgBox.setAccessibleName(m_position);
        msgBox.exec();
    }
}

void MainWindow::on_plainTextEdit_cursorPositionChanged()
{
    getCurrentLine();
}

void MainWindow::getCurrentLine()
{
    QPlainTextEdit *edit = qobject_cast<QPlainTextEdit *>(sender());
    Q_ASSERT(edit);
    QTextCursor cursor = edit->textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);

    int lines = 1;
    while(cursor.positionInBlock()>0) {
        cursor.movePosition(QTextCursor::Up);
        lines++;
    }
    QTextBlock block = cursor.block().previous();

    while(block.isValid()) {
        lines += block.lineCount();
        block = block.previous();
    }
    m_curLine = lines;

    m_position = tr("Line: ") + QString::number(m_curLine) + " "
            + tr("Page: ") + QString::number(m_curpage) + " "
            + tr("of") + " " + QString::number(m_numPages);

    ui->statusbar->showMessage(m_position);
}

void MainWindow::sendQuit()
{
    QString str1 = "quit\n";

    QByteArray ba = str1.toLocal8Bit();
    const char *c_str2 = ba.data();
    m_process->write(c_str2);
}

void MainWindow::keyUpPressed()
{
    m_moveBackwards = true;
    if (m_curLine == 1) {
        m_curpage--;
        if (m_curpage < 1) {
            m_curpage = 1;
            return;
        }
        setPage(m_curpage);
    }
}

void MainWindow::keyDownPressed()
{
    m_moveBackwards = false;
    if (m_curpage < m_numPages)
        if (m_curLine == ui->plainTextEdit->document()->lineCount())
            nextPage();
}

void MainWindow::getFocus()
{
    menuBar()->setFocus();
    ui->plainTextEdit->setFocus();
}
