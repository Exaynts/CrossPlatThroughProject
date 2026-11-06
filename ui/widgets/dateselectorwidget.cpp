#include "dateselectorwidget.h"
#include <QToolButton>
#include <QCalendarWidget>
#include <QMenu>
#include <QWidgetAction>
#include <QVBoxLayout>
#include <QDebug>
#include <QTextCharFormat>

DateSelectorWidget::DateSelectorWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentDate(QDate::currentDate())
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Кнопка, которая выглядит как дата
    m_button = new QToolButton(this);
    m_button->setPopupMode(QToolButton::InstantPopup);
    m_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_button->setText("📅 " + m_currentDate.toString("dd-MM-yyyy"));
    m_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->addWidget(m_button);

    // Выпадающее меню с календарем
    m_menu = new QMenu(this);
    m_calendar = new QCalendarWidget(this);
    m_calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    m_calendar->setGridVisible(true);

    // Запрещаем выбирать будущие даты
    m_calendar->setMaximumDate(QDate::currentDate());
    m_calendar->setSelectedDate(m_currentDate);

    QWidgetAction *action = new QWidgetAction(m_menu);
    action->setDefaultWidget(m_calendar);
    m_menu->addAction(action);
    m_button->setMenu(m_menu);

    // Связываем клик по дате в календаре
    connect(m_calendar, &QCalendarWidget::clicked, this, &DateSelectorWidget::onCalendarClicked);
}

QDate DateSelectorWidget::currentDate() const
{
    return m_currentDate;
}

void DateSelectorWidget::setDate(const QDate &date)
{
    if (!date.isValid() || date == m_currentDate) return;
    m_currentDate = date;
    m_calendar->setSelectedDate(date);
    updateButtonText();
    emit dateChanged(date);
}

void DateSelectorWidget::setHighlightedDates(const QList<QDate> &dates)
{
    // Сбрасываем старые форматы
    m_calendar->setDateTextFormat(QDate(), QTextCharFormat());

    // Настраиваем формат для дней с записями (жирный синий текст)
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setForeground(QBrush(Qt::blue));

    for (const QDate &date : dates) {
        m_calendar->setDateTextFormat(date, format);
    }
}

void DateSelectorWidget::onCalendarClicked(const QDate &date)
{
    if (date == m_currentDate) {
        m_menu->hide();
        return;
    }

    m_currentDate = date;
    updateButtonText();
    m_menu->hide();
    emit dateChanged(date);
    qDebug() << "[DATE] Выбрана дата:" << date.toString("dd-MM-yyyy");
}

void DateSelectorWidget::updateButtonText()
{
    m_button->setText("📅 " + m_currentDate.toString("dd-MM-yyyy"));
}
