#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lessonmodel.h"

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
    void onStartTraining();
    void onRestartTraining();
    void onReturnToMain();
    void onAbout();
    void onLessonChanged(int index);
    void onTestStep();

private:
    void setupLessons();
    void loadSelectedLesson();
    void updateTrainingText();

    Ui::MainWindow *ui;
    LessonModel lessonModel;
    QStringList lessonTexts;
};

#endif
