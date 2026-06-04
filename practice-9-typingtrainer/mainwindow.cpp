#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QComboBox>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSignalBlocker>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->timeValueLabel->setText("00:00");
    ui->speedValueLabel->setText("0 CPM");
    ui->accuracyValueLabel->setText("100%");
    ui->resultTimeValueLabel->setText("00:00");
    ui->resultSpeedValueLabel->setText("0 CPM");
    ui->resultAccuracyValueLabel->setText("100%");
    ui->randomLessonButton->show();
    ui->reloadLessonsButton->show();
    ui->speedMetricComboBox->hide();
    ui->speedMetricLabel->hide();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    connect(ui->restartButton, &QPushButton::clicked, this, &MainWindow::onRestartTraining);
    connect(ui->returnButton, &QPushButton::clicked, this, &MainWindow::onReturnToMain);
    connect(ui->lessonComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onLessonChanged);
    connect(ui->testStepButton, &QPushButton::clicked, this, &MainWindow::onTestStep);
    connect(ui->randomLessonButton, &QPushButton::clicked, this, &MainWindow::onRandomLesson);
    connect(ui->reloadLessonsButton, &QPushButton::clicked, this, &MainWindow::onReloadLessons);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    setupKeyboard();
    ui->pageTraining->installEventFilter(this);
    scanLessons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartTraining()
{
    loadSelectedLesson();
    lastInputWrong = false;
    clearKeyHighlight();
    ui->stackedWidget->setCurrentWidget(ui->pageTraining);
    ui->pageTraining->setFocus();
}

void MainWindow::onRestartTraining()
{
    lessonModel.reset();
    lastInputWrong = false;
    clearKeyHighlight();
    updateTrainingText();
    ui->stackedWidget->setCurrentWidget(ui->pageTraining);
    ui->pageTraining->setFocus();
}

void MainWindow::onReturnToMain()
{
    clearKeyHighlight();
    ui->stackedWidget->setCurrentWidget(ui->pageStart);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "Про програму", "TypingTrainer\nНавчальний тренажер набору тексту.");
}

void MainWindow::onLessonChanged(int index)
{
    Q_UNUSED(index);
    loadSelectedLesson();
}

void MainWindow::onTestStep()
{
    lessonModel.stepForward();
    lastInputWrong = false;
    updateTrainingText();
    ui->pageTraining->setFocus();
}

void MainWindow::onRandomLesson()
{
    const int count = ui->lessonComboBox->count();
    if (count <= 0) {
        return;
    }
    ui->lessonComboBox->setCurrentIndex(QRandomGenerator::global()->bounded(count));
}

void MainWindow::onReloadLessons()
{
    const QString currentPath = ui->lessonComboBox->currentData().toString();
    scanLessons(currentPath);
}

void MainWindow::scanLessons(const QString &preferredPath)
{
    const QSignalBlocker blocker(ui->lessonComboBox);
    ui->lessonComboBox->clear();
    lessonPaths.clear();

    QDir dir(lessonsDirectory());
    const QFileInfoList files = dir.entryInfoList({"*.txt"}, QDir::Files, QDir::Name);

    int preferredIndex = -1;
    for (const QFileInfo &fileInfo : files) {
        const QString path = fileInfo.absoluteFilePath();
        lessonPaths.append(path);
        ui->lessonComboBox->addItem(fileInfo.completeBaseName(), path);
        if (path == preferredPath) {
            preferredIndex = ui->lessonComboBox->count() - 1;
        }
    }

    const bool hasLessons = ui->lessonComboBox->count() > 0;
    ui->lessonComboBox->setEnabled(hasLessons);
    ui->startButton->setEnabled(hasLessons);
    ui->randomLessonButton->setEnabled(hasLessons);

    if (!hasLessons) {
        ui->lessonDescriptionLabel->setText("Уроки не знайдено.");
        lessonModel.setText("");
        updateTrainingText();
        return;
    }

    ui->lessonComboBox->setCurrentIndex(preferredIndex >= 0 ? preferredIndex : 0);
    loadSelectedLesson();
}

void MainWindow::loadSelectedLesson()
{
    const QString path = ui->lessonComboBox->currentData().toString();
    if (path.isEmpty()) {
        lessonModel.setText("");
    } else {
        lessonModel.setText(readLessonFile(path));
        ui->lessonDescriptionLabel->setText(QFileInfo(path).fileName());
    }
    lastInputWrong = false;
    updateTrainingText();
}

QString MainWindow::lessonsDirectory() const
{
    const QString appLessons = QDir(QCoreApplication::applicationDirPath()).filePath("lessons");
    if (QDir(appLessons).exists()) {
        return appLessons;
    }
    return QDir(QString::fromUtf8(SOURCE_DIR)).filePath("lessons");
}

QString MainWindow::readLessonFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Помилка", "Не вдалося відкрити урок:\n" + file.errorString());
        return "";
    }
    QTextStream stream(&file);
    return stream.readAll();
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->pageTraining && event->type() == QEvent::KeyPress && ui->stackedWidget->currentWidget() == ui->pageTraining) {
        handleKeyPress(static_cast<QKeyEvent *>(event));
        return true;
    }
    return QMainWindow::eventFilter(object, event);
}

void MainWindow::setupKeyboard()
{
    const auto buttons = ui->pageTraining->findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
        const QString text = button->text();
        if (text.size() == 1 || text == "Space") {
            keyboardButtons.insert(text.toUpper(), button);
        }
    }
}

void MainWindow::handleKeyPress(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Backspace) {
        lessonModel.stepBack();
        lastInputWrong = false;
        clearKeyHighlight();
        updateTrainingText();
        return;
    }

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (lessonModel.expectedChar() == QChar('\n')) {
            lessonModel.stepForward();
            lastInputWrong = false;
            updateTrainingText();
        }
        return;
    }

    const QString text = event->text();
    if (text.isEmpty()) {
        return;
    }

    const QChar input = text.at(0);
    const QChar expected = lessonModel.expectedChar();
    lastInputWrong = input != expected;
    if (!lastInputWrong) {
        lessonModel.stepForward();
    }

    highlightKey(input.isSpace() ? "SPACE" : QString(input).toUpper());
    updateTrainingText();

    if (lessonModel.isFinished()) {
        ui->stackedWidget->setCurrentWidget(ui->pageResults);
    }
}

void MainWindow::highlightKey(const QString &key)
{
    clearKeyHighlight();
    activeKey = keyboardButtons.value(key, nullptr);
    if (activeKey) {
        activeKey->setStyleSheet("background: #ffd966;");
    }
}

void MainWindow::clearKeyHighlight()
{
    if (activeKey) {
        activeKey->setStyleSheet("");
        activeKey = nullptr;
    }
}

void MainWindow::updateTrainingText()
{
    ui->previousLineLabel->setText("<span style='color:#777;'>" + lessonModel.previousLine().toHtmlEscaped() + "</span>");

    const QString typed = lessonModel.typedPart().toHtmlEscaped();
    QString remaining = lessonModel.remainingPart();
    QString current;

    if (remaining.isEmpty()) {
        current = "<span style='background:#c8e6c9;'>" + typed + "</span>";
    } else {
        const QString first = remaining.left(1).toHtmlEscaped();
        const QString rest = remaining.mid(1).toHtmlEscaped();
        const QString color = lastInputWrong ? "#ffcdd2" : "#fff59d";
        current = "<span style='background:#c8e6c9;'>" + typed + "</span>"
                  "<span style='background:" + color + ";'>" + first + "</span>" + rest;
    }

    ui->currentLineLabel->setText(current);
}
