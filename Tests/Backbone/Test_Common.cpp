#include <Backbone/Common.hpp>

#include "catch.hpp"

static_assert(IsPOD<void>(), "Type 'void' must be considered POD.");
static_assert(IsPOD<void const>(), "Type 'void const' must be considered POD.");
static_assert(IsNaN(NaN<float>()), "NaN() or IsNaN() aren't working properly.");
static_assert(IsNaN(NaN<double>()), "NaN() or IsNaN() aren't working properly.");

TEST_CASE("Defer", "[Common]")
{
  SECTION("POD")
  {
    int Value = 0;

    Defer [&](){ REQUIRE(Value == 3); };
    Defer [&](){ ++Value; };
    Defer [&](){ ++Value; };
    Defer [=](){ REQUIRE( Value == 0 ); };
    Defer [&](){ ++Value; };
  }

  SECTION("Complex capture")
  {
    struct foo
    {
      int Value = 0;
      foo() = default;
      foo(foo const& Other) { FAIL( "Copy constructor is not supposed to be called." ); }
      ~foo() { REQUIRE( Value == 42 ); }
    };

    struct bar
    {
      int Value = 1337;
      bar() = default;
      ~bar() { REQUIRE( Value == 1337 ); }
    };

    foo Foo;
    bar Bar;

    Defer [&](){ Foo.Value = 42; Bar = {}; };
    Defer [Bar](){ REQUIRE( Bar.Value == 1337 ); };
    Defer [&Bar](){ REQUIRE( Bar.Value == 123 ); };
    Defer [=, &Foo](){ REQUIRE( Foo.Value == 3 ); REQUIRE( Bar.Value == 1337 ); };

    Foo.Value = 3;

    Bar.Value = 123;
  }
}

TEST_CASE("Byte Sizes", "[Common]")
{
  REQUIRE( ToBytes(KiB(3)) == 3 * 1024ULL );
  REQUIRE( ToBytes(MiB(3)) == 3 * 1024ULL * 1024ULL );
  REQUIRE( ToBytes(GiB(3)) == 3 * 1024ULL * 1024ULL * 1024ULL );
  REQUIRE( ToBytes(TiB(3)) == 3 * 1024ULL * 1024ULL * 1024ULL * 1024ULL );

  REQUIRE( ToBytes(KB(3)) == 3 * 1000ULL );
  REQUIRE( ToBytes(MB(3)) == 3 * 1000ULL * 1000ULL );
  REQUIRE( ToBytes(GB(3)) == 3 * 1000ULL * 1000ULL * 1000ULL );
  REQUIRE( ToBytes(TB(3)) == 3 * 1000ULL * 1000ULL * 1000ULL * 1000ULL );

  REQUIRE( ToKiB(Bytes(512)) == 0.5f );
  REQUIRE( ToKiB<uint64>(Bytes(512)) == 0 );
  REQUIRE( ToKiB<uint32>(Bytes(512)) == 0 );
  REQUIRE( ToKiB<uint16>(Bytes(512)) == 0 );
  REQUIRE( ToKiB<uint8>(Bytes(512)) == 0 );

  REQUIRE( ToKiB(Bytes(2 * 1024)) == 2.0f );
  REQUIRE( ToKiB<uint64>(Bytes(2 * 1024)) == 2 );
  REQUIRE( ToKiB<uint32>(Bytes(2 * 1024)) == 2 );
  REQUIRE( ToKiB<uint16>(Bytes(2 * 1024)) == 2 );
  REQUIRE( ToKiB<uint8>(Bytes(2 * 1024)) == 2 );
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

TEST_CASE("IsPowerOfTwo", "[Common]")
{
  size_t Iteration = 0;
  for(size_t Index = 0; Index < NumBits<size_t>(); ++Index)
  {
    CAPTURE( Index );
    auto Integer = size_t(1) << Index;
    CAPTURE( Integer );
    REQUIRE( IsPowerOfTwo(Integer) );
  }

  auto NegativeTest = [](size_t Argument)
  {
    CAPTURE(Argument);
    REQUIRE( !IsPowerOfTwo(Argument) );
  };

  NegativeTest( 3 );
  NegativeTest( 5 );
  NegativeTest( 6 );
  NegativeTest( 12 );
  NegativeTest( 1024-1 );
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

namespace
{
  struct SomeDataCounter
  {
    int DefaultConstructions = 0;
    int CopyConstructions = 0;
    int MoveConstructions = 0;
    int Destructions = 0;
  };

  struct SomeData
  {
    static SomeDataCounter* Counter;

    SomeData()                { Counter->DefaultConstructions++; }
    SomeData(SomeData const&) { Counter->CopyConstructions++; }
    SomeData(SomeData&&)      { Counter->MoveConstructions++; }
    ~SomeData()               { Counter->Destructions++; }
  };
  SomeDataCounter* SomeData::Counter = nullptr;

  void TestCopy(SomeData Something) {}
  void TestMove(SomeData&& Something) {}
}

TEST_CASE("Move", "[Common]")
{
  SomeDataCounter Counter;
  SomeData::Counter = &Counter;
  Defer [](){ SomeData::Counter = nullptr; };

  SomeData Something;
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 0 );
  REQUIRE( Counter.MoveConstructions    == 0 );
  REQUIRE( Counter.Destructions         == 0 );

  TestCopy(Something);
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 0 );
  REQUIRE( Counter.Destructions         == 1 );

  TestMove(Move(Something));
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 0 );
  REQUIRE( Counter.Destructions         == 1 );

  TestCopy(Move(Something));
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 1 );
  REQUIRE( Counter.Destructions         == 2 );

  TestCopy(SomeData());
  REQUIRE( Counter.DefaultConstructions == 2 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 1 );
  REQUIRE( Counter.Destructions         == 3 );

  TestMove(Move(SomeData()));
  REQUIRE( Counter.DefaultConstructions == 3 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 1 );
  REQUIRE( Counter.Destructions         == 4 );
}

