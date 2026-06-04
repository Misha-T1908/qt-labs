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
#include <QSettings>
#include <QSignalBlocker>
#include <QTimer>
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
    ui->testStepButton->hide();
    ui->speedMetricComboBox->show();
    ui->speedMetricLabel->show();
    sessionTimer = new QTimer(this);
    sessionTimer->setInterval(1000);

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    connect(ui->restartButton, &QPushButton::clicked, this, &MainWindow::onRestartTraining);
    connect(ui->returnButton, &QPushButton::clicked, this, &MainWindow::onReturnToMain);
    connect(ui->lessonComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onLessonChanged);
    connect(ui->testStepButton, &QPushButton::clicked, this, &MainWindow::onTestStep);
    connect(ui->randomLessonButton, &QPushButton::clicked, this, &MainWindow::onRandomLesson);
    connect(ui->reloadLessonsButton, &QPushButton::clicked, this, &MainWindow::onReloadLessons);
    connect(ui->speedMetricComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onSpeedMetricChanged);
    connect(sessionTimer, &QTimer::timeout, this, &MainWindow::onTimerTick);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    setupKeyboard();
    ui->pageTraining->installEventFilter(this);
    loadSettings();
    scanLessons(savedLessonPath);
    resetMetrics();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartTraining()
{
    loadSelectedLesson();
    startSession();
    ui->stackedWidget->setCurrentWidget(ui->pageTraining);
    ui->pageTraining->setFocus();
}

void MainWindow::onRestartTraining()
{
    lessonModel.reset();
    startSession();
    ui->stackedWidget->setCurrentWidget(ui->pageTraining);
    ui->pageTraining->setFocus();
}

void MainWindow::onReturnToMain()
{
    stopSession();
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
    saveSettings();
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

void MainWindow::onSpeedMetricChanged()
{
    updateMetrics();
    saveSettings();
}

void MainWindow::onTimerTick()
{
    updateMetrics();
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
    saveSettings();
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
    saveSettings();
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

void MainWindow::loadSettings()
{
    QSettings settings("QtLabs", "TypingTrainer");
    savedLessonPath = settings.value("lesson/path").toString();
    const QString metric = settings.value("speed/metric", "CPM").toString();
    const int metricIndex = ui->speedMetricComboBox->findText(metric);
    if (metricIndex >= 0) {
        ui->speedMetricComboBox->setCurrentIndex(metricIndex);
    }
}

void MainWindow::saveSettings()
{
    QSettings settings("QtLabs", "TypingTrainer");
    settings.setValue("lesson/path", ui->lessonComboBox->currentData().toString());
    settings.setValue("speed/metric", ui->speedMetricComboBox->currentText());
}

void MainWindow::startSession()
{
    resetMetrics();
    lastInputWrong = false;
    clearKeyHighlight();
    updateTrainingText();
    elapsedTimer.start();
    sessionTimer->start();
    updateMetrics();
}

void MainWindow::stopSession()
{
    if (sessionTimer->isActive()) {
        sessionTimer->stop();
    }
}

void MainWindow::resetMetrics()
{
    finalElapsedMs = 0;
    totalInput = 0;
    correctInput = 0;
    ui->timeValueLabel->setText("00:00");
    ui->speedValueLabel->setText(formattedSpeed(0.0));
    ui->accuracyValueLabel->setText("100%");
}

void MainWindow::finishSession()
{
    finalElapsedMs = elapsedTimer.isValid() ? elapsedTimer.elapsed() : 0;
    stopSession();
    updateMetrics();
    ui->resultTimeValueLabel->setText(formattedTime(finalElapsedMs));
    ui->resultSpeedValueLabel->setText(formattedSpeed(currentSpeed()));
    ui->resultAccuracyValueLabel->setText(ui->accuracyValueLabel->text());
    ui->stackedWidget->setCurrentWidget(ui->pageResults);
}

void MainWindow::updateMetrics()
{
    const qint64 ms = finalElapsedMs > 0 ? finalElapsedMs : (elapsedTimer.isValid() ? elapsedTimer.elapsed() : 0);
    ui->timeValueLabel->setText(formattedTime(ms));
    ui->speedValueLabel->setText(formattedSpeed(currentSpeed()));

    const double accuracy = totalInput > 0 ? (100.0 * correctInput / totalInput) : 100.0;
    ui->accuracyValueLabel->setText(QString::number(accuracy, 'f', 1) + "%");
}

QString MainWindow::formattedTime(qint64 ms) const
{
    const qint64 seconds = ms / 1000;
    const qint64 minutes = seconds / 60;
    const qint64 rest = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(rest, 2, 10, QChar('0'));
}

QString MainWindow::formattedSpeed(double speed) const
{
    return QString::number(speed, 'f', 1) + " " + ui->speedMetricComboBox->currentText();
}

double MainWindow::currentSpeed() const
{
    const qint64 ms = finalElapsedMs > 0 ? finalElapsedMs : (elapsedTimer.isValid() ? elapsedTimer.elapsed() : 0);
    if (ms <= 0 || totalInput <= 0) {
        return 0.0;
    }

    const double minutes = ms / 60000.0;
    const double cpm = totalInput / minutes;
    if (ui->speedMetricComboBox->currentText() == "WPM") {
        return cpm / 5.0;
    }
    return cpm;
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
            ++totalInput;
            ++correctInput;
            lessonModel.stepForward();
            lastInputWrong = false;
            updateTrainingText();
            updateMetrics();
            if (lessonModel.isFinished()) {
                finishSession();
            }
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
    ++totalInput;
    if (!lastInputWrong) {
        ++correctInput;
        lessonModel.stepForward();
    }

    highlightKey(input.isSpace() ? "SPACE" : QString(input).toUpper());
    updateTrainingText();
    updateMetrics();

    if (lessonModel.isFinished()) {
        finishSession();
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
