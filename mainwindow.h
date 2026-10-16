#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QList>
#include "nutritiondiary.h"

class QVBoxLayout;
class QLabel;
class QSpinBox;
class MealWidget;
class Meal;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAddMealClicked();
    void onResetDay();
    void onMinNormChanged(int value);
    void onMaxNormChanged(int value);
    void onDailyTotalChanged(double total);
    void onNormStatusChanged(NutritionDiary::NormStatus status, double total);
    void onDeleteMealRequested(MealWidget *widget);
    void onMealAdded(Meal *meal);
    void onMealRemoved(Meal *meal);
    void onSaveClicked();
    void onLoadClicked();

private:
    void setupUi();
    void addMealWidget(Meal *meal);
    void clearMealWidgets();

    NutritionDiary *m_diary;
    QPointer<QVBoxLayout> m_mealsLayout;
    QList<MealWidget*> m_mealWidgets;
    QPointer<QLabel> m_statusLabel;
    QPointer<QLabel> m_totalLabel;
    QPointer<QSpinBox> m_minNormSpin;
    QPointer<QSpinBox> m_maxNormSpin;
    QPointer<QLabel> m_fileStatusLabel;
};

#endif // MAINWINDOW_H
