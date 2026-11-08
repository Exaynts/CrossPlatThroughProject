#include "meal.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

Meal::Meal(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_model(new DishModel(this)) // Модель - ребенок Meal
{
    qDebug() << "[CREATE] Meal" << name;

    // Когда модель меняется, Meal пересчитывает общий итог
    connect(m_model, &DishModel::dataChanged, this, &Meal::updateTotal);
    connect(m_model, &DishModel::rowsInserted, this, &Meal::updateTotal);
    connect(m_model, &DishModel::rowsRemoved, this, &Meal::updateTotal);
    connect(m_model, &DishModel::modelReset, this, &Meal::updateTotal);
}

void Meal::setName(const QString &name)
{
    if (name.isEmpty() || name == m_name) return;
    m_name = name;
    emit nameChanged(m_name);
}

double Meal::totalCalories() const
{
    return m_model->totalCalories();
}

void Meal::addDish(const QString &name, double calories)
{
    if (name.isEmpty() || calories < 0) return;
    m_model->addDish(name, calories);
}

void Meal::removeDish(int row)
{
    m_model->removeDish(row);
}

void Meal::clear()
{
    m_model->clear();
}

void Meal::updateTotal()
{
    emit totalChanged(totalCalories());
}

QJsonObject Meal::toJson() const
{
    QJsonObject mealObj;
    mealObj["name"] = m_name;

    QJsonArray dishesArr;
    for (int i = 0; i < m_model->rowCount(); ++i) {
        QModelIndex idx = m_model->index(i, 0);
        QJsonObject dishObj;
        dishObj["name"] = m_model->data(idx, DishModel::NameRole).toString();
        dishObj["calories"] = m_model->data(idx, DishModel::CaloriesRole).toDouble();
        dishesArr.append(dishObj);
    }
    mealObj["dishes"] = dishesArr;
    return mealObj;
}

void Meal::fromJson(const QJsonObject &obj)
{
    // Очищаем модель перед загрузкой новых данных
    m_model->clear();

    if (obj.contains("name")) {
        setName(obj["name"].toString());
    }

    QJsonArray dishesArr = obj["dishes"].toArray();
    for (const QJsonValue &val : dishesArr) {
        QJsonObject dishObj = val.toObject();
        m_model->addDish(dishObj["name"].toString(),
                         dishObj["calories"].toDouble());
    }
}
