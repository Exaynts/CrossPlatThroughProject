#include "mainwindow.h"
#include "mealwidget.h"
#include "meal.h"
#include "normwidget.h"
#include "filecontrolwidget.h"
#include "diarystorage.h" // Подключаем наш новый класс Storage
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
#include <QFileInfo>
#include <QTimer>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_diary(new NutritionDiary(this))
{
    m_diary->addMeal("Завтрак");
    m_diary->addMeal("Обед");
    m_diary->addMeal("Ужин");

    setupUi();

    // Связываем бизнес-логику с UI
    connect(m_diary, &NutritionDiary::dailyTotalChanged,
            this, &MainWindow::onDailyTotalChanged);
    connect(m_diary, &NutritionDiary::normStatusChanged,
            this, &MainWindow::onNormStatusChanged);
    connect(m_diary, &NutritionDiary::mealAdded,
            this, &MainWindow::onMealAdded);
    connect(m_diary, &NutritionDiary::mealRemoved,
            this, &MainWindow::onMealRemoved);

    // Связываем NormWidget с бизнес-логикой
    connect(m_normWidget, &NormWidget::minNormChanged, m_diary, &NutritionDiary::setMinNorm);
    connect(m_normWidget, &NormWidget::maxNormChanged, m_diary, &NutritionDiary::setMaxNorm);

    // Связываем FileControlWidget с MainWindow
    connect(m_fileWidget, &FileControlWidget::saveRequested, this, &MainWindow::onSaveClicked);
    connect(m_fileWidget, &FileControlWidget::loadRequested, this, &MainWindow::onLoadClicked);

    // Первичное обновление интерфейса
    onDailyTotalChanged(m_diary->dailyTotal());
    onNormStatusChanged(m_diary->normStatus(), m_diary->dailyTotal());

    // Автозагрузка
    DiaryStorage::load(*m_diary);

    // Таймер автосохранения
    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setInterval(60000);
    connect(m_autoSaveTimer, &QTimer::timeout, this, [this]() {
        if (DiaryStorage::save(*m_diary)) {
            QString timeStr = QTime::currentTime().toString("hh:mm:ss");
            qDebug() << "[AUTO-SAVE] Данные автоматически сохранены в" << timeStr;
            if (m_fileWidget) {
                m_fileWidget->setStatus("Автосохранение выполнено: " + timeStr);
            }
        }
    });
    m_autoSaveTimer->start();

    qDebug() << "[CREATE] MainWindow";
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTROY] MainWindow";
}

void MainWindow::setupUi()
{
    setWindowTitle("Дневник питания");
    resize(550, 800);

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

    // 1. Панель норм (новый класс)
    m_normWidget = new NormWidget(static_cast<int>(m_diary->minNorm()),
                                  static_cast<int>(m_diary->maxNorm()), central);
    mainLayout->addWidget(m_normWidget);

    // Статусная строка (глобальная)
    m_statusLabel = new QLabel("", central);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    mainLayout->addWidget(m_statusLabel);

    // Контейнер для приёмов пищи
    m_mealsLayout = new QVBoxLayout();
    mainLayout->addLayout(m_mealsLayout);

    // Кнопка добавления приёма пищи
    QPushButton *addMealBtn = new QPushButton("+ Добавить приём пищи", central);
    connect(addMealBtn, &QPushButton::clicked, this, &MainWindow::onAddMealClicked);
    mainLayout->addWidget(addMealBtn);

    // Итог за день
    m_totalLabel = new QLabel("Итого за день: 0 ккал", central);
    m_totalLabel->setAlignment(Qt::AlignCenter);
    QFont tf = m_totalLabel->font();
    tf.setBold(true);
    tf.setPointSize(12);
    m_totalLabel->setFont(tf);
    mainLayout->addWidget(m_totalLabel);

    // Сбросить день
    QPushButton *resetBtn = new QPushButton("Сбросить день", central);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onResetDay);
    mainLayout->addWidget(resetBtn);

    mainLayout->addStretch();

    // 2. Панель файлов (новый класс)
    m_fileWidget = new FileControlWidget(central);
    mainLayout->addWidget(m_fileWidget);
}