struct my_conversion
{
  struct foo {};
  struct bar {};

  static constexpr int
  Do(foo)
  {
    return 42;
  }

  static constexpr int
  Do(bar)
  {
    return 1337;
  }
};

template<> struct impl_convert<int, my_conversion::foo> : public my_conversion {};
template<> struct impl_convert<int, my_conversion::bar> : public my_conversion {};

TEST_CASE("General Conversion", "[Common]")
{
  REQUIRE(Convert<int>(my_conversion::foo()) == 42);
  REQUIRE(Convert<int>(my_conversion::bar()) == 1337);
}

TEST_CASE("Float UNorm SNorm Conversion", "[Common]")
{
  SECTION("uint8")
  {
    // Note: We cast to uint32 so the output doesn't get treated as a char but
    //       as an integer value.
    REQUIRE( (uint32)FloatToUNorm<uint8>(0.0f)   == 0 );
    REQUIRE( (uint32)FloatToUNorm<uint8>(1.0f)   == 255 );
    REQUIRE( (uint32)FloatToUNorm<uint8>(0.5f)   == 128 );
    REQUIRE( (uint32)FloatToUNorm<uint8>(0.2f)   == 51 );
    REQUIRE( (uint32)FloatToUNorm<uint8>(0.25f)  == 64 );

    REQUIRE( UNormToFloat(0) == 0.0f );
    REQUIRE( UNormToFloat(IntMaxValue<uint8>()) == 1.0f );
    REQUIRE( AreNearlyEqual(UNormToFloat<uint8>(IntMaxValue<uint8>() / 2), 0.5f, 0.005f) );

    //
    // Test Clamping
    //
    REQUIRE( (uint32)FloatToUNorm<uint8>(2.0f)   == 255 );
    REQUIRE( (uint32)FloatToUNorm<uint8>(-1.0f)  == 0 );
  }
}

TEST_CASE("Sign", "[Common]")
{
  SECTION("uint8")
  {
    REQUIRE( Sign(uint8( 42)) ==  1 );
    REQUIRE( Sign(uint8( 0)) ==  0 );
  }

  SECTION("uint16")
  {
    REQUIRE( Sign(uint16( 42)) ==  1 );
    REQUIRE( Sign(uint16( 0)) ==  0 );
  }

  SECTION("uint32")
  {
    REQUIRE( Sign(uint32( 42)) ==  1 );
    REQUIRE( Sign(uint32( 0)) ==  0 );
  }

  SECTION("uint64")
  {
    REQUIRE( Sign(uint64( 42)) ==  1 );
    REQUIRE( Sign(uint64( 0)) ==  0 );
  }

  SECTION("int8")
  {
    REQUIRE( Sign(int8(-42)) == -1 );
    REQUIRE( Sign(int8( 42)) ==  1 );
    REQUIRE( Sign(int8( 0)) ==  0 );
  }

  SECTION("int16")
  {
    REQUIRE( Sign(int16(-42)) == -1 );
    REQUIRE( Sign(int16( 42)) ==  1 );
    REQUIRE( Sign(int16( 0)) ==  0 );
  }

  SECTION("int32")
  {
    REQUIRE( Sign(int32(-42)) == -1 );
    REQUIRE( Sign(int32( 42)) ==  1 );
    REQUIRE( Sign(int32( 0)) ==  0 );
  }

  SECTION("int64")
  {
    REQUIRE( Sign(int64(-42)) == -1 );
    REQUIRE( Sign(int64( 42)) ==  1 );
    REQUIRE( Sign(int64( 0)) ==  0 );
  }

  SECTION("float")
  {
    REQUIRE( Sign(-42.1337f) == -1 );
    REQUIRE( Sign( 42.1337f) ==  1 );
    REQUIRE( Sign( 0.00000f) ==  0 );
  }

  SECTION("double")
  {
    REQUIRE( Sign(-42.1337) == -1 );
    REQUIRE( Sign( 42.1337) ==  1 );
    REQUIRE( Sign( 0.00000) ==  0 );
  }
}

