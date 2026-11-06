#include "meal.h"
#include <QDebug>

Meal::Meal(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_totalCalories(0.0)
{
    qDebug() << "[CREATE] Meal" << name;
}

void Meal::setName(const QString &name)
{
    if (name.isEmpty() || name == m_name) return;
    m_name = name;
    emit nameChanged(m_name);
}

void Meal::addDish(const QString &name, double calories)
{
    if (name.isEmpty() || calories < 0) return;
    m_dishes.append(qMakePair(name, calories));
    m_totalCalories += calories;
    qDebug() << "[MEAL]" << m_name << "добавлено:" << name << "(" << calories << "ккал)";
    emit dishAdded(name, calories);
    emit totalChanged(m_totalCalories);
}

void Meal::removeDish(int index)
{
    if (index < 0 || index >= m_dishes.size()) return;
    double removedCal = m_dishes[index].second;
    QString removedName = m_dishes[index].first;
    m_dishes.removeAt(index);
    m_totalCalories -= removedCal;
    qDebug() << "[MEAL]" << m_name << "удалено блюдо:" << removedName;
    emit dishRemoved(index);
    emit totalChanged(m_totalCalories);
}

void Meal::clear()
{
    m_dishes.clear();
    m_totalCalories = 0.0;
    emit cleared();
    emit totalChanged(m_totalCalories);
}
