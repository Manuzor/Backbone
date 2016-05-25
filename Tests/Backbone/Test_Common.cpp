#include <Backbone/Common.hpp>

#include "catch.hpp"


TEST_CASE("Byte Sizes", "[Common]")
{
  REQUIRE(KiB(3) == 3 * 1024ULL);
  REQUIRE(MiB(3) == 3 * 1024ULL * 1024ULL);
  REQUIRE(GiB(3) == 3 * 1024ULL * 1024ULL * 1024ULL);
  REQUIRE(TiB(3) == 3 * 1024ULL * 1024ULL * 1024ULL * 1024ULL);

  REQUIRE(KB(3) == 3 * 1000ULL);
  REQUIRE(MB(3) == 3 * 1000ULL * 1000ULL);
  REQUIRE(GB(3) == 3 * 1000ULL * 1000ULL * 1000ULL);
  REQUIRE(TB(3) == 3 * 1000ULL * 1000ULL * 1000ULL * 1000ULL);
}

TEST_CASE("Array Count", "[Common]")
{
  SECTION("POD array")
  {
    int Array[42];
    REQUIRE(ArrayCount(Array) == 42);
  }

  SECTION("String literal")
  {
    REQUIRE(ArrayCount("Foo") == 3 + 1);
  }
}

TEST_CASE("Mem offsets", "[Common]")
{
  int32 Data = 42;
  auto DataOffset = (int64)&Data;
  auto Foo = (int32*)&Data;
  auto Bar = (void*)&Data;

  SECTION("Byte offset")
  {
    REQUIRE(MemAddByteOffset(Foo,  2) == (int32*)( (char*)&Data + 2 ));
    REQUIRE(MemAddByteOffset(Foo, -2) == (int32*)( (char*)&Data - 2 ));

    REQUIRE(MemAddByteOffset(Bar,  2) == (char*)&Data + 2);
    REQUIRE(MemAddByteOffset(Bar, -2) == (char*)&Data - 2);

    REQUIRE(MemAddByteOffset(Foo, -DataOffset)     == nullptr);
    REQUIRE(MemAddByteOffset(Foo, -DataOffset - 1) == (int32*)( (char*)nullptr - 1 ));
  }

  SECTION("Sized offset")
  {
    REQUIRE(MemAddOffset(Foo,  2) == Foo + 2);
    REQUIRE(MemAddOffset(Foo, -2) == Foo - 2);

    REQUIRE(MemAddOffset(Bar,  2) == (char*)Bar + 2);
    REQUIRE(MemAddOffset(Bar, -2) == (char*)Bar - 2);
  }
}

struct CastDataInt
{
  int Data = 42;
};

struct CastDataFloat
{
  float Data = 0.5f;
  operator CastDataInt() { return {}; }
};

namespace casts
{
  int helper(int&)       { return 42; }
  int helper(int const&) { return 64; }
  int helper(int*)       { return 128; }
  int helper(int const*) { return 512; }
}

TEST_CASE("Cast", "[Common]")
{
  REQUIRE(Cast<float>(1) == 1.0f);
  REQUIRE(Cast<CastDataInt>(CastDataFloat()).Data == 42);
}

TEST_CASE("Reinterpret", "[Common]")
{
  int Data;
  REQUIRE(Reinterpret<void*>(&Data) == (void*)&Data);
}

TEST_CASE("Coerce", "[Common]")
{
  REQUIRE(Coerce<float>((int)42) == (float)((int)42));
}

TEST_CASE("AsConst", "[Common]")
{
  int Data;
  REQUIRE(casts::helper(Data) == 42);
  REQUIRE(casts::helper(AsConst(Data)) == 64);
  REQUIRE(casts::helper(&Data) == 128);
  REQUIRE(casts::helper(AsPtrToConst(&Data)) == 512);
}

TEST_CASE("Sign", "[Common]")
{
  REQUIRE(Sign(   0) == 0);
  REQUIRE(Sign( 0.0) == 0.0);
  REQUIRE(Sign(0.0f) == 0.0f);

  REQUIRE(Sign( 1.0f) ==  1.0f);
  REQUIRE(Sign(-1.0f) == -1.0f);

  REQUIRE(Sign(uint(42))  == 1);
  REQUIRE(Sign(uint(-42)) == 1); // Underflow.
}

TEST_CASE("Min", "[Common]")
{
  REQUIRE(Min( 0,  1) == 0);
  REQUIRE(Min( 1,  0) == 0);
  REQUIRE(Min(-1,  0) == -1);
  REQUIRE(Min( 0, -1) == -1);

  REQUIRE(Min(uint64(0), float(1)) == uint64(0));
  REQUIRE(Min(uint64(1), float(0)) == uint64(0));
}

TEST_CASE("Max", "[Common]")
{
  REQUIRE(Max( 0,  1) == 1);
  REQUIRE(Max( 1,  0) == 1);
  REQUIRE(Max(-1,  0) == 0);
  REQUIRE(Max( 0, -1) == 0);

  REQUIRE(Max(uint64(0), float(1)) == uint64(1));
  REQUIRE(Max(uint64(1), float(0)) == uint64(1));
}

TEST_CASE("Clamp", "[Common]")
{
  REQUIRE(Clamp(10, 5, 15) == 10);
  REQUIRE(Clamp( 5, 5, 15) ==  5);
  REQUIRE(Clamp(15, 5, 15) == 15);
  REQUIRE(Clamp(20, 5, 15) == 15);
  REQUIRE(Clamp( 0, 5, 15) ==  5);

  REQUIRE(Clamp(10, 15, 5) ==  10);
}

TEST_CASE("Wrap", "[Common]")
{
  REQUIRE(Wrap(10, 5, 15) == 10);
  REQUIRE(Wrap(15, 5, 15) == 5);
  REQUIRE(Wrap(16, 5, 15) == 6);
  REQUIRE(Wrap(5, 5, 15)  == 5);
  REQUIRE(Wrap(4, 5, 15)  == 14);

  REQUIRE(Wrap(  0, -10, 10) ==   0);
  REQUIRE(Wrap(  1, -10, 10) ==   1);
  REQUIRE(Wrap( 10, -10, 10) == -10);
  REQUIRE(Wrap( 15, -10, 10) ==  -5);
  REQUIRE(Wrap(-15, -10, 10) ==   5);

  REQUIRE(Wrap( 30, 5, 15) == 10);  //  30 => 20 => 10
  REQUIRE(Wrap(-10, 5, 15) == 10);  // -10 =>  0 => 10
}

TEST_CASE("NormalizeValue", "[Common]")
{
  REQUIRE(NormalizeValue<float>(15, 10, 30) == 0.25f);
  REQUIRE(NormalizeValue<float>(15, 30, 10) == 0.0f); // Invalid bounds.
}

TEST_CASE("AreNearlyEqual", "[Common]")
{
  REQUIRE( AreNearlyEqual(0.9f, 1.1f,  /* Epsilon: */ 0.200001f));
  REQUIRE(!AreNearlyEqual(0.9f, 1.11f, /* Epsilon: */ 0.2f));
}
