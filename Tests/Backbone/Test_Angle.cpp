#include <Backbone/Angle.hpp>

#include "catch.hpp"

TEST_CASE("Angle Conversion", "[Angle]")
{
  REQUIRE(ToRadians(Degrees(180)) == Pi<float>());
  REQUIRE(ToRadians(Degrees(90))  == Pi<float>() * 0.5f);
  REQUIRE(ToRadians(Degrees(270)) == Pi<float>() * 1.5f);
  REQUIRE(ToRadians(Degrees(360)) == Pi<float>() * 2.0f);

  REQUIRE(ToDegrees(Radians(       Pi<float>())) == 180);
  REQUIRE(ToDegrees(Radians(0.5f * Pi<float>())) == 90);
  REQUIRE(ToDegrees(Radians(1.5f * Pi<float>())) == 270);
  REQUIRE(ToDegrees(Radians(2.0f * Pi<float>())) == 360);
}

TEST_CASE("Angle Normalization", "[Angle]")
{
  REQUIRE( IsNormalized(Degrees(  0)));
  REQUIRE( IsNormalized(Degrees( 90)));
  REQUIRE(!IsNormalized(Degrees(-90)));
  REQUIRE(!IsNormalized(Degrees(360)));
  REQUIRE(!IsNormalized(Degrees(720)));

  REQUIRE(AreNearlyEqual(ToDegrees(Normalized(Degrees(90))), 90, 1e-4f));
  REQUIRE(AreNearlyEqual(ToDegrees(Normalized(Degrees(360))), 0, 1e-4f));
  REQUIRE(AreNearlyEqual(ToDegrees(Normalized(Degrees(450))), 90, 1e-4f));
  REQUIRE(AreNearlyEqual(ToDegrees(Normalized(Degrees(-90))), 270, 1e-4f));
  REQUIRE(AreNearlyEqual(ToDegrees(Normalized(Degrees(-360))), 0, 1e-4f));

  REQUIRE(AreNearlyEqual(ToDegrees(Normalized(Degrees(720))), 0, 1e-4f));
  REQUIRE(AreNearlyEqual(ToDegrees(Normalized(Degrees(-720))), 0, 1e-4f));
  REQUIRE(AreNearlyEqual(ToDegrees(Normalized(Degrees(-450))), 270, 1e-4f));
}

TEST_CASE("Angle operators", "[Angle]")
{
  // Note: Only tested with Degrees.

  SECTION("Equality Operators")
  {
    REQUIRE(Degrees(90) == Degrees(90));
    REQUIRE(Degrees(90) != Degrees(45));
    REQUIRE(Degrees(45) <  Degrees(90));
    REQUIRE(Degrees(45) <= Degrees(45));
    REQUIRE(Degrees(90) >  Degrees(45));
    REQUIRE(Degrees(45) >= Degrees(45));
  }

  SECTION("Add")
  {
    REQUIRE(Degrees(45) + Degrees(45) == Degrees(90));
  }

  SECTION("Subtract")
  {
    REQUIRE(Degrees(90) - Degrees(45) == Degrees(45));
  }

  SECTION("Scale")
  {
    REQUIRE(Degrees(45) *  2.0f == Degrees(90));
    REQUIRE(2.0f *  Degrees(45) == Degrees(90));
    REQUIRE(Degrees(90) / 2 == Degrees(45));
  }

  SECTION("Assignment Operators")
  {
    auto A = Degrees(90);

    SECTION("Add")
    {
      A += Degrees(90);
      REQUIRE(A == Degrees(180));
    }

    SECTION("Subtract")
    {
      A -= Degrees(45);
      REQUIRE(A == Degrees(45));
    }

    SECTION("Scale")
    {
      A *= 2.0f;
      REQUIRE(A == Degrees(180));

      A /= 2.0f;
      REQUIRE(A == Degrees(90));
    }
  }
}

TEST_CASE("Angle Equality", "[Angle]")
{
  REQUIRE( AreNearlyEqual(Degrees(90), Degrees( 91), Degrees(1)));
  REQUIRE(!AreNearlyEqual(Degrees(90), Degrees( 92), Degrees(1)));

  REQUIRE(!AreNearlyEqual(Degrees(0),            Degrees(360))); // Must manually normalize before calling this.
  REQUIRE( AreNearlyEqual(Degrees(0), Normalized(Degrees(360))));
}

TEST_CASE("AngleBetween", "[Angle]")
{
  // Note: Tests taken from the ezEngine.

  REQUIRE(AreNearlyEqual(AngleBetween(Degrees(0),   Degrees(  0)), Degrees(0.0f), Degrees(0.0001f)));
  REQUIRE(AreNearlyEqual(AngleBetween(Degrees(0),   Degrees(360)), Degrees(0.0f), Degrees(0.0001f)));
  REQUIRE(AreNearlyEqual(AngleBetween(Degrees(360), Degrees(360)), Degrees(0.0f), Degrees(0.0001f)));
  REQUIRE(AreNearlyEqual(AngleBetween(Degrees(360), Degrees(  0)), Degrees(0.0f), Degrees(0.0001f)));

  REQUIRE(AreNearlyEqual(AngleBetween(Degrees( 5), Degrees( 186)), Degrees(179.0f), Degrees(0.0001f)));
  REQUIRE(AreNearlyEqual(AngleBetween(Degrees(-5), Degrees(-186)), Degrees(179.0f), Degrees(0.0001f)));

  REQUIRE(AreNearlyEqual(AngleBetween(Degrees(360.0f +  5),  Degrees( 360.0f + 186)), Degrees(179.0f), Degrees(0.0001f)));
  REQUIRE(AreNearlyEqual(AngleBetween(Degrees(360.0f + -5),  Degrees( 360.0f - 186)), Degrees(179.0f), Degrees(0.0001f)));

  for (int32 Index = 0; Index <= 179; ++Index)
    REQUIRE(AreNearlyEqual(AngleBetween(Degrees((float)Index), Degrees((float)(Index + Index))), Degrees((float)Index), Degrees(0.0001f)));

  for (int32 Index = -179; Index <= 0; ++Index)
    REQUIRE(AreNearlyEqual(AngleBetween(Degrees((float)Index), Degrees((float)(Index + Index))), Degrees((float)-Index), Degrees(0.0001f)));
}
