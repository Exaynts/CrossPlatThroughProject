#include "diarystorage.h"
#include "diarymanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDebug>

QString DiaryStorage::defaultFilePath()
{
    return QDir::currentPath() + "/diary_history.json"; // Переименовали файл
}

bool DiaryStorage::save(const DiaryManager &manager, const QString &filePath)
{
    QString path = filePath.isEmpty() ? defaultFilePath() : filePath;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[STORAGE] Не удалось открыть файл для записи:" << path;
        return false;
    }

    QJsonObject root;
    root["userName"] = manager.userName();
    // Добавляем дни
    for (auto it = manager.allDiaries().begin(); it != manager.allDiaries().end(); ++it) {
        QString dateKey = it.key().toString(Qt::ISODate);
        root[dateKey] = it.value()->toJson();
    }

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "[STORAGE] Сохранено в" << path;
    return true;
}

bool DiaryStorage::load(DiaryManager &manager, const QString &filePath)
{
    QString path = filePath.isEmpty() ? defaultFilePath() : filePath;
    QFile file(path);
    if (!file.exists()) return false;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[STORAGE] Не удалось открыть файл для чтения:" << path;
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();

    // Загружаем имя пользователя
    if (root.contains("userName")) {
        manager.setUserName(root["userName"].toString());
    }

    // Загружаем дни
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (it.key() == "userName") continue; // Пропускаем строку с именем, это не дата

        QDate date = QDate::fromString(it.key(), Qt::ISODate);
        if (!date.isValid()) continue;

        NutritionDiary *diary = manager.getDiary(date);
        diary->fromJson(it.value().toObject());
    }

    qDebug() << "[STORAGE] Загружено из" << path;
    return true;
}
