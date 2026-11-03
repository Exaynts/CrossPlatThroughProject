#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QTimer>
#include "nutritiondiary.h"

class QVBoxLayout;
class NormWidget;
class FileControlWidget;
class DiarySummaryWidget;
class MealsContainerWidget;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onResetDay();
    void onSaveClicked();
    void onLoadClicked();

private:
    void setupUi();

    NutritionDiary *m_diary;
    QTimer *m_autoSaveTimer;

    // Компоненты интерфейса
    QPointer<NormWidget> m_normWidget;
    QPointer<DiarySummaryWidget> m_summaryWidget;
    QPointer<MealsContainerWidget> m_mealsWidget;
    QPointer<FileControlWidget> m_fileWidget;
};

#endif // MAINWINDOW_H
