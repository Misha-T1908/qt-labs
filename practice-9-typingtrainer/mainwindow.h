#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lessonmodel.h"

#include <QMap>
#include <QMainWindow>

class QPushButton;

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

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

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
    void setupKeyboard();
    void handleKeyPress(QKeyEvent *event);
    void highlightKey(const QString &key);
    void clearKeyHighlight();
    void updateTrainingText();

    Ui::MainWindow *ui;
    LessonModel lessonModel;
    QStringList lessonPaths;
    QMap<QString, QPushButton *> keyboardButtons;
    QPushButton *activeKey = nullptr;
    bool lastInputWrong = false;
};

#endif
