#include <Backbone/Slice.hpp>
#include <Backbone/FixedBlock.hpp>
#include <Backbone/Path.hpp>

#include "catch.hpp"


TEST_CASE("Concat Paths", "[Path]")
{
  auto Head = SliceFromString("head");
  auto Tail = SliceFromString("tail");
  fixed_block<256, char> Buffer;

  SECTION("Default")
  {
    auto Result = ConcatPaths(Head, Tail, Slice(Buffer));
    REQUIRE(Result == SliceFromString("head\\tail"));
  }
}
