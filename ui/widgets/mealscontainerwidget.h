#ifndef MEALSCONTAINERWIDGET_H
#define MEALSCONTAINERWIDGET_H

#include <QWidget>
#include <QList>
#include <QPointer>

class QVBoxLayout;
class NutritionDiary;
class Meal;
class MealWidget;

class MealsContainerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MealsContainerWidget(NutritionDiary *diary, QWidget *parent = nullptr);
    void setDiary(NutritionDiary *newDiary);

private slots:
    void onAddMealClicked();
    void onMealAdded(Meal *meal);
    void onMealRemoved(Meal *meal);
    void onDeleteMealRequested(MealWidget *widget);

private:
    NutritionDiary *m_diary;
    QVBoxLayout *m_mealsLayout;
    QList<MealWidget*> m_mealWidgets;
};

#endif // MEALSCONTAINERWIDGET_H
