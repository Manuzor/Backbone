#include <Backbone/Slice.hpp>
#include "catch.hpp"


TEST_CASE("Slice creation", "[Slice]")
{
  SECTION("Uninitialized slices ")
  {
    slice<int> Foo = {};
    REQUIRE(Foo.Num == 0);
    REQUIRE(Foo.Data == nullptr);
  }

  SECTION("From number and pointer")
  {
    int Foo = 42;
    auto FooSlice = CreateSlice(1, &Foo);
    REQUIRE(FooSlice.Num == 1);
    REQUIRE(FooSlice[0] == 42);
  }

  SECTION("From static array")
  {
    int Foo[] = {0, 1, 2};
    auto Bar = CreateSlice(Foo);
    REQUIRE(Bar.Num == ArrayCount(Foo));
    REQUIRE(Bar.Data == &Foo[0]);
  }

  SECTION("From string")
  {
    const char CharArray[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0' };
    const char* String = &CharArray[0];

    auto Foo = CreateSliceFromString(CharArray);
    REQUIRE(Foo.Num == ArrayCount(CharArray) - 1);
    REQUIRE(Foo.Data == &CharArray[0]);

    auto Bar = CreateSliceFromString(String);
    REQUIRE(Bar.Num == ArrayCount(CharArray) - 1);
    REQUIRE(Bar.Data == String);
  }
}

TEST_CASE("Slice implicit bool conversion", "[Slice]")
{
  slice<int> Foo = {};
  REQUIRE(Cast<bool>(Foo) == false);

  SECTION("Set Num")
  {
    Foo.Num = 42;
    REQUIRE(Cast<bool>(Foo) == false);
  }

  SECTION("Set Data")
  {
    Foo.Data = Coerce<int*, uint64>(42);
    REQUIRE(Cast<bool>(Foo) == false);
  }
}

TEST_CASE("Slices of void", "[Slice]")
{
  // slice<void> Foo;
  // slice<void const> Bar;
}

TEST_CASE("Slice casting", "[Slice]")
{
  SECTION("Reinterpretation")
  {
    float Foo = 3.1415f;
    auto Bar = CreateSlice(1, &Foo);
    auto Baz = SliceReinterpret<int>(Bar);
    REQUIRE(Baz[0] == *reinterpret_cast<int*>(&Bar[0]));
  }
}
