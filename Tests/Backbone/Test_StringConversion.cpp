#include <Backbone/StringConversion.hpp>
#include <Backbone/Common.hpp>

#include "catch.hpp"

static void
TestFloat(char const* String, double ExpectedValue, int ExpectedRemainingNumStringElements)
{
  auto Source = SliceFromString(String);
  bool Success{};
  double ConvertedValue = Convert<double>(&Source, &Success);
  REQUIRE( Success );
  REQUIRE( AreNearlyEqual(ConvertedValue, ExpectedValue, 1e-10) );
  REQUIRE( Source.Num == ExpectedRemainingNumStringElements );
}

TEST_CASE("String Conversion: String -> Floating Point", "[StringConversion]")
{
  TestFloat("1", 1.0f, 0);
  TestFloat("-1", -1.0f, 0);
  TestFloat("1.5", 1.5f, 0);
  TestFloat("-1.5", -1.5f, 0);
  TestFloat("1E10", 1e10f, 0);
  TestFloat("1E-10", 1e-10f, 0);
  TestFloat("-1E-10", -1e-10f, 0);
  TestFloat("-1E10", -1e10f, 0);
  TestFloat("1.234E-10", 1.234e-10f, 0);
  TestFloat("-1.234E-10", -1.234e-10f, 0);

  TestFloat("1e10", 1e10f, 0);
  TestFloat("1e-10", 1e-10f, 0);
  TestFloat("-1e-10", -1e-10f, 0);
  TestFloat("-1e10", -1e10f, 0);
  TestFloat("1.234e-10", 1.234e-10f, 0);
  TestFloat("-1.234e-10", -1.234e-10f, 0);

  TestFloat("23443A", 23443, 1);
  TestFloat("  23443A", 23443, 1);
  TestFloat("\n \r  \t23443A", 23443, 1);
  TestFloat("76.55.43", 76.55, 3);

  auto String = SliceFromString("ABC");
  bool Success{};
  auto Value = Convert<double>(&String, &Success);
  REQUIRE( !Success );
  REQUIRE( IsNaN(Value) );
  REQUIRE( String.Num == 3 );
}

static void
TestInt(char const* String, int64 ExpectedValue, int ExpectedRemainingNumStringElements)
{
  auto Source = SliceFromString(String);
  bool Success{};
  int64 ConvertedValue = Convert<int64>(&Source, &Success);
  REQUIRE( Success );
  REQUIRE( ConvertedValue == ExpectedValue );
  REQUIRE( Source.Num == ExpectedRemainingNumStringElements );
}

TEST_CASE("String Conversion: String -> Integer", "[StringConversion]")
{
  TestInt("1", 1, 0);
  TestInt("-1", -1, 0);
  TestInt("400000000", 400000000, 0);
  TestInt("-400000000", -400000000, 0);
  TestInt("23443A", 23443, 1);
  TestInt("  23443A", 23443, 1);
  TestInt("\n \r  \t23443A", 23443, 1);
  TestInt("76.55.43", 76, 6);

  auto String = SliceFromString("ABC");
  bool Success{};
  int64 Value = Convert<int64>(&String, &Success);
  REQUIRE( !Success );
  REQUIRE( Value == IntMaxValue<int64>() );
  REQUIRE( String.Num == 3 );
}
