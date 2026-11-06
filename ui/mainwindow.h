#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QTimer>
#include "nutritiondiary.h"
#include "diarymanager.h"

class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
class QLineEdit;
class QPushButton;
class QGroupBox;
class NormWidget;
class FileControlWidget;
class DiarySummaryWidget;
class MealsContainerWidget;
class DateSelectorWidget;

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
    void onDeleteClicked();
    void onSaveClicked();
    void onSettingsToggled();
    void onDailyTotalChanged(double total);
    void onNormStatusChanged(NutritionDiary::NormStatus status, double total);

private:
    void setupUi();

    DiaryManager *m_diaryManager;
    NutritionDiary *m_currentDiary;
    QTimer *m_autoSaveTimer;

    // Компоненты интерфейса
    QPointer<QWidget> m_settingsGroup;
    QPointer<QFormLayout> m_settingsForm;
    QPointer<QLineEdit> m_userNameEdit;

    QPointer<QHBoxLayout> m_actionLayout;
    QPointer<QPushButton> m_addBtn;
    QPointer<QPushButton> m_deleteBtn;
    QPointer<QPushButton> m_saveBtn;
    QPointer<QPushButton> m_settingsBtn;

    QPointer<DiarySummaryWidget> m_summaryWidget;
    QPointer<MealsContainerWidget> m_mealsWidget;
    QPointer<DateSelectorWidget> m_dateSelector;
    QPointer<NormWidget> m_normWidget;

    void onDateChanged(const QDate &date);
};

#endif // MAINWINDOW_H
