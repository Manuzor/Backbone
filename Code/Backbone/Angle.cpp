#include "Angle.hpp"

#include <cmath>

//~~[[


auto
::operator +=(angle& A, angle B)
  -> void
{
  A.InternalData += B.InternalData;
}

auto
::operator -=(angle& A, angle B)
  -> void
{
  A.InternalData -= B.InternalData;
}

auto
::operator *=(angle& A, float Scale)
  -> void
{
  A.InternalData *= Scale;
}

auto
::operator /=(angle& A, float Scale)
  -> void
{
  A.InternalData /= Scale;
}

auto
::IsNormalized(angle Angle)
  -> bool
{
  return Angle.InternalData >= 0.0f && Angle.InternalData < 2.0f * Pi();
}

auto
::Normalized(angle Angle)
  -> angle
{
  return { Wrap(Angle.InternalData, 0.0f, 2.0f * Pi()) };
}

auto
::AngleBetween(angle A, angle B)
  -> angle
{
  // Taken from ezEngine who got it from here:
  // http://gamedev.stackexchange.com/questions/4467/comparing-angles-and-working-out-the-difference
  return { Pi() - Abs(Abs(A.InternalData - B.InternalData) - Pi()) };
}

auto
::AreNearlyEqual(angle A, angle B, angle Epsilon)
  -> bool
{
  return AreNearlyEqual(A.InternalData, B.InternalData, Epsilon.InternalData);
}

auto
::Sin(angle Angle)
  -> float
{
  return std::sin(ToRadians(Angle));
}

auto
::Cos(angle Angle)
  -> float
{
  return std::cos(ToRadians(Angle));
}

auto
::Tan(angle Angle)
  -> float
{
  return std::tan(ToRadians(Angle));
}

auto
::ASin(float A)
  -> angle
{
  return Radians(std::asin(A));
}

auto
::ACos(float A)
  -> angle
{
  return Radians(std::acos(A));
}

auto
::ATan(float A)
  -> angle
{
  return Radians(std::atan(A));
}

auto
::ATan2(float A, float B)
  -> angle
{
  return Radians(std::atan2(A, B));
}

//]]~~
