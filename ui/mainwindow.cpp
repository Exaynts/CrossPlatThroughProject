#include "mainwindow.h"
#include "normwidget.h"
#include "diarysummarywidget.h"
#include "mealscontainerwidget.h"
#include "diarystorage.h"
#include "diarymanager.h"
#include "dateselectorwidget.h"

#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollArea>
#include <QCloseEvent>
#include <QFileInfo>
#include <QTimer>
#include <QTime>
#include <QInputDialog>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_diaryManager(new DiaryManager(this))
{
    DiaryStorage::load(*m_diaryManager);
    m_currentDiary = m_diaryManager->getDiary(QDate::currentDate());

    setupUi();

    connect(m_currentDiary, &NutritionDiary::dailyTotalChanged, this, &MainWindow::onDailyTotalChanged);
    connect(m_currentDiary, &NutritionDiary::normStatusChanged, this, &MainWindow::onNormStatusChanged);

    onDailyTotalChanged(m_currentDiary->dailyTotal());
    onNormStatusChanged(m_currentDiary->normStatus(), m_currentDiary->dailyTotal());

    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setInterval(60000);
    connect(m_autoSaveTimer, &QTimer::timeout, this, [this]() {
        if (DiaryStorage::save(*m_diaryManager)) {
            qDebug() << "[AUTO-SAVE] История сохранена в" << QTime::currentTime().toString("hh:mm:ss");
        }
    });
    m_autoSaveTimer->start();
    m_dateSelector->setHighlightedDates(m_diaryManager->datesWithRecords());

    qDebug() << "[INIT] Окно создано. Текущая дата:" << QDate::currentDate().toString("dd-MM-yyyy");
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTROY] MainWindow";
}

void MainWindow::setupUi()
{
    setWindowTitle("Дневник питания (Адаптивный интерфейс)");
    resize(600, 800);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Главный вертикальный Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // ВЕРХНЯЯ ПАНЕЛЬ
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *title = new QLabel("Мой дневник питания", central);
    QFont f = title->font();
    f.setBold(true);
    f.setPointSize(16);
    title->setFont(f);
    headerLayout->addWidget(title);
    headerLayout->addStretch(); // Отодвигаем селектор даты вправо

    // Селектор даты
    m_dateSelector = new DateSelectorWidget(central);
    m_dateSelector->setDate(QDate::currentDate());
    // Подключаем сигнал смены даты (слот onDateChanged мы напишем отдельно)
    connect(m_dateSelector, &DateSelectorWidget::dateChanged, this, &MainWindow::onDateChanged);
    headerLayout->addWidget(m_dateSelector);

    mainLayout->addLayout(headerLayout);

    // ПАНЕЛЬ ДЕЙСТВИЙ (Кнопки)
    QHBoxLayout *actionLayout = new QHBoxLayout();

    QPushButton *addBtn = new QPushButton("Добавить", central);
    QPushButton *deleteBtn = new QPushButton("Удалить", central);
    m_saveBtn = new QPushButton("Сохранить", central);
    m_settingsBtn = new QPushButton("Настройки", central);

    actionLayout->addWidget(addBtn);
    actionLayout->addWidget(deleteBtn);
    actionLayout->addStretch(); // Прижимаем "Сохранить" и "Настройки" вправо
    actionLayout->addWidget(m_saveBtn);
    actionLayout->addWidget(m_settingsBtn);

    mainLayout->addLayout(actionLayout);

    // ФОРМА НАСТРОЕК (Скрываемая)
    m_settingsGroup = new QGroupBox("Настройки профиля", central);
    m_settingsForm = new QFormLayout(m_settingsGroup);

    m_userNameEdit = new QLineEdit(m_settingsGroup);
    m_userNameEdit->setPlaceholderText("Введите ваше имя");
    m_userNameEdit->setText(m_diaryManager->userName());
    m_userNameEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_settingsForm->addRow("Имя пользователя:", m_userNameEdit);

    // Нормы калорий (сохраняем указатель m_normWidget в классе!)
    m_normWidget = new NormWidget(static_cast<int>(m_currentDiary->minNorm()),
                                  static_cast<int>(m_currentDiary->maxNorm()), central);
    m_settingsForm->addRow("Нормы калорий:", m_normWidget);

    // Связываем нормы с ТЕКУЩИМ дневником
    connect(m_normWidget, &NormWidget::minNormChanged, m_currentDiary, &NutritionDiary::setMinNorm);
    connect(m_normWidget, &NormWidget::maxNormChanged, m_currentDiary, &NutritionDiary::setMaxNorm);
    connect(m_userNameEdit, &QLineEdit::textChanged, m_diaryManager, &DiaryManager::setUserName);

    mainLayout->addWidget(m_settingsGroup);
    m_settingsGroup->setVisible(false); // По умолчанию скрыто

    // КОНТЕЙНЕР ПРИЕМОВ ПИЩИ (Скроллируемая область)
    QScrollArea *scroll = new QScrollArea(central);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    m_mealsWidget = new MealsContainerWidget(m_currentDiary, scroll);
    scroll->setWidget(m_mealsWidget);
    // Даем скроллу политику Expanding, чтобы он забирал все свободное место по вертикали
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addWidget(scroll);

    // ПАНЕЛЬ ИТОГОВ (Под списком блюд)
    m_summaryWidget = new DiarySummaryWidget(central);
    mainLayout->addWidget(m_summaryWidget);

    // ПОДКЛЮЧЕНИЕ КНОПОК
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddMealClicked);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::onSettingsToggled);
}

