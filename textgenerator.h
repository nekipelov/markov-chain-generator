#ifndef TEXTGENERATOR_H
#define TEXTGENERATOR_H

#include <string>
#include <memory>
#include <locale>

#include <boost/noncopyable.hpp>

class TextGeneratorImpl;

class TextGenerator : boost::noncopyable
{
public:
    // Конструирует объект генератора текстов. Аргумент prefixSize задает порядок цепи
    // для обучния (в случае уже подготовленных данных, будет использован порядок
    // заданный при обучении. locale - используемую локаль.
    TextGenerator(size_t prefixSize = 3, const std::string &localeName = "ru_RU.UTF-8");
    TextGenerator(TextGenerator &&other);
    ~TextGenerator();

    // Обучение на тексте text
    void learn(const std::string &text);

    // Сгенерировать текст начинающийся с prefixString размером textSize слов.
    // Текст может получиться меньше textSize, если данных будет недостаточно.
    std::string generate(const std::string &prefixString, size_t textSize) const;

    // Сохранить данные в файл fileName. В случае проблем возвращает false, текстовое описание
    // ошибки можно получить с помощью errorString.
    bool store(const std::string &fileName) const;
    // Загрузить данные из файла fileName. В случае проблем возвращает false, текстовое описание
    // ошибки можно получить с помощью errorString.
    bool load(const std::string &fileName);

    // Возвращает текстовое описание ошибки для вызовов store и load.
    std::string errorString() const;

    // Возвращает порядок цепи
    size_t prefixSize() const;
    // Возвращает количество элементов цепи
    size_t chainSize() const;

private:
    std::unique_ptr<TextGeneratorImpl> pimpl;
};

#endif // TEXTGENERATOR_H
