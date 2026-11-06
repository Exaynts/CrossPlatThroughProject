#ifndef MEAL_H
#define MEAL_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>

class Meal : public QObject
{
    Q_OBJECT
public:
    explicit Meal(const QString &name, QObject *parent = nullptr);

    QString name() const { return m_name; }
    double totalCalories() const { return m_totalCalories; }
    int dishCount() const { return m_dishes.size(); }
    QList<QPair<QString, double>> dishes() const { return m_dishes; }

public slots:
    void setName(const QString &name);
    void addDish(const QString &name, double calories);
    void removeDish(int index);
    void clear();

signals:
    void nameChanged(const QString &name);
    void dishAdded(const QString &name, double calories);
    void dishRemoved(int index);
    void totalChanged(double total);
    void cleared();

private:
    QString m_name;
    QList<QPair<QString, double>> m_dishes;
    double m_totalCalories;
};

#endif // MEAL_H
