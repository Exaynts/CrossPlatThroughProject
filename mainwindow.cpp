#include "mainwindow.h"
#include "mealwidget.h"
#include "meal.h"
#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QCloseEvent>
#include <QFileDialog>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_diary(new NutritionDiary(this))
{
    // Создаём базовые 3 приёма пищи — они могут быть перезаписаны загрузкой
    m_diary->addMeal("Завтрак");
    m_diary->addMeal("Обед");
    m_diary->addMeal("Ужин");

    setupUi();

    connect(m_diary, &NutritionDiary::dailyTotalChanged,
            this, &MainWindow::onDailyTotalChanged);
    connect(m_diary, &NutritionDiary::normStatusChanged,
            this, &MainWindow::onNormStatusChanged);
    connect(m_diary, &NutritionDiary::mealAdded,
            this, &MainWindow::onMealAdded);
    connect(m_diary, &NutritionDiary::mealRemoved,
            this, &MainWindow::onMealRemoved);

    // Первичное обновление
    onDailyTotalChanged(m_diary->dailyTotal());
    onNormStatusChanged(m_diary->normStatus(), m_diary->dailyTotal());

    // Автозагрузка: если файл есть — перезапишет приёмы пищи
    m_diary->loadFromFile();

    qDebug() << "[CREATE] MainWindow";
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTROY] MainWindow";
}

void MainWindow::setupUi()
{
    setWindowTitle("Дневник питания");
    resize(550, 750);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    setCentralWidget(scroll);

    QWidget *central = new QWidget(scroll);
    scroll->setWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Заголовок
    QLabel *title = new QLabel("Дневник питания", central);
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font();
    f.setBold(true);
    f.setPointSize(14);
    title->setFont(f);
    mainLayout->addWidget(title);

    // Нормы: от и до
    QHBoxLayout *normLayout = new QHBoxLayout();
    normLayout->addWidget(new QLabel("Норма от:", central));
    m_minNormSpin = new QSpinBox(central);
    m_minNormSpin->setRange(0, 10000);
    m_minNormSpin->setValue(static_cast<int>(m_diary->minNorm()));
    m_minNormSpin->setSuffix(" ккал");
    normLayout->addWidget(m_minNormSpin.data());

    normLayout->addWidget(new QLabel("до:", central));
    m_maxNormSpin = new QSpinBox(central);
    m_maxNormSpin->setRange(0, 10000);
    m_maxNormSpin->setValue(static_cast<int>(m_diary->maxNorm()));
    m_maxNormSpin->setSuffix(" ккал");
    normLayout->addWidget(m_maxNormSpin.data());
    normLayout->addStretch();
    mainLayout->addLayout(normLayout);

    // Статусная строка
    m_statusLabel = new QLabel("", central);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    mainLayout->addWidget(m_statusLabel.data());

    // Контейнер для динамически добавляемых приёмов пищи
    m_mealsLayout = new QVBoxLayout();
    mainLayout->addLayout(m_mealsLayout.data());

    // Кнопка добавления приёма пищи
    QPushButton *addMealBtn = new QPushButton("+ Добавить приём пищи", central);
    connect(addMealBtn, &QPushButton::clicked,
            this, &MainWindow::onAddMealClicked);
    mainLayout->addWidget(addMealBtn);

    // Итог за день
    m_totalLabel = new QLabel("Итого за день: 0 ккал", central);
    m_totalLabel->setAlignment(Qt::AlignCenter);
    QFont tf = m_totalLabel->font();
    tf.setBold(true);
    tf.setPointSize(12);
    m_totalLabel->setFont(tf);
    mainLayout->addWidget(m_totalLabel.data());

    // Сбросить день
    QPushButton *resetBtn = new QPushButton("Сбросить день", central);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onResetDay);
    mainLayout->addWidget(resetBtn);

    mainLayout->addStretch();

    // Кнопки сохранения/загрузки
    QHBoxLayout *fileLayout = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("💾 Сохранить", central);
    QPushButton *loadBtn = new QPushButton("📂 Загрузить", central);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
    fileLayout->addWidget(saveBtn);
    fileLayout->addWidget(loadBtn);
    mainLayout->addLayout(fileLayout);

    // Строка статуса файла
    m_fileStatusLabel = new QLabel("", central);
    m_fileStatusLabel->setAlignment(Qt::AlignCenter);
    m_fileStatusLabel->setStyleSheet("color: gray; font-size: 10px;");
    mainLayout->addWidget(m_fileStatusLabel.data());

    // Связки норм
    connect(m_minNormSpin.data(), QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onMinNormChanged);
    connect(m_maxNormSpin.data(), QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onMaxNormChanged);
}

