#pragma once

#include <string>
#include <string_view>

namespace unicode
{
bool get_next_char_from_utf8(std::string_view &text, char32_t &character);
std::string to_utf8(std::u16string_view);
// std::string to_utf8(std::u32string_view);

std::u16string to_utf16(std::string_view);
// std::u16string to_utf16(std::u32string_view);

// std::u32string to_utf32(std::u16string_view);
// std::u32string to_utf32(std::u32string_view);
}
