#ifndef UTILS_H
#define UTILS_H

#include <string>

// Возвращает текстовое описание текущей ошибки в errno, добавляя в начало msgPrefix.
std::string systemError(const std::string &msgPrefix);

#endif // TEXTUTILS_H
