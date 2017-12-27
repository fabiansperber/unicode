#include "..\include\unicode.h"

namespace unicode
{
bool get_next_char_from_utf8(std::string_view &text, char32_t &character)
{
  if (text.empty())
    return false;
  character = text[0] & 0xFF;
  text.remove_prefix(1);
  if (character > 0x7F) {
    if (character < 0xC0 || character > 0xF4 || character == 0xC0 || character == 0xc1)
    {
      while (!text.empty() && (text[0] & 0xFF) > 0x7F)
        text.remove_prefix(1);
      return false; // invalid chars or not a start char (< 0xC0)
    }
    bool check_for_surrogate = character == 0xED;
    bool check_for_overlong_three = character == 0xE0;
    bool check_for_overlong_four = character == 0xF0;
    auto num_chars_following =
      (character >= 0xF0) ? 3 :
      (character >= 0xE0) ? 2 : 1;
    if (text.size() < num_chars_following)
    {
      text.remove_prefix(text.size());
      return false;
    }
    auto mask = 0xFF << (6 - num_chars_following);
    character = character & ~mask;

    bool found_invalid_char = false;
    for (auto i = 0; i < num_chars_following; ++i) {
      if ((text[i] & 0xC0) != 0x80)
      {
        // not a valid follow char
        found_invalid_char = true;
        break;
      }
      character = (character << 6) | (text[i] & 0x3F);
    }
    text.remove_prefix(num_chars_following);
    if (found_invalid_char)
      return false;

    if (check_for_surrogate && 0xD800 <= character && character <= 0xDFFF)
        return false; // reserved for UTF-16 surrogate halves
    if (check_for_overlong_three && character < 0x0800)
      return false;
    if (check_for_overlong_four && character < 0x10000)
      return false;
    if (num_chars_following == 3 && character > 0x10FFFF)
      return false; // code points greater than U+10FFFF which are invalid
  }
  return true;
}

std::string to_utf8(std::u16string_view src)
{
  std::string result;
  char16_t last = 0;
  for (auto const& c : src)
  {
    if (last != 0) { // surrogate handling
      if (c <= 0xDBFF || c > 0xDFFF)
        throw "expected low surrogate";
      char32_t full = 0x10000 + ((last & 0x3FF) | ((c & 0x3FF) << 10));
      last = u'\0';
      result.push_back(0xF0 | static_cast<const char>((full >> 18) & 0x7));
      result.push_back(0x80 | static_cast<const char>((full >> 12) & 0x3F));
      result.push_back(0x80 | static_cast<const char>((full >> 6) & 0x3F));
      result.push_back(0x80 | (static_cast<const char>(full & 0x3F)));
      continue;
    }
    if (c >= 0xD800 && c <= 0xDFFF) { // surrogate detection
      if (c > 0xDBFF)
        throw "unexpected low surrogate";
      last = c;
      continue;
    }
    if (c > 0x7FF) {
      result.push_back(0xE0 | static_cast<const char>((c >> 12) & 0xF));
      result.push_back(0x80 | static_cast<const char>((c >> 6) & 0x3F));
      result.push_back(0x80 | (static_cast<const char>(c & 0x3F)));
      continue;
    }
    if (c > 0x7f) {
      result.push_back(0xC0 | static_cast<const char>(c >> 6));
      result.push_back(0x80 | (static_cast<const char>(c) & 0x3F));
      continue;
    }
    result.push_back(static_cast<const char>(c));
  }
  return result;
}

std::u16string to_utf16(std::string_view src)
{
  std::u16string result;
  for (auto const& c : src)
    result.push_back(c);
  return result;
}
}
