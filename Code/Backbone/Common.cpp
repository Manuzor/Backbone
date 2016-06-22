
#include "Common.hpp"

#include <cmath>

//~~[[

auto
::Pow(float Base, float Exponent)
  -> float
{
  return std::pow(Base, Exponent);
}

auto
::Pow(double Base, double Exponent)
  -> double
{
  return std::pow(Base, Exponent);
}

auto
::AreNearlyEqual(double A, double B, double Epsilon)
  -> bool
{
  return Abs(A - B) <= Epsilon;
}

auto
::AreNearlyEqual(float A, float B, float Epsilon)
  -> bool
{
  return Abs(A - B) <= Epsilon;
}

//]]~~
