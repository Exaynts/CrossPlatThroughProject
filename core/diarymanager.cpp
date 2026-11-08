#include "diarymanager.h"
#include <QDebug>

DiaryManager::DiaryManager(QObject *parent)
    : QObject(parent)
{
    qDebug() << "[CREATE] DiaryManager";
}

NutritionDiary* DiaryManager::getDiary(const QDate &date)
{
    if (!m_diaries.contains(date)) {
        // Создаем новый дневник, родитель = this (автоудаление)
        NutritionDiary *newDiary = new NutritionDiary(this);
        m_diaries.insert(date, newDiary);
        qDebug() << "[DIARY] Создан новый день:" << date.toString("dd-MM-yyyy");
    }
    return m_diaries.value(date);
}

bool DiaryManager::hasRecords(const QDate &date) const
{
    if (!m_diaries.contains(date)) return false;

    NutritionDiary *diary = m_diaries.value(date);
    for (Meal *meal : diary->meals()) {
        if (meal->model()->rowCount() > 0) {
            return true;
        }
    }
    return false;
}

QList<QDate> DiaryManager::datesWithRecords() const
{
    QList<QDate> dates;
    for (auto it = m_diaries.begin(); it != m_diaries.end(); ++it) {
        if (hasRecords(it.key())) {
            dates.append(it.key());
        }
    }
    return dates;
}

void DiaryManager::setUserName(const QString &name)
{
    m_userName = name;
}