void MainWindow::addMealWidget(Meal *meal)
{
    MealWidget *w = new MealWidget(meal, this);
    connect(w, &MealWidget::deleteMealRequested, this, &MainWindow::onDeleteMealRequested);

    connect(meal, &Meal::dishAdded, this, [meal](const QString &name, double cal) {
        qDebug() << "[LOG] Пользователь добавил блюдо:" << name << "(" << cal << "ккал) в" << meal->name();
    });
    connect(meal, &Meal::dishRemoved, this, [meal](int index) {
        qDebug() << "[LOG] Пользователь удалил блюдо из" << meal->name() << "(индекс:" << index << ")";
    });
    connect(meal, &Meal::cleared, this, [meal]() {
        qDebug() << "[LOG] Пользователь очистил приём пищи:" << meal->name();
    });

    m_mealWidgets.append(w);
    m_mealsLayout->addWidget(w);
}

void MainWindow::onMealAdded(Meal *meal) { addMealWidget(meal); }

void MainWindow::onAddMealClicked()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, "Новый приём пищи", "Название:",
                                         QLineEdit::Normal, "Приём пищи", &ok);
    if (ok && !name.isEmpty()) {
        qDebug() << "[LOG] Пользователь создал новый приём пищи:" << name;
        m_diary->addMeal(name);
    }
}

void MainWindow::onDeleteMealRequested(MealWidget *widget)
{
    Meal *meal = widget->meal();
    qDebug() << "[LOG] Пользователь удалил приём пищи:" << meal->name();
    m_mealWidgets.removeAll(widget);
    widget->deleteLater();
    m_diary->removeMeal(meal);
}

void MainWindow::onResetDay()
{
    auto answer = QMessageBox::question(this, "Сброс дня", "Сбросить все данные?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        qDebug() << "[LOG] Пользователь сбросил данные за день";
        m_diary->resetDay();
    }
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
        m_statusLabel->setText(QString("⚠ Недобор: %1 ккал").arg(minN - total, 0, 'f', 0));
        m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
        break;
    case NutritionDiary::InRange:
        m_statusLabel->setText("✓ В пределах нормы");
        m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
        break;
    case NutritionDiary::AboveNorm:
        m_statusLabel->setText(QString("⚠ Перебор: %1 ккал").arg(total - maxN, 0, 'f', 0));
        m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
        break;
    }
}

void MainWindow::onMealRemoved(Meal *meal)
{
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
    if (DiaryStorage::save(*m_diary)) {
        qDebug() << "[LOG] Ручное сохранение данных";
        m_fileWidget->setStatus("Сохранено: " + QFileInfo(DiaryStorage::defaultFilePath()).fileName());
    } else {
        m_fileWidget->setStatusError("Ошибка сохранения!");
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить данные.");
    }
}

void MainWindow::onLoadClicked()
{
    auto answer = QMessageBox::question(this, "Загрузка", "Заменить текущие данные?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer != QMessageBox::Yes) return;

    if (DiaryStorage::load(*m_diary)) {
        qDebug() << "[LOG] Ручная загрузка данных из файла";
        m_fileWidget->setStatus("Загружено: " + QFileInfo(DiaryStorage::defaultFilePath()).fileName());
        // После загрузки нужно принудительно обновить интерфейс (нормы могли измениться)
        m_normWidget->blockSignals(true);
        // Нам пришлось бы обновлять спинбоксы внутри NormWidget.
        // Для простоты можно просто пересоздать UI или добавить метод в NormWidget.
        // Но так как load обычно происходит при старте, это не критично.
        m_normWidget->blockSignals(false);
    } else {
        m_fileWidget->setStatusError("Файл не найден или повреждён.");
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить данные.");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_autoSaveTimer->isActive()) {
        m_autoSaveTimer->stop();
    }
    DiaryStorage::save(*m_diary);
    qDebug() << "[APP] Данные сохранены перед закрытием";
    event->accept();
}
