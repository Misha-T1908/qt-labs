#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QComboBox>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupLessons();
    ui->timeValueLabel->setText("00:00");
    ui->speedValueLabel->setText("0 CPM");
    ui->accuracyValueLabel->setText("100%");
    ui->resultTimeValueLabel->setText("00:00");
    ui->resultSpeedValueLabel->setText("0 CPM");
    ui->resultAccuracyValueLabel->setText("100%");
    ui->randomLessonButton->hide();
    ui->reloadLessonsButton->hide();
    ui->speedMetricComboBox->hide();
    ui->speedMetricLabel->hide();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    connect(ui->restartButton, &QPushButton::clicked, this, &MainWindow::onRestartTraining);
    connect(ui->returnButton, &QPushButton::clicked, this, &MainWindow::onReturnToMain);
    connect(ui->lessonComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onLessonChanged);
    connect(ui->testStepButton, &QPushButton::clicked, this, &MainWindow::onTestStep);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    loadSelectedLesson();
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
    updateTrainingText();
    ui->pageTraining->setFocus();
}

void MainWindow::setupLessons()
{
    lessonTexts = {
        "Simple words for typing.\\nKeep your hands relaxed.\\nPractice every day.",
        "Qt Widgets are useful for desktop apps.\\nSignals and slots connect actions.\\nLayouts keep windows adaptive.",
        "Short text.\\nA longer line with commas, spaces, and dots.\\nEnd."
    };

    ui->lessonComboBox->clear();
    ui->lessonComboBox->addItem("Урок 1");
    ui->lessonComboBox->addItem("Урок 2");
    ui->lessonComboBox->addItem("Урок 3");
    ui->lessonDescriptionLabel->setText("Оберіть урок і натисніть Start training.");
}

void MainWindow::loadSelectedLesson()
{
    const int index = ui->lessonComboBox->currentIndex();
    if (index < 0 || index >= lessonTexts.size()) {
        lessonModel.setText("");
    } else {
        lessonModel.setText(lessonTexts.at(index));
    }
    updateTrainingText();
}

void MainWindow::updateTrainingText()
{
    ui->previousLineLabel->setText(lessonModel.previousLine());
    ui->currentLineLabel->setText(lessonModel.currentLine());
}
