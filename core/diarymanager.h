#ifndef DIARYMANAGER_H
#define DIARYMANAGER_H

#include <QObject>
#include <QMap>
#include <QDate>
#include "nutritiondiary.h"

class DiaryManager : public QObject
{
    Q_OBJECT
public:
    explicit DiaryManager(QObject *parent = nullptr);

    // Возвращает дневник на дату (если нет — создает новый)
    NutritionDiary* getDiary(const QDate &date);

    QString userName() const { return m_userName; }
    void setUserName(const QString &name);

    // Проверяет, есть ли записи в этот день
    bool hasRecords(const QDate &date) const;

    // Возвращает список дат, в которых есть записи (для календаря)
    QList<QDate> datesWithRecords() const;

    // Для сохранения в файл
    const QMap<QDate, NutritionDiary*>& allDiaries() const { return m_diaries; }

private:
    QMap<QDate, NutritionDiary*> m_diaries;
    QString m_userName;
};

#endif // DIARYMANAGER_H
