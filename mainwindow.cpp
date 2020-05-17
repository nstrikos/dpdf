#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QTextBlock>
#include <QtTest>

#include "keyReceiver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();

    m_process = new QProcess;
    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessOutput()));

    m_text = "";
    m_contents = "";
    m_input = "";

    m_curpage = 1;
    m_curLine = 1;
    m_numPages = 0;
    m_filename = "";
    m_moveBackwards = false;

    keyReceiver = new KeyReceiver(this);
    ui->plainTextEdit->installEventFilter(keyReceiver);
}

MainWindow::~MainWindow()
{
    m_process->terminate();
    m_process->waitForFinished();
    delete m_process;

    delete keyReceiver;

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

    handleFilenameQuotes(filename);

    ui->plainTextEdit->clear();
    ui->label->clear();
    ui->label_2->clear();
    m_contents.clear();

    m_curpage = 1;
    m_numPages = 0;

    m_input = "";
    m_contents = "";
    m_text = "";
    startProcess();

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
    //    command = "java -Dfile.encoding=UTF-8 -classpath /media/data/nick/projects/eclipse-workspace/pdfbox/bin:/media/data/nick/projects/eclipse-workspace/pdfbox/lib/fontbox-2.0.19.jar:/media/data/nick/projects/eclipse-workspace/pdfbox/lib/pdfbox-2.0.19.jar:/media/data/nick/projects/eclipse-workspace/pdfbox/lib/pdfbox-app-2.0.19.jar:/media/data/nick/projects/eclipse-workspace/pdfbox/lib/pdfbox-tools-2.0.19.jar:/media/data/nick/projects/eclipse-workspace/pdfbox/lib/preflight-2.0.19.jar:/media/data/nick/projects/eclipse-workspace/pdfbox/lib/xmpbox-2.0.19.jar pdfbox.PageReader";
    command = "java -Dfile.encoding=UTF-8 -jar /home/nick/projects/dpdf/pdfbox/lib/dpdf.jar";
#endif

#ifdef Q_OS_WIN
    //    command = "C:\\Users\\Nick\\projects\\jdk-12.0.1\\bin\\javaw.exe -Dfile.encoding=Cp1253 -classpath \"C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\bin;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\fontbox-2.0.19.jar;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\\pdfbox-2.0.19.jar;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\\pdfbox-app-2.0.19.jar;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\\pdfbox-tools-2.0.19.jar;C:\\Users\\Nick\\eclipse-workspace\\pdfbox\\lib\\preflight-2.0.19.jar;C:\\Users\\Nick\eclipse-workspace\\pdfbox\\lib\\xmpbox-2.0.19.jar\" pdfbox.PageReader";
    command = "C:\\Users\\Nick\\projects\\jdk-12.0.1\\bin\\java.exe -Dfile.encoding=UTF-8 -jar C:\\Users\\Nick\\Desktop\\dpdf\\pdfbox\\lib\\dpdf.jar";
#endif

    command += " " + m_filename;
    m_process->terminate();
    m_process->waitForFinished();
    m_process->start(command.toUtf8());
}

void MainWindow::readContents()
{

}

void MainWindow::readProcessOutput()
{
    QString input = m_process->readAllStandardOutput();
    m_input.append(input);
    if (m_input.contains("@pagereader finished contents@")) {
        int pos = m_input.lastIndexOf("@pagereader finished contents@");
        QString tmp = m_input.left(pos);
        int pos2 = tmp.lastIndexOf("\n");
        m_contents = tmp.left(pos2);
        QString numPages = tmp.right(tmp.size() - pos2);
        ui->label_2->setText(numPages);
        m_numPages = numPages.toInt();
        m_input = "";
        setPage(1);
    } else if (m_input.contains("#end of page#"))
        readPage();

    getFocus();
}

void MainWindow::readPage()
{
    int pos = m_input.indexOf("#end of page#");
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

void MainWindow::setPage(int page)
{
    ui->label->setText(QString::number(page));

    m_input = "";

    //send page number to process
    QString str1 = QString::number(page) + "\n";

    QByteArray ba = str1.toLocal8Bit();
    const char *c_str2 = ba.data();
    m_process->write(c_str2);
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
}

void MainWindow::keyUpPressed()
{
    m_moveBackwards = true;
    if (m_curLine == 1) {
        m_curpage--;
        if (m_curpage < 1)
            m_curpage = 1;
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
