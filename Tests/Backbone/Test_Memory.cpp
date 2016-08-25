#include <Backbone/FixedBlock.hpp>
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

TEST_CASE("Memory Construction", "[Memory]")
{
  SECTION("Plain Old Data (POD)")
  {
    static_assert(IsPOD<int>(), "int must be POD!");
    fixed_block<4, int> Pods;

    SECTION("Default construct array")
    {
      MemConstruct(Pods.Num, &Pods[0]);
      REQUIRE( Pods[0] == 0 );
      REQUIRE( Pods[1] == 0 );
      REQUIRE( Pods[2] == 0 );
      REQUIRE( Pods[3] == 0 );
    }

    SECTION("Construct array with value")
    {
      MemConstruct(Pods.Num, &Pods[0], 42);
      REQUIRE( Pods[0] == 42 );
      REQUIRE( Pods[1] == 42 );
      REQUIRE( Pods[2] == 42 );
      REQUIRE( Pods[3] == 42 );

      SECTION("Destruct array")
      {
        MemDestruct(Pods.Num, &Pods[0]);
        // Should not do anything for PODs.
        REQUIRE( Pods[0] == 42 );
        REQUIRE( Pods[1] == 42 );
        REQUIRE( Pods[2] == 42 );
        REQUIRE( Pods[3] == 42 );
      }
    }
  }

  SECTION("Non POD")
  {
    struct counters
    {
      int Ctor{};
      int Dtor{};
    };

    static counters* Counters;
    counters CountersInstance;
    Counters = &CountersInstance;
    Defer[](){ Counters = nullptr; };

    struct foo
    {
      uint8 Data;

      foo() { if(Counters) ++Counters->Ctor; }
      foo(uint8 Data) : foo() { this->Data = Data; }
      ~foo() { if(Counters) ++Counters->Dtor; }
    };
    static_assert(!IsPOD<foo>(), "foo must NOT be POD!");
    static_assert(SizeOf<foo>() == 1, "Size of foo must be exactly 1 byte.");


    fixed_block<4, foo> Foos;
    REQUIRE( Counters->Ctor == 1 * Convert<int>(Foos.Num) );
    REQUIRE( Counters->Dtor == 0 * Convert<int>(Foos.Num) );

    MemSetBytes(Foos.Num * SizeOf<foo>(), &Foos[0], 0);

    MemConstruct(Foos.Num, &Foos.Data[0]);
    REQUIRE( Counters->Ctor == 2 * Convert<int>(Foos.Num) );
    REQUIRE( Counters->Dtor == 0 * Convert<int>(Foos.Num) );
    REQUIRE( Foos[0].Data == 0 );
    REQUIRE( Foos[1].Data == 0 );
    REQUIRE( Foos[2].Data == 0 );
    REQUIRE( Foos[3].Data == 0 );

    MemConstruct(Foos.Num, &Foos.Data[0], uint8(42));
    REQUIRE( Counters->Ctor == 3 * Convert<int>(Foos.Num) );
    REQUIRE( Counters->Dtor == 0 * Convert<int>(Foos.Num) );
    REQUIRE( Foos[0].Data == 42 );
    REQUIRE( Foos[1].Data == 42 );
    REQUIRE( Foos[2].Data == 42 );
    REQUIRE( Foos[3].Data == 42 );

    MemDestruct(Foos.Num, &Foos.Data[0]);
    REQUIRE( Counters->Ctor == 3 * Convert<int>(Foos.Num) );
    REQUIRE( Counters->Dtor == 1 * Convert<int>(Foos.Num) );
    REQUIRE( Foos[0].Data == 42 );
    REQUIRE( Foos[1].Data == 42 );
    REQUIRE( Foos[2].Data == 42 );
    REQUIRE( Foos[3].Data == 42 );
  }
}

