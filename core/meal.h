#ifndef MEAL_H
#define MEAL_H

#include <QObject>
#include <QString>
#include "dishmodel.h"

class Meal : public QObject
{
    Q_OBJECT
public:
    explicit Meal(const QString &name, QObject *parent = nullptr);

    QString name() const { return m_name; }
    DishModel* model() const { return m_model; }
    double totalCalories() const;
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &obj);

public slots:
    void setName(const QString &name);
    void addDish(const QString &name, double calories);
    void removeDish(int row);
    void clear();

signals:
    void nameChanged(const QString &name);
    void totalChanged(double total);

private:
    QString m_name;
    DishModel *m_model;

    void updateTotal(); //
};

#endif // MEAL_H
