#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QComboBox>
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
    connect(ui->actionВихід, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    scanLessons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartTraining()
{
    loadSelectedLesson();
    ui->stackedWidget->setCurrentWidget(ui->pageTraining);
    ui->pageTraining->setFocus();
}

void MainWindow::onRestartTraining()
{
    lessonModel.reset();
    updateTrainingText();
    ui->stackedWidget->setCurrentWidget(ui->pageTraining);
    ui->pageTraining->setFocus();
}

void MainWindow::onReturnToMain()
{
    ui->stackedWidget->setCurrentWidget(ui->pageStart);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "Про програму", "Тренажер набору\nНавчальний тренажер набору тексту.");
}

void MainWindow::onLessonChanged(int index)
{
    Q_UNUSED(index);
    loadSelectedLesson();
}

void MainWindow::onTestStep()
{
    lessonModel.stepForward();
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

void MainWindow::updateTrainingText()
{
    ui->previousLineLabel->setText(lessonModel.previousLine());
    ui->currentLineLabel->setText(lessonModel.currentLine());
}