TEST_CASE("Memory Relocation", "[Memory]")
{
  SECTION("POD types")
  {
    fixed_block<6, int> Ints;
    Ints[0] = 0;
    Ints[1] = 1;
    Ints[2] = 2;
    Ints[3] = 3;
    Ints[4] = 4;
    Ints[5] = 5;

    SECTION("Non overlapping")
    {
      size_t Num = 3;
      auto A = &Ints[0];
      auto B = &Ints[Num];

      MemMove(Num, A, B);
      REQUIRE( Ints[0] == 3 );
      REQUIRE( Ints[1] == 4 );
      REQUIRE( Ints[2] == 5 );
      // The other Ints are considered "destructed" now.
    }

    SECTION("Overlapping")
    {
      size_t Num = 3;
      auto A = &Ints[0];
      auto B = &Ints[2];
      MemMove(Num, A, B);
      REQUIRE( Ints[0] == 2 );
      REQUIRE( Ints[1] == 3 );
      REQUIRE( Ints[2] == 4 );
      REQUIRE( Ints[5] == 5 );
    }
  }

  SECTION("Non POD types")
  {
    struct foo
    {
      int Id = 0;
      int MovedIn = 0;
      int Mover = 0;
      bool IsAlive = true;

      foo() = default;

      void operator =(foo&& ToMove)
      {
        this->MovedIn = ToMove.Id;
        ToMove.Mover = this->Id;
      }

      ~foo()
      {
        this->IsAlive = false;
      }
    };

    fixed_block<6, foo> Foos;
    Foos[0].Id = 1;
    Foos[1].Id = 2;
    Foos[2].Id = 3;
    Foos[3].Id = 4;
    Foos[4].Id = 5;
    Foos[5].Id = 6;

    SECTION("Non overlapping")
    {
      size_t Num = 3;
      auto A = &Foos[0];
      auto B = &Foos[Num];
      MemMove(Num, A, B);
      REQUIRE(  Foos[0].Id == 1 );
      REQUIRE(  Foos[0].IsAlive );
      REQUIRE(  Foos[0].MovedIn == 4 );
      REQUIRE(  Foos[0].Mover == 0 );
      REQUIRE(  Foos[1].Id == 2 );
      REQUIRE(  Foos[1].IsAlive );
      REQUIRE(  Foos[1].MovedIn == 5 );
      REQUIRE(  Foos[1].Mover == 0 );
      REQUIRE(  Foos[2].Id == 3 );
      REQUIRE(  Foos[2].IsAlive );
      REQUIRE(  Foos[2].MovedIn == 6 );
      REQUIRE(  Foos[2].Mover == 0 );
      REQUIRE(  Foos[3].Id == 4 );
      REQUIRE( !Foos[3].IsAlive );
      REQUIRE(  Foos[3].MovedIn == 0 );
      REQUIRE(  Foos[3].Mover == 1 );
      REQUIRE(  Foos[4].Id == 5 );
      REQUIRE( !Foos[4].IsAlive );
      REQUIRE(  Foos[4].MovedIn == 0 );
      REQUIRE(  Foos[4].Mover == 2 );
      REQUIRE(  Foos[5].Id == 6 );
      REQUIRE( !Foos[5].IsAlive );
      REQUIRE(  Foos[5].MovedIn == 0 );
      REQUIRE(  Foos[5].Mover == 3 );
    }

    SECTION("overlapping")
    {
      size_t Num = 3;
      auto A = &Foos[1];
      auto B = &Foos[3];
      MemMove(Num, A, B);
      REQUIRE(  Foos[0].Id == 1 );
      REQUIRE(  Foos[0].IsAlive );
      REQUIRE(  Foos[0].MovedIn == 0 );
      REQUIRE(  Foos[0].Mover == 0 );
      REQUIRE(  Foos[1].Id == 2 );
      REQUIRE(  Foos[1].IsAlive );
      REQUIRE(  Foos[1].MovedIn == 4 );
      REQUIRE(  Foos[1].Mover == 0 );
      REQUIRE(  Foos[2].Id == 3 );
      REQUIRE(  Foos[2].IsAlive );
      REQUIRE(  Foos[2].MovedIn == 5 );
      REQUIRE(  Foos[2].Mover == 0 );
      REQUIRE(  Foos[3].Id == 4 );
      REQUIRE(  Foos[3].IsAlive );
      REQUIRE(  Foos[3].MovedIn == 6 );
      REQUIRE(  Foos[3].Mover == 2 );
      REQUIRE(  Foos[4].Id == 5 );
      REQUIRE( !Foos[4].IsAlive );
      REQUIRE(  Foos[4].MovedIn == 0 );
      REQUIRE(  Foos[4].Mover == 3 );
      REQUIRE(  Foos[5].Id == 6 );
      REQUIRE( !Foos[5].IsAlive );
      REQUIRE(  Foos[5].MovedIn == 0 );
      REQUIRE(  Foos[5].Mover == 4 );
    }
  }
}
