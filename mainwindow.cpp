#include "mainwindow.h"
#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_calculator(new Calculator(this)) // Calculator — ребенок окна!
{
    setupUi();

    // Связываем сигналы калькулятора со слотами окна
    connect(m_calculator, &Calculator::resultReady,
            this, &MainWindow::onResultReady);
    connect(m_calculator, &Calculator::errorOccurred,
            this, &MainWindow::onErrorOccurred);

    qDebug() << "[CREATE] MainWindow";
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTROY] MainWindow";
}

void MainWindow::setupUi()
{
    setWindowTitle("Сквозной проект: Qt Калькулятор");
    resize(400, 300);

    // Центральный виджет (родитель для всех элементов)
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Поля ввода
    QHBoxLayout *inputLayout = new QHBoxLayout();
    m_inputA = new QLineEdit(centralWidget);
    m_inputA->setPlaceholderText("Число A");
    m_inputB = new QLineEdit(centralWidget);
    m_inputB->setPlaceholderText("Число B");
    inputLayout->addWidget(m_inputA.data());
    inputLayout->addWidget(m_inputB.data());
    mainLayout->addLayout(inputLayout);

    // Кнопки операций
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("+", centralWidget);
    m_subButton = new QPushButton("-", centralWidget);
    m_mulButton = new QPushButton("*", centralWidget);
    m_divButton = new QPushButton("/", centralWidget);
    buttonLayout->addWidget(m_addButton.data());
    buttonLayout->addWidget(m_subButton.data());
    buttonLayout->addWidget(m_mulButton.data());
    buttonLayout->addWidget(m_divButton.data());
    mainLayout->addLayout(buttonLayout);

    // Кнопка сброса
    m_resetButton = new QPushButton("Сброс", centralWidget);
    mainLayout->addWidget(m_resetButton.data());

    // Метка результата
    m_resultLabel = new QLabel("Результат: 0", centralWidget);
    m_resultLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_resultLabel.data());

    // Соединения кнопок со слотами
    connect(m_addButton.data(), &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(m_subButton.data(), &QPushButton::clicked, this, &MainWindow::onSubClicked);
    connect(m_mulButton.data(), &QPushButton::clicked, this, &MainWindow::onMulClicked);
    connect(m_divButton.data(), &QPushButton::clicked, this, &MainWindow::onDivClicked);
    connect(m_resetButton.data(), &QPushButton::clicked, this, &MainWindow::onResetClicked);
}

void MainWindow::onAddClicked()
{
    bool ok1, ok2;
    double a = m_inputA->text().toDouble(&ok1);
    double b = m_inputB->text().toDouble(&ok2);
    if (ok1 && ok2) m_calculator->add(a, b);
}

void MainWindow::onSubClicked()
{
    bool ok1, ok2;
    double a = m_inputA->text().toDouble(&ok1);
    double b = m_inputB->text().toDouble(&ok2);
    if (ok1 && ok2) m_calculator->subtract(a, b);
}

void MainWindow::onMulClicked()
{
    bool ok1, ok2;
    double a = m_inputA->text().toDouble(&ok1);
    double b = m_inputB->text().toDouble(&ok2);
    if (ok1 && ok2) m_calculator->multiply(a, b);
}

void MainWindow::onDivClicked()
{
    bool ok1, ok2;
    double a = m_inputA->text().toDouble(&ok1);
    double b = m_inputB->text().toDouble(&ok2);
    if (ok1 && ok2) m_calculator->divide(a, b);
}

void MainWindow::onResetClicked()
{
    m_calculator->reset();
    m_inputA->clear();
    m_inputB->clear();
}

void MainWindow::onResultReady(double result)
{
    m_resultLabel->setText(QString("Результат: %1").arg(result, 0, 'f', 4));
}

void MainWindow::onErrorOccurred(const QString &message)
{
    m_resultLabel->setText("Ошибка: " + message);
}