void MainWindow::onAddMealClicked()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, "Новый приём пищи", "Название:",
                                         QLineEdit::Normal, "Приём пищи", &ok);
    if (ok && !name.isEmpty()) {
        qDebug() << "[LOG] Создан приём пищи:" << name;
        m_currentDiary->addMeal(name); // Используем ТЕКУЩИЙ день
    }
}

void MainWindow::onDeleteClicked()
{
    auto answer = QMessageBox::question(this, "Удалить", "Сбросить все данные за сегодняшний день?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        qDebug() << "[LOG] Сброс текущего дня";
        m_currentDiary->resetDay();
    }
}

void MainWindow::onSaveClicked()
{
    // Сохраняем ВСЮ историю через менеджер
    if (DiaryStorage::save(*m_diaryManager)) {
        QMessageBox::information(this, "Успех", "История сохранена в " + QFileInfo(DiaryStorage::defaultFilePath()).fileName());
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить данные.");
    }
}

void MainWindow::onSettingsToggled()
{
    bool isVisible = m_settingsGroup->isVisible();
    m_settingsGroup->setVisible(!isVisible);
    m_settingsBtn->setText(isVisible ? "Настройки" : "Скрыть настройки");
}

void MainWindow::onDailyTotalChanged(double total)
{
    m_summaryWidget->updateTotal(total);
}

void MainWindow::onNormStatusChanged(NutritionDiary::NormStatus status, double total)
{
    m_summaryWidget->updateStatus(status, total, m_currentDiary->minNorm(), m_currentDiary->maxNorm());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_autoSaveTimer->isActive()) {
        m_autoSaveTimer->stop();
    }
    // Сохраняем всю историю перед выходом
    DiaryStorage::save(*m_diaryManager);
    qDebug() << "[APP] История сохранена перед закрытием";
    event->accept();
}

void MainWindow::onDateChanged(const QDate &date)
{
    if (m_currentDiary) {
        disconnect(m_currentDiary, nullptr, this, nullptr);
    }

    m_currentDiary = m_diaryManager->getDiary(date);
    m_mealsWidget->setDiary(m_currentDiary);

    // Подключаем сигналы к новому дню
    connect(m_currentDiary, &NutritionDiary::dailyTotalChanged, this, &MainWindow::onDailyTotalChanged);
    connect(m_currentDiary, &NutritionDiary::normStatusChanged, this, &MainWindow::onNormStatusChanged);

    // Принудительно обновляем интерфейс
    onDailyTotalChanged(m_currentDiary->dailyTotal());
    onNormStatusChanged(m_currentDiary->normStatus(), m_currentDiary->dailyTotal());

    // Обновляем подсветку дат с записями в календаре
    m_dateSelector->setHighlightedDates(m_diaryManager->datesWithRecords());
}
