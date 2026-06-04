#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "converter.h"

#include <QMainWindow>

class QComboBox;
class QLineEdit;

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

private:
    struct TabControls {
        Converter::Mode mode;
        QLineEdit *topEdit;
        QLineEdit *bottomEdit;
        QComboBox *topUnit;
        QComboBox *bottomUnit;
    };

    void setupValidators();
    void setupConnections();
    void updateFromTop(int tabIndex);
    void updateFromBottom(int tabIndex);
    void updateCurrentTab();
    void updateResult(const TabControls &controls, bool fromTop);
    void setError(QLineEdit *edit, bool hasError);
    QString formatResult(double value) const;
    TabControls controlsForTab(int tabIndex) const;

    Ui::MainWindow *ui;
    int lastEditedSide[3];
};

#endif
