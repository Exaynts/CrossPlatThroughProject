#ifndef DIARYSTORAGE_H
#define DIARYSTORAGE_H

#include <QString>

class NutritionDiary;

class DiaryStorage
{
public:
    // Возвращает путь к файлу по умолчанию
    static QString defaultFilePath();

    // Сохраняет дневник в JSON-файл
    static bool save(const NutritionDiary &diary, const QString &filePath = QString());

    // Загружает дневник из JSON-файла
    static bool load(NutritionDiary &diary, const QString &filePath = QString());
};

#endif // DIARYSTORAGE_H
