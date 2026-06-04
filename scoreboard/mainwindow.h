#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_teamAPlusButton_clicked();
    void on_teamAMinusButton_clicked();
    void on_teamBPlusButton_clicked();
    void on_teamBMinusButton_clicked();
    void on_resetButton_clicked();

private:
    void updateScores();

    Ui::MainWindow *ui;
    int teamAScore;
    int teamBScore;
};

#endif

