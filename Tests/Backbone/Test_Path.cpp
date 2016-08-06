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

TEST_CASE("Find File Extension", "[Path]")
{
  SECTION("Empty Slice")
  {
    auto Result = FindFileExtension(slice<char>{});
    REQUIRE( Result.Num == 0 );
    REQUIRE( Result.Ptr == nullptr );
  }

  SECTION("Empty String")
  {
    auto EmptyString = SliceFromString("");
    auto Result = FindFileExtension(EmptyString);
    REQUIRE( Result.Num == 0 );
    REQUIRE( Result.Ptr == EmptyString.Ptr );
  }

  SECTION("Real Example")
  {
    auto FileName = SliceFromString("Foo/Bar.Baz");
    auto Result = FindFileExtension(FileName);
    REQUIRE( Result.Num == 4 );
    REQUIRE( Result.Ptr == FileName.Ptr + 7 );

    FileName = SliceFromString("Foo\\Bar.Baz");
    Result = FindFileExtension(FileName);
    REQUIRE( Result.Num == 4 );
    REQUIRE( Result.Ptr == FileName.Ptr + 7 );
  }
}
