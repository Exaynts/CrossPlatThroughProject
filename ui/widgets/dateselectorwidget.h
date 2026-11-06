#ifndef DATESELECTORWIDGET_H
#define DATESELECTORWIDGET_H

#include <QWidget>
#include <QDate>

class QToolButton;
class QCalendarWidget;
class QMenu;
class QWidgetAction;

class DateSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DateSelectorWidget(QWidget *parent = nullptr);

    QDate currentDate() const;

public slots:
    void setDate(const QDate &date);
    // Передает список дат для подсветки в календаре
    void setHighlightedDates(const QList<QDate> &dates);

signals:
    void dateChanged(const QDate &date);

private slots:
    void onCalendarClicked(const QDate &date);
    void updateButtonText();

private:
    QToolButton *m_button;
    QCalendarWidget *m_calendar;
    QMenu *m_menu;
    QDate m_currentDate;
};

#endif // DATESELECTORWIDGET_H
