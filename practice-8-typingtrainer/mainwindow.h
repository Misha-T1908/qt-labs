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
    void onRandomLesson();
    void onReloadLessons();

private:
    void scanLessons(const QString &preferredPath = QString());
    void loadSelectedLesson();
    QString lessonsDirectory() const;
    QString readLessonFile(const QString &path);
    void updateTrainingText();

    Ui::MainWindow *ui;
    LessonModel lessonModel;
    QStringList lessonPaths;
};

#endif
