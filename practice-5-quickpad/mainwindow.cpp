#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QMessageBox>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      modified(false)
{
    ui->setupUi(this);

    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSaveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));
    ui->actionExit->setShortcut(QKeySequence("Ctrl+Q"));
    ui->actionCut->setShortcut(QKeySequence::Cut);
    ui->actionCopy->setShortcut(QKeySequence::Copy);
    ui->actionPaste->setShortcut(QKeySequence::Paste);
    ui->actionSelectAll->setShortcut(QKeySequence::SelectAll);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::exitApp);
    connect(ui->actionCut, &QAction::triggered, this, &MainWindow::cutText);
    connect(ui->actionCopy, &QAction::triggered, this, &MainWindow::copyText);
    connect(ui->actionPaste, &QAction::triggered, this, &MainWindow::pasteText);
    connect(ui->actionSelectAll, &QAction::triggered, this, &MainWindow::selectAllText);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    connect(ui->editor, &QPlainTextEdit::textChanged, this, &MainWindow::markModified);
    connect(ui->editor, &QPlainTextEdit::copyAvailable, this, &MainWindow::updateEditActions);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::updateEditActions);

    updateWindowTitle();
    updateEditActions();
    statusBar()->showMessage("Готово");
    ui->editor->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (!maybeSave()) {
        statusBar()->showMessage("Скасовано", 3000);
        return;
    }

    {
        QSignalBlocker blocker(ui->editor);
        ui->editor->clear();
    }
    currentPath.clear();
    modified = false;
    updateWindowTitle();
    statusBar()->showMessage("Створено новий документ", 3000);
    ui->editor->setFocus();
}

void MainWindow::openFile()
{
    if (!maybeSave()) {
        statusBar()->showMessage("Скасовано", 3000);
        return;
    }

    const QString path = QFileDialog::getOpenFileName(this, "Відкрити файл", QString(), "Text files (*.txt);;All files (*.*)");
    if (path.isEmpty()) {
        statusBar()->showMessage("Скасовано", 3000);
        ui->editor->setFocus();
        return;
    }

    loadFromPath(path);
    ui->editor->setFocus();
}

bool MainWindow::saveFile()
{
    if (currentPath.isEmpty()) {
        return saveFileAs();
    }
    return saveToPath(currentPath);
}

bool MainWindow::saveFileAs()
{
    const QString path = QFileDialog::getSaveFileName(this, "Зберегти файл", currentPath, "Text files (*.txt);;All files (*.*)");
    if (path.isEmpty()) {
        statusBar()->showMessage("Скасовано", 3000);
        ui->editor->setFocus();
        return false;
    }

    return saveToPath(path);
}

void MainWindow::exitApp()
{
    close();
}

void MainWindow::cutText()
{
    ui->editor->cut();
    statusBar()->showMessage("Вирізано", 3000);
    ui->editor->setFocus();
}

void MainWindow::copyText()
{
    ui->editor->copy();
    statusBar()->showMessage("Скопійовано", 3000);
    ui->editor->setFocus();
}

void MainWindow::pasteText()
{
    ui->editor->paste();
    statusBar()->showMessage("Вставлено", 3000);
    ui->editor->setFocus();
}

void MainWindow::selectAllText()
{
    ui->editor->selectAll();
    statusBar()->showMessage("Виділено все", 3000);
    updateEditActions();
    ui->editor->setFocus();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "Про програму", "QuickPad\nНавчальний текстовий редактор на Qt Widgets.");
    statusBar()->showMessage("Про програму", 3000);
    ui->editor->setFocus();
}

void MainWindow::markModified()
{
    modified = true;
    updateWindowTitle();
}

void MainWindow::updateEditActions()
{
    const bool hasSelection = ui->editor->textCursor().hasSelection();
    const bool hasClipboardText = QApplication::clipboard()->mimeData()->hasText();
    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);
    ui->actionPaste->setEnabled(hasClipboardText);
}

bool MainWindow::maybeSave()
{
    if (!modified) {
        return true;
    }

    const auto answer = QMessageBox::warning(
        this,
        "Незбережені зміни",
        "Документ має незбережені зміни. Зберегти їх?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (answer == QMessageBox::Save) {
        return saveFile();
    }
    if (answer == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

bool MainWindow::saveToPath(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка", "Не вдалося зберегти файл:\n" + file.errorString());
        statusBar()->showMessage("Помилка збереження", 3000);
        return false;
    }

    QTextStream stream(&file);
    stream << ui->editor->toPlainText();
    setCurrentPath(path);
    modified = false;
    updateWindowTitle();
    statusBar()->showMessage("Збережено", 3000);
    ui->editor->setFocus();
    return true;
}

void MainWindow::loadFromPath(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка", "Не вдалося відкрити файл:\n" + file.errorString());
        statusBar()->showMessage("Помилка відкриття", 3000);
        return;
    }

    QTextStream stream(&file);
    {
        QSignalBlocker blocker(ui->editor);
        ui->editor->setPlainText(stream.readAll());
    }
    setCurrentPath(path);
    modified = false;
    updateWindowTitle();
    statusBar()->showMessage("Відкрито", 3000);
}

void MainWindow::setCurrentPath(const QString &path)
{
    currentPath = path;
}

void MainWindow::updateWindowTitle()
{
    const QString name = currentPath.isEmpty() ? "Без назви" : QFileInfo(currentPath).fileName();
    setWindowTitle((modified ? "* " : "") + name + " - QuickPad");
}
