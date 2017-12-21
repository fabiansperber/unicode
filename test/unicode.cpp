#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <unicode.h>

using unicode::to_utf8;

TEST_CASE("empty string")
{
  REQUIRE(to_utf8(u"") == u8"");
}

TEST_CASE("ascii subset")
{
  REQUIRE(to_utf8(u"abc123\u007F") == u8"abc123\u007F");
}

TEST_CASE("2byte utf8")
{
  REQUIRE(to_utf8(u"\u0080") == u8"\u0080");
  REQUIRE(to_utf8(u"\u07FF") == u8"\u07FF");
}

TEST_CASE("3byte utf8")
{
  REQUIRE(to_utf8(u"\u0800") == u8"\u0800");
  REQUIRE(to_utf8(u"\uFFFF") == u8"\uFFFF");
}

TEST_CASE("4byte utf8")
{
  REQUIRE(to_utf8(u"\U00010000") == u8"\U00010000");
  REQUIRE(to_utf8(u"\U0010FFFF") == u8"\U0010FFFF");
}
