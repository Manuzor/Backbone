#include <Backbone/Angle.hpp>

#include "catch.hpp"

TEST_CASE("Angle Conversion", "[Angle]")
{
  REQUIRE(ToRadians(degrees{180}).Value == Pi32);
  REQUIRE(ToRadians(degrees{90}).Value  == Pi32 * 0.5f);
  REQUIRE(ToRadians(degrees{270}).Value == Pi32 * 1.5f);
  REQUIRE(ToRadians(degrees{360}).Value == Pi32 * 2.0f);

  REQUIRE(ToDegrees(radians{       Pi32}).Value == 180);
  REQUIRE(ToDegrees(radians{0.5f * Pi32}).Value == 90);
  REQUIRE(ToDegrees(radians{1.5f * Pi32}).Value == 270);
  REQUIRE(ToDegrees(radians{2.0f * Pi32}).Value == 360);
}

TEST_CASE("Angle Normalization", "[Angle]")
{
  REQUIRE( IsNormalized(degrees{  0}));
  REQUIRE( IsNormalized(degrees{ 90}));
  REQUIRE(!IsNormalized(degrees{-90}));
  REQUIRE(!IsNormalized(degrees{360}));
  REQUIRE(!IsNormalized(degrees{720}));

  REQUIRE(Normalized(degrees{90}).Value == 90);
  REQUIRE(Normalized(degrees{360}).Value == 0);
  REQUIRE(Normalized(degrees{450}).Value == 90);
  REQUIRE(Normalized(degrees{-90}).Value == 270);
  REQUIRE(Normalized(degrees{-360}).Value == 0);

  REQUIRE(Normalized(degrees{720}).Value == 0);
  REQUIRE(Normalized(degrees{-720}).Value == 0);
  REQUIRE(Normalized(degrees{-450}).Value == 270);
}

TEST_CASE("Angle operators", "[Angle]")
{
  // Note: Only tested with degrees.

  SECTION("Equality Operators")
  {
    REQUIRE(degrees{90} == degrees{90});
    REQUIRE(degrees{90} != degrees{45});
    REQUIRE(degrees{45} <  degrees{90});
    REQUIRE(degrees{45} <= degrees{45});
    REQUIRE(degrees{90} >  degrees{45});
    REQUIRE(degrees{45} >= degrees{45});
  }

  SECTION("Add")
  {
    REQUIRE(degrees{45} + degrees{45} == degrees{90});
  }

  SECTION("Subtract")
  {
    REQUIRE(degrees{90} - degrees{45} == degrees{45});
  }

  SECTION("Scale")
  {
    REQUIRE(degrees{45} *  2.0f == degrees{90});
    REQUIRE(2.0f *  degrees{45} == degrees{90});
    REQUIRE(degrees{90} / 2 == degrees{45});
  }

  SECTION("Assignment Operators")
  {
    degrees A{90};

    SECTION("Add")
    {
      A += degrees{90};
      REQUIRE(A == degrees{180});
    }

    SECTION("Subtract")
    {
      A -= degrees{45};
      REQUIRE(A == degrees{45});
    }

    SECTION("Scale")
    {
      A *= 2.0f;
      REQUIRE(A == degrees{180});

      A /= 2.0f;
      REQUIRE(A == degrees{90});
    }
  }
}

TEST_CASE("Angle Equality", "[Angle]")
{
  REQUIRE( AreNearlyEqual(degrees{90}, degrees{ 91}, degrees{1}));
  REQUIRE(!AreNearlyEqual(degrees{90}, degrees{ 92}, degrees{1}));

  REQUIRE(!AreNearlyEqual(degrees{0},            degrees{360})); // Must manually normalize before calling this.
  REQUIRE( AreNearlyEqual(degrees{0}, Normalized(degrees{360})));
}

TEST_CASE("AngleBetween", "[Angle]")
{
  // Note: Tests taken from the ezEngine.

  REQUIRE(AreNearlyEqual(AngleBetween(degrees{0},   degrees{  0}), degrees{0.0f}, degrees{0.0001f}));
  REQUIRE(AreNearlyEqual(AngleBetween(degrees{0},   degrees{360}), degrees{0.0f}, degrees{0.0001f}));
  REQUIRE(AreNearlyEqual(AngleBetween(degrees{360}, degrees{360}), degrees{0.0f}, degrees{0.0001f}));
  REQUIRE(AreNearlyEqual(AngleBetween(degrees{360}, degrees{  0}), degrees{0.0f}, degrees{0.0001f}));

  REQUIRE(AreNearlyEqual(AngleBetween(degrees{ 5}, degrees{ 186}), degrees{179.0f}, degrees{0.0001f}));
  REQUIRE(AreNearlyEqual(AngleBetween(degrees{-5}, degrees{-186}), degrees{179.0f}, degrees{0.0001f}));

  REQUIRE(AreNearlyEqual(AngleBetween(degrees{360.0f +  5},  degrees{ 360.0f + 186}), degrees{179.0f}, degrees{0.0001f}));
  REQUIRE(AreNearlyEqual(AngleBetween(degrees{360.0f + -5},  degrees{ 360.0f - 186}), degrees{179.0f}, degrees{0.0001f}));

  for (int32 Index = 0; Index <= 179; ++Index)
    REQUIRE(AreNearlyEqual(AngleBetween(degrees{(float)Index}, degrees{(float)(Index + Index)}), degrees{(float)Index}, degrees{0.0001f}));

  for (int32 Index = -179; Index <= 0; ++Index)
    REQUIRE(AreNearlyEqual(AngleBetween(degrees{(float)Index}, degrees{(float)(Index + Index)}), degrees{(float)-Index}, degrees{0.0001f}));
}
