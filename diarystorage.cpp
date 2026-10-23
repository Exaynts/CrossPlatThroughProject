#include "diarystorage.h"
#include "nutritiondiary.h"
#include "meal.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QDebug>

namespace {
// Приватная функция для сериализации (не видна снаружи)
QJsonObject diaryToJson(const NutritionDiary &diary)
{
    QJsonObject root;
    root["minNorm"] = diary.minNorm();
    root["maxNorm"] = diary.maxNorm();

    QJsonArray mealsArr;
    for (Meal *meal : diary.meals()) {
        QJsonObject mealObj;
        mealObj["name"] = meal->name();

        QJsonArray dishesArr;
        for (const auto &dish : meal->dishes()) {
            QJsonObject dishObj;
            dishObj["name"] = dish.first;
            dishObj["calories"] = dish.second;
            dishesArr.append(dishObj);
        }
        mealObj["dishes"] = dishesArr;
        mealsArr.append(mealObj);
    }
    root["meals"] = mealsArr;
    return root;
}

// Приватная функция для десериализации
void jsonToDiary(const QJsonObject &obj, NutritionDiary &diary)
{
    // 1. Очищаем старые данные
    diary.clearAllMeals();

    // 2. Нормы
    if (obj.contains("minNorm"))
        diary.setMinNorm(obj["minNorm"].toDouble(1500.0));
    if (obj.contains("maxNorm"))
        diary.setMaxNorm(obj["maxNorm"].toDouble(2500.0));

    // 3. Приёмы пищи и блюда
    QJsonArray mealsArr = obj["meals"].toArray();
    for (const QJsonValue &mealVal : mealsArr) {
        QJsonObject mealObj = mealVal.toObject();
        QString mealName = mealObj["name"].toString("Приём пищи");
        Meal *meal = diary.addMeal(mealName);

        QJsonArray dishesArr = mealObj["dishes"].toArray();
        for (const QJsonValue &dishVal : dishesArr) {
            QJsonObject dishObj = dishVal.toObject();
            meal->addDish(dishObj["name"].toString(),
                          dishObj["calories"].toDouble());
        }
    }
}
} // конец анонимного namespace

QString DiaryStorage::defaultFilePath()
{
    return QDir::currentPath() + "/diary.json";
}

bool DiaryStorage::save(const NutritionDiary &diary, const QString &filePath)
{
    QString path = filePath.isEmpty() ? defaultFilePath() : filePath;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[STORAGE] Не удалось открыть файл для записи:" << path;
        return false;
    }

    QJsonDocument doc(diaryToJson(diary));
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "[STORAGE] Сохранено в" << path;
    return true;
}

bool DiaryStorage::load(NutritionDiary &diary, const QString &filePath)
{
    QString path = filePath.isEmpty() ? defaultFilePath() : filePath;
    QFile file(path);
    if (!file.exists()) {
        qDebug() << "[STORAGE] Файл не найден, пропускаем загрузку:" << path;
        return false;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[STORAGE] Не удалось открыть файл для чтения:" << path;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "[STORAGE] Ошибка парсинга JSON:" << err.errorString();
        return false;
    }

    jsonToDiary(doc.object(), diary);
    // Сигнал diaryLoaded() мы убрали, так как теперь загрузкой управляет MainWindow
    qDebug() << "[STORAGE] Загружено из" << path;
    return true;
}
