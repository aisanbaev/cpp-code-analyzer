#pragma once

#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>

inline int ToInt(std::string_view str) {
    // Удаляем пробелы в начале и конце
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos)
        return 0;

    size_t end = str.find_last_not_of(" \t\n\r");
    std::string_view trimmed = str.substr(start, end - start + 1);

    int result{};
    auto [parse_end_ptr, error_code] = std::from_chars(trimmed.data(), trimmed.data() + trimmed.size(), result);

    if (error_code != std::errc{} || parse_end_ptr != trimmed.data() + trimmed.size()) {
        throw std::invalid_argument("Cannot convert '" + std::string(str) + "' to integral");
    }

    return result;
}
