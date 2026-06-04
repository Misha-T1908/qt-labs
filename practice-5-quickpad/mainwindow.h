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

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void exitApp();
    void cutText();
    void copyText();
    void pasteText();
    void selectAllText();
    void showAbout();
    void markModified();
    void updateEditActions();

private:
    bool maybeSave();
    bool saveToPath(const QString &path);
    void loadFromPath(const QString &path);
    void setCurrentPath(const QString &path);
    void updateWindowTitle();

    Ui::MainWindow *ui;
    QString currentPath;
    bool modified;
};

#endif
