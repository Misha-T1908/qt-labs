#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage("Готово");
    ui->editor->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    bool ok = false;
    const QString name = QInputDialog::getText(this, "Новий документ", "Назва документа:", QLineEdit::Normal, "", &ok);
    if (ok) {
        setWindowTitle(name.isEmpty() ? "QuickPad" : name + " - QuickPad");
        statusBar()->showMessage("Створено новий документ", 3000);
    } else {
        statusBar()->showMessage("Скасовано", 3000);
    }
    ui->editor->setFocus();
}

void MainWindow::on_actionOpen_triggered()
{
    QMessageBox::information(this, "Відкрити", "Роботу з файлами буде додано у наступній практичній.");
    statusBar()->showMessage("Діалог відкриття закрито", 3000);
    ui->editor->setFocus();
}

void MainWindow::on_actionSave_triggered()
{
    QMessageBox::information(this, "Зберегти", "Збереження файлу буде додано у наступній практичній.");
    statusBar()->showMessage("Діалог збереження закрито", 3000);
    ui->editor->setFocus();
}

void MainWindow::on_actionSaveAs_triggered()
{
    QMessageBox::information(this, "Зберегти як", "Вибір шляху збереження буде додано у наступній практичній.");
    statusBar()->showMessage("Діалог збереження закрито", 3000);
    ui->editor->setFocus();
}

void MainWindow::on_actionExit_triggered()
{
    const auto answer = QMessageBox::question(this, "Вихід", "Закрити QuickPad?");
    if (answer == QMessageBox::Yes) {
        close();
    } else {
        statusBar()->showMessage("Скасовано", 3000);
        ui->editor->setFocus();
    }
}

void MainWindow::on_actionCut_triggered()
{
    ui->editor->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->editor->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->editor->paste();
}

void MainWindow::on_actionSelectAll_triggered()
{
    ui->editor->selectAll();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Про програму", "QuickPad\nНавчальний Qt Widgets редактор.");
    statusBar()->showMessage("Про програму", 3000);
    ui->editor->setFocus();
}
