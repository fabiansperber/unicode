#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <unicode.h>
#include <deque>

using unicode::get_next_char_from_utf8;
using unicode::Codepoints;
using unicode::to_utf8;
using unicode::to_utf16;

TEST_CASE("empty string")
{
  REQUIRE(to_utf8(u"") == u8"");
  REQUIRE(to_utf8(U"") == u8"");
}

TEST_CASE("ascii subset")
{
  REQUIRE(to_utf8(u"abc123\u007F") == u8"abc123\u007F");
  REQUIRE(to_utf8(U"abc123\u007F") == u8"abc123\u007F");
}

TEST_CASE("2byte utf8")
{
  REQUIRE(to_utf8(u"\u0080") == u8"\u0080");
  REQUIRE(to_utf8(u"\u07FF") == u8"\u07FF");
  REQUIRE(to_utf8(U"\u0080") == u8"\u0080");
  REQUIRE(to_utf8(U"\u07FF") == u8"\u07FF");
}

TEST_CASE("3byte utf8")
{
  REQUIRE(to_utf8(u"\u0800") == u8"\u0800");
  REQUIRE(to_utf8(u"\uFFFF") == u8"\uFFFF");
  REQUIRE(to_utf8(U"\u0800") == u8"\u0800");
  REQUIRE(to_utf8(U"\uFFFF") == u8"\uFFFF");
}

TEST_CASE("4byte utf8")
{
  REQUIRE(to_utf8(u"\U00010000") == u8"\U00010000");
  REQUIRE(to_utf8(u"\U0010FFFF") == u8"\U0010FFFF");
  REQUIRE(to_utf8(U"\U00010000") == u8"\U00010000");
  REQUIRE(to_utf8(U"\U0010FFFF") == u8"\U0010FFFF");
}

TEST_CASE("to utf16")
{
  REQUIRE(to_utf16(u8"") == u"");
  REQUIRE(to_utf16(u8"abc123\u007F") == u"abc123\u007F");
  REQUIRE(to_utf16(u8"\u0080") == u"\u0080");
  REQUIRE(to_utf16(u8"\u07FF") == u"\u07FF");
  REQUIRE(to_utf16(u8"\u0800") == u"\u0800");
  REQUIRE(to_utf16(u8"\uFFFF") == u"\uFFFF");
  REQUIRE(to_utf16(u8"\U00010000") == u"\U00010000");
  REQUIRE(to_utf16(u8"\U0010FFFF") == u"\U0010FFFF");
  REQUIRE(to_utf16(u8"\U00024F5C") == u"\U00024F5C");
}

TEST_CASE("get next char from utf8 empty")
{
  char32_t c;
  std::string_view input = "";
  REQUIRE_FALSE(get_next_char_from_utf8(input, c));
}

inline void validate_get_next_char_from_utf8(std::string_view str, char32_t expected)
{
  char32_t c;
  REQUIRE(get_next_char_from_utf8(str, c));
  REQUIRE(c == expected);
  REQUIRE(str.empty());
}
TEST_CASE("get char from utf8")
{
  validate_get_next_char_from_utf8(u8"\U00000001", U'\U00000001');
  validate_get_next_char_from_utf8(u8"\U0000007F", U'\U0000007F');
  validate_get_next_char_from_utf8(u8"\U00000080", U'\U00000080');
  validate_get_next_char_from_utf8(u8"\U000007FF", U'\U000007FF');
  validate_get_next_char_from_utf8(u8"\U00000800", U'\U00000800');
  validate_get_next_char_from_utf8(u8"\U0000FFFF", U'\U0000FFFF');
  validate_get_next_char_from_utf8(u8"\U00010000", U'\U00010000');
  validate_get_next_char_from_utf8(u8"\U0010FFFF", U'\U0010FFFF');
}

inline void validate_get_next_char_from_utf8_invalid(std::string_view str)
{
  char32_t c;
  REQUIRE_FALSE(get_next_char_from_utf8(str, c));
  REQUIRE(str.empty());
}
TEST_CASE("disallow invalid utf8")
{
  // missing following character
  validate_get_next_char_from_utf8_invalid("\x80");
  // invalid following character
  validate_get_next_char_from_utf8_invalid({ "\xC2\x00", 2});
  validate_get_next_char_from_utf8_invalid("\xC2\x7F");
  validate_get_next_char_from_utf8_invalid("\xC2\xC0");
  validate_get_next_char_from_utf8_invalid("\xC2\xFF");
  // surrogate pairs
  validate_get_next_char_from_utf8_invalid("\xED\xA0\x80");
  validate_get_next_char_from_utf8_invalid("\xED\xBF\xBF");
  // overlong
  validate_get_next_char_from_utf8_invalid("\xE0\x82\xA9"); // © (copyright) U+00A9 with 3 instead of 2 chars encoded
  validate_get_next_char_from_utf8_invalid("\xF0\x82\x82\xAC"); // € (euro)  U+20AC with 4 instead of 3 chars encoded
  // invalid chars (first is disallowed, rest is dummy data)
  validate_get_next_char_from_utf8_invalid("\xC0\x80");
  validate_get_next_char_from_utf8_invalid("\xC1\x80\x80");
  validate_get_next_char_from_utf8_invalid("\xF5\x80\x80\x80");
  // not valid sequence start chars
  validate_get_next_char_from_utf8_invalid("\x80\x80");
  validate_get_next_char_from_utf8_invalid("\xBF\x80");
  // greater than allowed codepoints
  validate_get_next_char_from_utf8_invalid("\xF4\x90\x80\x80"); // U+0010FFFF+1
}

inline void validate_ranged_for(std::string_view str, std::deque<char32_t> expected)
{
  Codepoints cp{ str };
  for (auto const& c : cp)
  {
    REQUIRE_FALSE(expected.empty());
    REQUIRE(c == expected[0]);
    expected.pop_front();
  }
  REQUIRE(expected.empty());
  //REQUIRE(cp.text.empty());
}
TEST_CASE("iterate range based for")
{
  validate_ranged_for("abc", { U'a', U'b', U'c' });
  validate_ranged_for(u8"\U00000080\U000007FF\U00000800\U0000FFFF\U00010000\U0010FFFF",
    { U'\U00000080', U'\U000007FF' , U'\U00000800' , U'\U0000FFFF' , U'\U00010000' , U'\U0010FFFF' });
}
