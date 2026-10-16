#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "calculator.h"

class QLineEdit;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onAddClicked();
    void onSubClicked();
    void onMulClicked();
    void onDivClicked();
    void onResetClicked();
    void onResultReady(double result);
    void onErrorOccurred(const QString &message);

private:
    void setupUi();

    // Бизнес-логика
    Calculator *m_calculator;

    // GUI-элементы (все с родителями!)
    QPointer<QLineEdit> m_inputA;
    QPointer<QLineEdit> m_inputB;
    QPointer<QLabel> m_resultLabel;
    QPointer<QPushButton> m_addButton;
    QPointer<QPushButton> m_subButton;
    QPointer<QPushButton> m_mulButton;
    QPointer<QPushButton> m_divButton;
    QPointer<QPushButton> m_resetButton;
};

#endif // MAINWINDOW_H
