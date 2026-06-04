#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lessonComboBox->addItem("Базовий урок");
    ui->lessonDescriptionLabel->setText("Короткий урок для тренування набору.");
    ui->timeValueLabel->setText("00:00");
    ui->speedValueLabel->setText("0 CPM");
    ui->accuracyValueLabel->setText("100%");
    ui->previousLineLabel->setText("");
    ui->currentLineLabel->setText("Текст уроку буде показано тут.");
    ui->resultTimeValueLabel->setText("00:00");
    ui->resultSpeedValueLabel->setText("0 CPM");
    ui->resultAccuracyValueLabel->setText("100%");
    ui->testStepButton->hide();
    ui->randomLessonButton->hide();
    ui->reloadLessonsButton->hide();
    ui->speedMetricComboBox->hide();
    ui->speedMetricLabel->hide();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    connect(ui->restartButton, &QPushButton::clicked, this, &MainWindow::onRestartTraining);
    connect(ui->returnButton, &QPushButton::clicked, this, &MainWindow::onReturnToMain);
    connect(ui->actionВихід, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartTraining()
{
    ui->stackedWidget->setCurrentWidget(ui->pageTraining);
    ui->pageTraining->setFocus();
}

void MainWindow::onRestartTraining()
{
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
