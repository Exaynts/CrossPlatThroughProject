#ifndef MEALWIDGET_H
#define MEALWIDGET_H

#include <QGroupBox>
#include <QPointer>

class QLineEdit;
class QSpinBox;
class QListWidget;
class QLabel;
class Meal;

class MealWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit MealWidget(Meal *meal, QWidget *parent = nullptr);
    Meal* meal() const { return m_meal; }

signals:
    void deleteMealRequested(MealWidget *widget);

private slots:
    void onAddDish();
    void onDeleteDish();
    void onDeleteMealClicked();
    void refreshList();
    void updateTotal(double total);

private:
    Meal *m_meal;
    QPointer<QLineEdit> m_nameEdit;
    QPointer<QSpinBox> m_caloriesSpin;
    QPointer<QListWidget> m_list;
    QPointer<QLabel> m_totalLabel;
};

#endif // MEALWIDGET_H
