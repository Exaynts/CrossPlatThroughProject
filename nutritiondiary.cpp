#include "nutritiondiary.h"
#include <QDebug>

NutritionDiary::NutritionDiary(QObject *parent)
    : QObject(parent)
    , m_minNorm(1500.0)
    , m_maxNorm(2500.0)
{
    qDebug() << "[CREATE] NutritionDiary";

    // Создаём три стандартных приёма пищи
    addMeal("Завтрак");
    addMeal("Обед");
    addMeal("Ужин");
}

Meal* NutritionDiary::mealAt(int index) const
{
    if (index < 0 || index >= m_meals.size()) return nullptr;
    return m_meals.at(index);
}

double NutritionDiary::dailyTotal() const
{
    double total = 0.0;
    for (Meal *m : m_meals) total += m->totalCalories();
    return total;
}

NutritionDiary::NormStatus NutritionDiary::normStatus() const
{
    double total = dailyTotal();
    if (total < m_minNorm) return BelowNorm;
    if (total > m_maxNorm) return AboveNorm;
    return InRange;
}

Meal* NutritionDiary::addMeal(const QString &name)
{
    QString mealName = name.isEmpty()
                           ? QString("Приём пищи %1").arg(m_meals.size() + 1)
                           : name;
    Meal *meal = new Meal(mealName, this);
    m_meals.append(meal);

    // При изменении приёма пищи обновляем общий итог и статус
    connect(meal, &Meal::totalChanged, this, [this]() {
        emit dailyTotalChanged(dailyTotal());
        updateStatus();
    });

    emit mealAdded(meal);
    emit dailyTotalChanged(dailyTotal());
    updateStatus();
    return meal;
}

void NutritionDiary::removeMeal(Meal *meal)
{
    int index = m_meals.indexOf(meal);
    if (index < 0) return;
    m_meals.removeAt(index);
    qDebug() << "[DIARY] Удалён приём пищи:" << meal->name();
    emit mealRemoved(meal);
    meal->deleteLater();
    emit dailyTotalChanged(dailyTotal());
    updateStatus();
}

void NutritionDiary::setMinNorm(double norm)
{
    if (norm < 0 || qFuzzyCompare(norm, m_minNorm)) return;
    m_minNorm = norm;
    emit minNormChanged(norm);
    updateStatus();
}

void NutritionDiary::setMaxNorm(double norm)
{
    if (norm < 0 || qFuzzyCompare(norm, m_maxNorm)) return;
    m_maxNorm = norm;
    emit maxNormChanged(norm);
    updateStatus();
}

void NutritionDiary::resetDay()
{
    for (Meal *m : m_meals) m->clear();
    emit dailyTotalChanged(0);
    updateStatus();
}

void NutritionDiary::clearAllMeals()
{
    // Удаляем в обратном порядке, чтобы не сбить индексы
    while (!m_meals.isEmpty()) {
        Meal *meal = m_meals.takeLast();
        emit mealRemoved(meal);
        meal->deleteLater();
    }
    emit dailyTotalChanged(0);
    updateStatus();
}

void NutritionDiary::updateStatus()
{
    emit normStatusChanged(normStatus(), dailyTotal());
}
