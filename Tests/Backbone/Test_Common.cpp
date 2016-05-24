// #include <Backbone/Common.hpp>
#include "../../Backbone.hpp"

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
    REQUIRE(ArrayCount("Foo") == 4);
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

TEST_CASE("Casts", "[Common]")
{
  SECTION("Regular casting")
  {
    REQUIRE(Cast<float>(1) == 1.0f);
    REQUIRE(Cast<CastDataInt>(CastDataFloat()).Data == 42);
  }

  SECTION("Reinterpretation")
  {
    int Data;
    REQUIRE(Reinterpret<void*>(&Data) == (void*)&Data);
  }

  SECTION("Coercion")
  {
    REQUIRE(Coerce<float>((int)42) == (float)((int)42));
  }

  SECTION("As const")
  {
    int Data;
    REQUIRE(casts::helper(Data) == 42);
    REQUIRE(casts::helper(AsConst(Data)) == 64);
    REQUIRE(casts::helper(&Data) == 128);
    REQUIRE(casts::helper(AsPtrToConst(&Data)) == 512);
  }
}
