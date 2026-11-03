#include "mainwindow.h"
#include "normwidget.h"
#include "filecontrolwidget.h"
#include "diarysummarywidget.h"
#include "mealscontainerwidget.h"
#include "diarystorage.h"
#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
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

    // --- СВЯЗЫВАНИЕ КОМПОНЕНТОВ ---

    // Нормы -> Модель
    connect(m_normWidget, &NormWidget::minNormChanged, m_diary, &NutritionDiary::setMinNorm);
    connect(m_normWidget, &NormWidget::maxNormChanged, m_diary, &NutritionDiary::setMaxNorm);

    // Модель -> Панель итогов
    connect(m_diary, &NutritionDiary::dailyTotalChanged, m_summaryWidget, &DiarySummaryWidget::updateTotal);
    connect(m_diary, &NutritionDiary::normStatusChanged, this, [this](NutritionDiary::NormStatus st, double total) {
        m_summaryWidget->updateStatus(st, total, m_diary->minNorm(), m_diary->maxNorm());
    });

    // Файловые операции
    connect(m_fileWidget, &FileControlWidget::saveRequested, this, &MainWindow::onSaveClicked);
    connect(m_fileWidget, &FileControlWidget::loadRequested, this, &MainWindow::onLoadClicked);

    // Первичное обновление интерфейса
    m_summaryWidget->updateTotal(m_diary->dailyTotal());
    m_summaryWidget->updateStatus(m_diary->normStatus(), m_diary->dailyTotal(), m_diary->minNorm(), m_diary->maxNorm());

    // Автозагрузка
    DiaryStorage::load(*m_diary);

    // --- ТАЙМЕР АВТОСОХРАНЕНИЯ ---
    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setInterval(60000);
    connect(m_autoSaveTimer, &QTimer::timeout, this, [this]() {
        if (DiaryStorage::save(*m_diary)) {
            QString timeStr = QTime::currentTime().toString("hh:mm:ss");
            qDebug() << "[AUTO-SAVE] Данные сохранены в" << timeStr;
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

    // 1. Заголовок
    QLabel *title = new QLabel("Дневник питания", central);
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font();
    f.setBold(true);
    f.setPointSize(14);
    title->setFont(f);
    mainLayout->addWidget(title);

    // 2. Панель норм
    m_normWidget = new NormWidget(static_cast<int>(m_diary->minNorm()),
                                  static_cast<int>(m_diary->maxNorm()), central);
    mainLayout->addWidget(m_normWidget);

    // 3. Панель итогов (статус + общая сумма)
    m_summaryWidget = new DiarySummaryWidget(central);
    mainLayout->addWidget(m_summaryWidget);

    // 4. Список приемов пищи (включая кнопку "+ Добавить")
    m_mealsWidget = new MealsContainerWidget(m_diary, central);
    mainLayout->addWidget(m_mealsWidget);

    // 5. Кнопка сброса
    QPushButton *resetBtn = new QPushButton("Сбросить день", central);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onResetDay);
    mainLayout->addWidget(resetBtn);

    mainLayout->addStretch();

    // 6. Файловые операции
    m_fileWidget = new FileControlWidget(central);
    mainLayout->addWidget(m_fileWidget);
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
        // После загрузки обновляем интерфейс
        m_summaryWidget->updateTotal(m_diary->dailyTotal());
        m_summaryWidget->updateStatus(m_diary->normStatus(), m_diary->dailyTotal(), m_diary->minNorm(), m_diary->maxNorm());
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
