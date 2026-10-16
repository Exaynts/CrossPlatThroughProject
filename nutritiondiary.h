#ifndef NUTRITIONDIARY_H
#define NUTRITIONDIARY_H

#include <QObject>
#include <QJsonObject>
#include "meal.h"

class NutritionDiary : public QObject
{
    Q_OBJECT
public:
    enum NormStatus { BelowNorm, InRange, AboveNorm };
    Q_ENUM(NormStatus)

    explicit NutritionDiary(QObject *parent = nullptr);

    const QList<Meal*>& meals() const { return m_meals; }
    int mealCount() const { return m_meals.size(); }
    Meal* mealAt(int index) const;

    double dailyTotal() const;
    double minNorm() const { return m_minNorm; }
    double maxNorm() const { return m_maxNorm; }
    NormStatus normStatus() const;

    // --- Новые методы для JSON ---
    static QString defaultFilePath();
    bool saveToFile(const QString &filePath = QString()) const;
    bool loadFromFile(const QString &filePath = QString());

public slots:
    Meal* addMeal(const QString &name);
    void removeMeal(Meal *meal);
    void clearAllMeals();
    void setMinNorm(double minNorm);
    void setMaxNorm(double maxNorm);
    void resetDay();

signals:
    void mealAdded(Meal *meal);
    void mealRemoved(Meal *meal);
    void dailyTotalChanged(double total);
    void minNormChanged(double norm);
    void maxNormChanged(double norm);
    void normStatusChanged(NutritionDiary::NormStatus status, double total);
    void diaryLoaded();   // <-- новое

private:
    QList<Meal*> m_meals;
    double m_minNorm;
    double m_maxNorm;
    void updateStatus();
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &obj);
};

#endif // NUTRITIONDIARY_H
