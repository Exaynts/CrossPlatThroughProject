#ifndef DISHMODEL_H
#define DISHMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>

// Структура для хранения одного блюда
struct Dish {
    QString name;
    double calories;
};

class DishModel : public QAbstractListModel
{
    Q_OBJECT
public:
    // Кастомные роли для доступа к данным из делегата
    enum DishRoles {
        NameRole = Qt::UserRole + 1,
        CaloriesRole
    };

    explicit DishModel(QObject *parent = nullptr);

    // Обязательные методы QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Специфичные методы для нашего приложения
    void addDish(const QString &name, double calories);
    void removeDish(int row);
    void clear();

    // Получить общую калорийность всех блюд в модели
    double totalCalories() const;

private:
    QList<Dish> m_dishes;
};

#endif // DISHMODEL_H
