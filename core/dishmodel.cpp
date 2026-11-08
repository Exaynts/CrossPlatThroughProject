#include "dishmodel.h"
#include <QDebug>

DishModel::DishModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DishModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0; // Плоский список
    return m_dishes.size();
}

QVariant DishModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_dishes.size())
        return QVariant();

    const Dish &dish = m_dishes.at(index.row());

    if (role == Qt::DisplayRole) {
        // То, что видит пользователь в списке
        return QString("%1 — %2 ккал").arg(dish.name).arg(dish.calories);
    }
    else if (role == Qt::EditRole) {
        // Данные для редактора (по умолчанию отдаем имя)
        return dish.name;
    }
    else if (role == NameRole) {
        return dish.name;
    }
    else if (role == CaloriesRole) {
        return dish.calories;
    }

    return QVariant();
}

bool DishModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_dishes.size())
        return false;

    Dish &dish = m_dishes[index.row()];

    if (role == Qt::EditRole) {
        // Временно поддерживаем изменение только имени (для этапа 1)
        dish.name = value.toString();
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole, NameRole});
        return true;
    }

    if (role == CaloriesRole) {
        dish.calories = value.toDouble();
        emit dataChanged(index, index, {Qt::DisplayRole, CaloriesRole});
        return true;
    }

    return false;
}

Qt::ItemFlags DishModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

void DishModel::addDish(const QString &name, double calories)
{
    beginInsertRows(QModelIndex(), m_dishes.size(), m_dishes.size());
    m_dishes.append({name, calories});
    endInsertRows();
    qDebug() << "[MODEL] Добавлено блюдо:" << name;
}

void DishModel::removeDish(int row)
{
    if (row < 0 || row >= m_dishes.size()) return;

    beginRemoveRows(QModelIndex(), row, row);
    m_dishes.removeAt(row);
    endRemoveRows();
    qDebug() << "[MODEL] Удалено блюдо (строка" << row << ")";
}

void DishModel::clear()
{
    if (m_dishes.isEmpty()) return;
    beginResetModel();
    m_dishes.clear();
    endResetModel();
    qDebug() << "[MODEL] Модель очищена";
}

double DishModel::totalCalories() const
{
    double total = 0;
    for (const Dish &d : m_dishes) total += d.calories;
    return total;
}
