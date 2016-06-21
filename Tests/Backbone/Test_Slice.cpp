#include <Backbone/Slice.hpp>

#include "catch.hpp"


TEST_CASE("Slice creation", "[Slice]")
{
  SECTION("Uninitialized slices ")
  {
    slice<int> Foo = {};
    REQUIRE(Foo.Num == 0);
    REQUIRE(Foo.Ptr == nullptr);
  }

  SECTION("From number and pointer")
  {
    int Foo = 42;
    auto FooSlice = Slice(1, &Foo);
    REQUIRE(FooSlice.Num == 1);
    REQUIRE(FooSlice[0] == 42);
  }

  SECTION("From static array")
  {
    int Foo[] = {0, 1, 2};
    auto Bar = Slice(Foo);
    REQUIRE(Bar.Num == ArrayCount(Foo));
    REQUIRE(Bar.Ptr == &Foo[0]);
  }

  SECTION("From const string")
  {
    const char CharArray[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0' };
    const char* String = &CharArray[0];

    auto Foo = SliceFromString(CharArray);
    REQUIRE(Foo.Num == ArrayCount(CharArray) - 1);
    REQUIRE(Foo.Ptr == &CharArray[0]);

    auto Bar = SliceFromString(String);
    REQUIRE(Bar.Num == ArrayCount(CharArray) - 1);
    REQUIRE(Bar.Ptr == String);
  }

  SECTION("From mutable string")
  {
    char CharArray[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0' };
    char* String = &CharArray[0];

    auto Foo = SliceFromString(CharArray);
    REQUIRE(Foo.Num == ArrayCount(CharArray) - 1);
    REQUIRE(Foo.Ptr == &CharArray[0]);

    auto Bar = SliceFromString(String);
    REQUIRE(Bar.Num == ArrayCount(CharArray) - 1);
    REQUIRE(Bar.Ptr == String);
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

  SECTION("Set Ptr")
  {
    Foo.Ptr = Coerce<int*, uint64>(42);
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
    auto Bar = Slice(1, &Foo);
    auto Baz = SliceReinterpret<int>(Bar);
    REQUIRE(Baz[0] == *reinterpret_cast<int*>(&Bar[0]));
  }
}

TEST_CASE("Slice Equality", "[Slice]")
{
  SECTION("Both Empty")
  {
    REQUIRE(slice<void>()  == slice<void>());
    REQUIRE(slice<int32>() == slice<int32>());
    REQUIRE(slice<int16>() == slice<int32>());
  }

  SECTION("Single Value")
  {
    int A = 42;
    int B = 123;
    int C = 42;
    auto Foo = Slice(1, &A);
    auto Bar = Slice(1, &B);
    auto Baz = Slice(1, &C);

    REQUIRE(Foo == Foo);
    REQUIRE(Foo != Bar);
    REQUIRE(Foo == Baz);
  }

  SECTION("From Same Data")
  {
    int Data[] = { 1, 2, 1, 2, };
    auto Foo = Slice(2, &Data[0]);
    auto Bar = Slice(2, &Data[1]);
    auto Baz = Slice(2, &Data[2]);

    REQUIRE(Foo == Foo);
    REQUIRE(Foo != Bar);
    REQUIRE(Foo == Baz);
  }
}

TEST_CASE("Slice Searching", "[Slice]")
{
  int Ints[] = { 0, 1, 2, 3, 4, 5, 6 };
  auto Foo = Slice(Ints);

  SECTION("CountUntil")
  {
    REQUIRE( SliceCountUntil(Slice<int>((size_t)0, nullptr), 42) == INVALID_INDEX );

    REQUIRE( SliceCountUntil(Foo, 0) == 0 );
    REQUIRE( SliceCountUntil(Foo, 2) == 2 );
    REQUIRE( SliceCountUntil(Foo, 6) == 6 );
    REQUIRE( SliceCountUntil(Foo, 7) == INVALID_INDEX );
  }
}
