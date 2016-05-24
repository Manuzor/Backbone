// #include <Backbone/FixedBlock.hpp>
#include "../../Backbone.hpp"

#include "catch.hpp"


TEST_CASE("Fixed Block", "[FixedBlock]")
{
  fixed_block<12, int> Integers;
  REQUIRE(Integers.Num == 12);
  REQUIRE(Integers.Data == &Integers[0]);
}

