#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QComboBox>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QLocale>
#include <QSignalBlocker>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      lastEditedSide{0, 0, 0}
{
    ui->setupUi(this);
    setupValidators();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupValidators()
{
    const auto edits = findChildren<QLineEdit *>();
    for (QLineEdit *edit : edits) {
        auto *validator = new QDoubleValidator(edit);
        validator->setNotation(QDoubleValidator::StandardNotation);
        validator->setLocale(QLocale::C);
        edit->setValidator(validator);
    }
}

void MainWindow::setupConnections()
{
    connect(ui->editLengthTop, &QLineEdit::textEdited, this, [this]() { lastEditedSide[0] = 0; updateFromTop(0); });
    connect(ui->editLengthBottom, &QLineEdit::textEdited, this, [this]() { lastEditedSide[0] = 1; updateFromBottom(0); });
    connect(ui->editMassTop, &QLineEdit::textEdited, this, [this]() { lastEditedSide[1] = 0; updateFromTop(1); });
    connect(ui->editMassBottom, &QLineEdit::textEdited, this, [this]() { lastEditedSide[1] = 1; updateFromBottom(1); });
    connect(ui->editTemperatureTop, &QLineEdit::textEdited, this, [this]() { lastEditedSide[2] = 0; updateFromTop(2); });
    connect(ui->editTemperatureBottom, &QLineEdit::textEdited, this, [this]() { lastEditedSide[2] = 1; updateFromBottom(2); });

    const auto combos = findChildren<QComboBox *>();
    for (QComboBox *combo : combos) {
        connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::updateCurrentTab);
    }

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateCurrentTab);
}

void MainWindow::updateFromTop(int tabIndex)
{
    updateResult(controlsForTab(tabIndex), true);
}

void MainWindow::updateFromBottom(int tabIndex)
{
    updateResult(controlsForTab(tabIndex), false);
}

void MainWindow::updateCurrentTab()
{
    const int tabIndex = ui->tabWidget->currentIndex();
    updateResult(controlsForTab(tabIndex), lastEditedSide[tabIndex] == 0);
}

void MainWindow::updateResult(const TabControls &controls, bool fromTop)
{
    QLineEdit *sourceEdit = fromTop ? controls.topEdit : controls.bottomEdit;
    QLineEdit *targetEdit = fromTop ? controls.bottomEdit : controls.topEdit;
    QComboBox *sourceUnit = fromTop ? controls.topUnit : controls.bottomUnit;
    QComboBox *targetUnit = fromTop ? controls.bottomUnit : controls.topUnit;

    bool ok = false;
    const double sourceValue = sourceEdit->text().trimmed().toDouble(&ok);
    setError(sourceEdit, !ok && !sourceEdit->text().isEmpty());

    if (!ok) {
        QSignalBlocker blocker(targetEdit);
        targetEdit->clear();
        return;
    }

    const double result = Converter::convert(
        controls.mode,
        sourceValue,
        sourceUnit->currentText(),
        targetUnit->currentText());

    setError(targetEdit, false);
    QSignalBlocker blocker(targetEdit);
    targetEdit->setText(formatResult(result));
}

void MainWindow::setError(QLineEdit *edit, bool hasError)
{
    edit->setStyleSheet(hasError ? "QLineEdit { background: #ffe8e8; }" : "");
}

QString MainWindow::formatResult(double value) const
{
    return QString::number(value, 'g', 10);
}

MainWindow::TabControls MainWindow::controlsForTab(int tabIndex) const
{
    if (tabIndex == 1) {
        return {Converter::Mode::Mass, ui->editMassTop, ui->editMassBottom, ui->comboMassTopUnit, ui->comboMassBottomUnit};
    }
    if (tabIndex == 2) {
        return {Converter::Mode::Temperature, ui->editTemperatureTop, ui->editTemperatureBottom, ui->comboTemperatureTopUnit, ui->comboTemperatureBottomUnit};
    }

    return {Converter::Mode::Length, ui->editLengthTop, ui->editLengthBottom, ui->comboLengthTopUnit, ui->comboLengthBottomUnit};
}
