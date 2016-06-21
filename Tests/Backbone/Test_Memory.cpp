#include <Backbone/Memory.hpp>

#include "catch.hpp"

namespace
{
  struct FooPOD {};
  struct BarPOD { FooPOD Foo; };
  struct BazNoPOD { BazNoPOD() { /* Default ctor */ } };
}

static_assert(IsPOD<int8>(),   "Expected int8 to be a POD type!");
static_assert(IsPOD<int16>(),  "Expected int16 to be a POD type!");
static_assert(IsPOD<int32>(),  "Expected int32 to be a POD type!");
static_assert(IsPOD<int64>(),  "Expected int64 to be a POD type!");
static_assert(IsPOD<uint8>(),  "Expected uint8 to be a POD type!");
static_assert(IsPOD<uint16>(), "Expected uint16 to be a POD type!");
static_assert(IsPOD<uint32>(), "Expected uint32 to be a POD type!");
static_assert(IsPOD<uint64>(), "Expected uint64 to be a POD type!");

static_assert(IsPOD<FooPOD>(), "Expected FooPOD to be a POD type!");
static_assert(IsPOD<BarPOD>(), "Expected BarPOD to be a POD type!");
static_assert(!IsPOD<BazNoPOD>(), "Expected BazNoPOD to be no POD type!");

TEST_CASE("MemDefaultConstruct", "[Memory]")
{
  int Ints[4] = { 1, 1, 1, 1 };

  MemDefaultConstruct(&Ints[0], 4);
  REQUIRE( Ints[0] == 0 );
  REQUIRE( Ints[1] == 0 );
  REQUIRE( Ints[2] == 0 );
  REQUIRE( Ints[3] == 0 );
}
