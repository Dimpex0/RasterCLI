#include "Image.h"
#include <ctime>

std::string Image::getCurrentDateAndTime() const
{
    std::time_t now = std::time(nullptr);
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &now);

    std::string result(buffer);
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    // Изчиства датата, да да няма забранение символи при запис на файла
    for (char& c : result) {
        if (c == ' ' || c == ':') {
            c = '_';
        }
    }

    return result;
}