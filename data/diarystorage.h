#ifndef DIARYSTORAGE_H
#define DIARYSTORAGE_H

#include <QString>

class DiaryManager;

class DiaryStorage
{
public:
    static QString defaultFilePath();

    static bool save(const DiaryManager &manager, const QString &filePath = QString());
    static bool load(DiaryManager &manager, const QString &filePath = QString());
};

#endif // DIARYSTORAGE_H
