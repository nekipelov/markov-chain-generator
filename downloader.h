#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

/*
 * Класс для загрузки url'ов с помощью curl через pipe.
 *
 */
class Downloader
{
public:
    // Скачивает url, записывая ответ сервера в content.
    // Возвращает true в случае успеха, иначе false.
    // В случае ошибки следует вызвать errorString, чтобы получить описание.
    bool download(const std::string &url, std::string &content);

    // Возвращает текстовое описание ошибки.
    std::string errorString() const;

private:
    std::string m_error;
};

#endif // DOWNLOADER_H
