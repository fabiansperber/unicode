#pragma once

#include <string>
#include <string_view>

namespace unicode
{
bool get_next_char_from_utf8(std::string_view &text, char32_t &character);
struct Codepoints
{
  struct iterator {
    iterator() = default;
    iterator(std::string_view text) : remainder(text) { operator++(); }
    iterator& operator++()
    {
      if (!get_next_char_from_utf8(remainder, character))
        character = 0;
      return *this;
    }
    char32_t operator*() const { return character; }
    bool operator!=(iterator const& other) const
    {
      return character != other.character || remainder != other.remainder;
    }
  private:
    char32_t character;
    std::string_view remainder;
  };
  std::string_view text;
  iterator begin() const { return { text }; }
  iterator end() const { return {}; }
};
std::string to_utf8(std::u16string_view);
// std::string to_utf8(std::u32string_view);

std::u16string to_utf16(std::string_view);
// std::u16string to_utf16(std::u32string_view);

// std::u32string to_utf32(std::u16string_view);
// std::u32string to_utf32(std::u32string_view);
}
