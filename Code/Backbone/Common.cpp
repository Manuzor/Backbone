
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
::Sqrt(double Value)
  -> double
{
  return std::sqrt(Value);
}

auto
::Sqrt(float Value)
  -> float
{
  return std::sqrt(Value);
}

auto
::InvSqrt(float Value)
  -> float
{
  union FloatInt
  {
    float Float;
    int Int;
  };
  FloatInt MagicNumber;
  float HalfValue;
  float Result;
  const float ThreeHalfs = 1.5f;

  HalfValue = Value * 0.5f;
  Result = Value;
  MagicNumber.Float = Result;                               // evil floating point bit level hacking
  MagicNumber.Int  = 0x5f3759df - ( MagicNumber.Int >> 1 ); // what the fuck?
  Result = MagicNumber.Float;
  Result = Result * ( ThreeHalfs - ( HalfValue * Result * Result ) ); // 1st iteration

  return Result;
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