void MainWindow::addMealWidget(Meal *meal)
{
    MealWidget *w = new MealWidget(meal, this);
    connect(w, &MealWidget::deleteMealRequested,
            this, &MainWindow::onDeleteMealRequested);
    m_mealWidgets.append(w);
    m_mealsLayout->addWidget(w);
}

void MainWindow::onMealAdded(Meal *meal)
{
    addMealWidget(meal);
}

void MainWindow::onAddMealClicked()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, "Новый приём пищи",
                                         "Название приёма пищи:", QLineEdit::Normal,
                                         QString("Приём пищи %1").arg(m_diary->mealCount() + 1), &ok);
    if (ok && !name.isEmpty()) {
        m_diary->addMeal(name);
    }
}

void MainWindow::onDeleteMealRequested(MealWidget *widget)
{
    Meal *meal = widget->meal();
    m_mealWidgets.removeAll(widget);
    widget->deleteLater();
    m_diary->removeMeal(meal);
}

void MainWindow::onResetDay()
{
    auto answer = QMessageBox::question(this, "Сброс дня",
                                        "Сбросить все данные за день?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        m_diary->resetDay();
    }
}

void MainWindow::onMinNormChanged(int value)
{
    // Нижняя граница не может быть выше верхней
    if (value > m_maxNormSpin->value()) {
        m_maxNormSpin->setValue(value);
    }
    m_diary->setMinNorm(value);
}

void MainWindow::onMaxNormChanged(int value)
{
    // Верхняя граница не может быть ниже нижней
    if (value < m_minNormSpin->value()) {
        m_minNormSpin->setValue(value);
    }
    m_diary->setMaxNorm(value);
}

void MainWindow::onDailyTotalChanged(double total)
{
    m_totalLabel->setText(QString("Итого за день: %1 ккал").arg(total));
}

void MainWindow::onNormStatusChanged(NutritionDiary::NormStatus status, double total)
{
    double minN = m_diary->minNorm();
    double maxN = m_diary->maxNorm();

    switch (status) {
    case NutritionDiary::BelowNorm:
        m_statusLabel->setText(QString("⚠ Недобор: %1 ккал (норма от %2 до %3)")
                                   .arg(minN - total, 0, 'f', 0).arg(minN, 0, 'f', 0).arg(maxN, 0, 'f', 0));
        m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
        break;
    case NutritionDiary::InRange:
        m_statusLabel->setText(QString("✓ В пределах нормы (%1 – %2 ккал)")
                                   .arg(minN, 0, 'f', 0).arg(maxN, 0, 'f', 0));
        m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
        break;
    case NutritionDiary::AboveNorm:
        m_statusLabel->setText(QString("⚠ Перебор: %1 ккал (норма от %2 до %3)")
                                   .arg(total - maxN, 0, 'f', 0).arg(minN, 0, 'f', 0).arg(maxN, 0, 'f', 0));
        m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
        break;
    }
}

void MainWindow::onMealRemoved(Meal *meal)
{
    // Находим виджет, соответствующий этому приёму пищи, и удаляем
    for (int i = 0; i < m_mealWidgets.size(); ++i) {
        if (m_mealWidgets[i]->meal() == meal) {
            MealWidget *w = m_mealWidgets.takeAt(i);
            w->deleteLater();
            break;
        }
    }
}

void MainWindow::onSaveClicked()
{
    if (m_diary->saveToFile()) {
        m_fileStatusLabel->setText("Сохранено: " + QFileInfo(NutritionDiary::defaultFilePath()).fileName());
    } else {
        m_fileStatusLabel->setText("Ошибка сохранения!");
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить данные.");
    }
}

void MainWindow::onLoadClicked()
{
    auto answer = QMessageBox::question(this, "Загрузка",
                                        "Текущие данные будут заменены данными из файла. Продолжить?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer != QMessageBox::Yes) return;

    if (m_diary->loadFromFile()) {
        m_fileStatusLabel->setText("Загружено: " + QFileInfo(NutritionDiary::defaultFilePath()).fileName());
    } else {
        m_fileStatusLabel->setText("Файл не найден или повреждён.");
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить данные.");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Автосохранение при закрытии окна
    m_diary->saveToFile();
    qDebug() << "[APP] Данные сохранены перед закрытием";
    event->accept();
}