TEST_CASE("Abs", "[Common]")
{
  SECTION("uint8")
  {
    REQUIRE( Abs(uint8(1)) == 1 );
    REQUIRE( Abs(IntMaxValue<uint8>()) == IntMaxValue<uint8>() );
  }

  SECTION("uint16")
  {
    REQUIRE( Abs(uint16(1)) == 1 );
    REQUIRE( Abs(IntMaxValue<uint16>()) == IntMaxValue<uint16>() );
  }

  SECTION("uint32")
  {
    REQUIRE( Abs(uint32(1)) == 1 );
    REQUIRE( Abs(IntMaxValue<uint32>()) == IntMaxValue<uint32>() );
  }

  SECTION("uint64")
  {
    REQUIRE( Abs(uint64(1)) == 1 );
    REQUIRE( Abs(IntMaxValue<uint64>()) == IntMaxValue<uint64>() );
  }

  SECTION("int8")
  {
    REQUIRE( Abs(int8(1)) == 1 );
    REQUIRE( Abs(int8(-1)) == 1 );
    REQUIRE( Abs(IntMaxValue<int8>()) == IntMaxValue<int8>() );
    REQUIRE( Abs(IntMinValue<int8>() + 1) == IntMaxValue<int8>() );
  }

  SECTION("int16")
  {
    REQUIRE( Abs(int16(1)) == 1 );
    REQUIRE( Abs(int16(-1)) == 1 );
    REQUIRE( Abs(IntMaxValue<int16>()) == IntMaxValue<int16>() );
    REQUIRE( Abs(IntMinValue<int16>() + 1) == IntMaxValue<int16>() );
  }

  SECTION("int32")
  {
    REQUIRE( Abs(int32(1)) == 1 );
    REQUIRE( Abs(int32(-1)) == 1 );
    REQUIRE( Abs(IntMaxValue<int32>()) == IntMaxValue<int32>() );
    REQUIRE( Abs(IntMinValue<int32>() + 1) == IntMaxValue<int32>() );
  }

  SECTION("int64")
  {
    REQUIRE( Abs(int64(1)) == 1 );
    REQUIRE( Abs(int64(-1)) == 1 );
    REQUIRE( Abs(IntMinValue<int64>() + 1) == IntMaxValue<int64>() );
    REQUIRE( Abs(IntMaxValue<int64>()) == IntMaxValue<int64>() );
  }

  SECTION("float")
  {
    REQUIRE( Abs( 1.0f) == 1.0f );
    REQUIRE( Abs(-1.0f) == 1.0f );
  }

  SECTION("double")
  {
    REQUIRE( Abs( 1.0) == 1.0 );
    REQUIRE( Abs(-1.0) == 1.0 );
  }
}


TEST_CASE("Rounding", "[Common]")
{
  SECTION("RoundDown")
  {
    REQUIRE( RoundDown<int>( 3.40f) ==  3 );
    REQUIRE( RoundDown<int>( 3.50f) ==  3 );
    REQUIRE( RoundDown<int>( 3.60f) ==  3 );
    REQUIRE( RoundDown<int>(-3.40f) == -4 );
    REQUIRE( RoundDown<int>(-3.50f) == -4 );
    REQUIRE( RoundDown<int>(-3.60f) == -4 );
  }

  SECTION("RoundUp")
  {
    REQUIRE( RoundUp<int>( 3.40f) ==  4 );
    REQUIRE( RoundUp<int>( 3.50f) ==  4 );
    REQUIRE( RoundUp<int>( 3.60f) ==  4 );
    REQUIRE( RoundUp<int>(-3.40f) == -3 );
    REQUIRE( RoundUp<int>(-3.50f) == -3 );
    REQUIRE( RoundUp<int>(-3.60f) == -3 );
  }

  SECTION("RoundTowardsZero")
  {
    REQUIRE( RoundTowardsZero<int>( 3.40f) ==  3 );
    REQUIRE( RoundTowardsZero<int>( 3.50f) ==  3 );
    REQUIRE( RoundTowardsZero<int>( 3.60f) ==  3 );
    REQUIRE( RoundTowardsZero<int>(-3.40f) == -3 );
    REQUIRE( RoundTowardsZero<int>(-3.50f) == -3 );
    REQUIRE( RoundTowardsZero<int>(-3.60f) == -3 );
  }

  SECTION("RoundAwayFromZero")
  {
    REQUIRE( RoundAwayFromZero<int>( 3.40f) ==  4 );
    REQUIRE( RoundAwayFromZero<int>( 3.50f) ==  4 );
    REQUIRE( RoundAwayFromZero<int>( 3.60f) ==  4 );
    REQUIRE( RoundAwayFromZero<int>(-3.40f) == -4 );
    REQUIRE( RoundAwayFromZero<int>(-3.50f) == -4 );
    REQUIRE( RoundAwayFromZero<int>(-3.60f) == -4 );
  }

  SECTION("Round")
  {
    REQUIRE( Round<int>( 3.40f) ==  3 );
    REQUIRE( Round<int>( 3.50f) ==  4 );
    REQUIRE( Round<int>( 3.60f) ==  4 );
    REQUIRE( Round<int>(-3.40f) == -3 );
    REQUIRE( Round<int>(-3.50f) == -3 );
    REQUIRE( Round<int>(-3.51f) == -4 );
    REQUIRE( Round<int>(-3.60f) == -4 );
  }
}
