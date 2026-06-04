#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      teamAScore(0),
      teamBScore(0)
{
    ui->setupUi(this);
    updateScores();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_teamAPlusButton_clicked()
{
    ++teamAScore;
    updateScores();
}

void MainWindow::on_teamAMinusButton_clicked()
{
    --teamAScore;
    updateScores();
}

void MainWindow::on_teamBPlusButton_clicked()
{
    ++teamBScore;
    updateScores();
}

void MainWindow::on_teamBMinusButton_clicked()
{
    --teamBScore;
    updateScores();
}

void MainWindow::on_resetButton_clicked()
{
    teamAScore = 0;
    teamBScore = 0;
    updateScores();
}

void MainWindow::updateScores()
{
    ui->teamAScoreLabel->setText(QString::number(teamAScore));
    ui->teamBScoreLabel->setText(QString::number(teamBScore));
}

