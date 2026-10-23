#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QList>
#include <QTimer>
#include "nutritiondiary.h"

class QVBoxLayout;
class QLabel;
class QSpinBox; // Оставляем, так как QSpinBox используется в MealWidget, но здесь можно удалить
class MealWidget;
class Meal;
class NormWidget;
class FileControlWidget;

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

    NutritionDiary *m_diary;
    QTimer *m_autoSaveTimer;

    QPointer<QVBoxLayout> m_mealsLayout;
    QList<MealWidget*> m_mealWidgets;

    QPointer<QLabel> m_statusLabel; // Это глобальная строка статуса (недобор/перебор)
    QPointer<QLabel> m_totalLabel;  // Итог за день

    // Новые компоненты
    QPointer<NormWidget> m_normWidget;
    QPointer<FileControlWidget> m_fileWidget;
};

#endif // MAINWINDOW_H
