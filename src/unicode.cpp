#include "..\include\unicode.h"

namespace unicode
{
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
}
