#ifndef MEALWIDGET_H
#define MEALWIDGET_H

#include <QGroupBox>
#include <QPointer>

class QLineEdit;
class QDoubleSpinBox;
class QListView;
class QLabel;
class QPushButton;
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
    void onRemoveDish();
    void onClearDishes();
    void onDeleteMealClicked();
    void onSelectionChanged();
    void updateTotal(double total);

private:
    Meal *m_meal;

    QPointer<QLineEdit> m_nameEdit;
    QPointer<QDoubleSpinBox> m_caloriesSpin;
    QPointer<QListView> m_view;
    QPointer<QLabel> m_totalLabel;

    QPointer<QPushButton> m_addBtn;
    QPointer<QPushButton> m_removeBtn;
    QPointer<QPushButton> m_clearBtn;
};

#endif // MEALWIDGET_H
