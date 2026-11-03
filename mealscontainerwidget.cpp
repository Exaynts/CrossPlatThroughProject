#include "mealscontainerwidget.h"
#include "nutritiondiary.h"
#include "meal.h"
#include "mealwidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QDebug>

MealsContainerWidget::MealsContainerWidget(NutritionDiary *diary, QWidget *parent)
    : QWidget(parent)
    , m_diary(diary)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Контейнер для самих виджетов приемов пищи
    m_mealsLayout = new QVBoxLayout();
    mainLayout->addLayout(m_mealsLayout);

    // Кнопка добавления
    QPushButton *addMealBtn = new QPushButton("+ Добавить приём пищи", this);
    connect(addMealBtn, &QPushButton::clicked, this, &MealsContainerWidget::onAddMealClicked);
    mainLayout->addWidget(addMealBtn);

    // Подписываемся на сигналы модели
    connect(m_diary, &NutritionDiary::mealAdded, this, &MealsContainerWidget::onMealAdded);
    connect(m_diary, &NutritionDiary::mealRemoved, this, &MealsContainerWidget::onMealRemoved);

    // Добавляем уже существующие приемы пищи (если они есть)
    for (Meal *meal : m_diary->meals()) {
        onMealAdded(meal);
    }
}

void MealsContainerWidget::onAddMealClicked()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, "Новый приём пищи", "Название:",
                                         QLineEdit::Normal,
                                         QString("Приём пищи %1").arg(m_diary->mealCount() + 1), &ok);
    if (ok && !name.isEmpty()) {
        qDebug() << "[LOG] Пользователь создал новый приём пищи:" << name;
        m_diary->addMeal(name);
    }
}

void MealsContainerWidget::onMealAdded(Meal *meal)
{
    MealWidget *w = new MealWidget(meal, this);
    connect(w, &MealWidget::deleteMealRequested, this, &MealsContainerWidget::onDeleteMealRequested);

    // Логирование действий с блюдами
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

void MealsContainerWidget::onMealRemoved(Meal *meal)
{
    for (int i = 0; i < m_mealWidgets.size(); ++i) {
        if (m_mealWidgets[i]->meal() == meal) {
            MealWidget *w = m_mealWidgets.takeAt(i);
            w->deleteLater();
            break;
        }
    }
}

void MealsContainerWidget::onDeleteMealRequested(MealWidget *widget)
{
    Meal *meal = widget->meal();
    qDebug() << "[LOG] Пользователь удалил приём пищи:" << meal->name();
    m_diary->removeMeal(meal); // Модель сама вызовет сигнал mealRemoved, и мы удалим виджет
}
