#pragma once

#include <Backbone/Common.hpp>

//~~[[

struct angle { float InternalData; };

constexpr bool operator ==(angle A, angle B) { return A.InternalData == B.InternalData; }
constexpr bool operator !=(angle A, angle B) { return A.InternalData != B.InternalData; }
constexpr bool operator < (angle A, angle B) { return A.InternalData <  B.InternalData; }
constexpr bool operator <=(angle A, angle B) { return A.InternalData <= B.InternalData; }
constexpr bool operator > (angle A, angle B) { return A.InternalData >  B.InternalData; }
constexpr bool operator >=(angle A, angle B) { return A.InternalData >= B.InternalData; }

constexpr angle operator +(angle A, angle B)     { return { A.InternalData + B.InternalData }; }
constexpr angle operator -(angle A, angle B)     { return { A.InternalData - B.InternalData }; }
constexpr angle operator *(angle A, float Scale) { return { A.InternalData * Scale };   }
constexpr angle operator *(float Scale, angle A) { return { Scale * A.InternalData };   }
constexpr angle operator /(angle A, float Scale) { return { A.InternalData / Scale };   }

void operator +=(angle& A, angle B);
void operator -=(angle& A, angle B);
void operator *=(angle& A, float Scale);
void operator /=(angle& A, float Scale);


//
// Angle Conversion
//

constexpr angle
Radians(float RadiansValue)
{
  return { RadiansValue };
}

constexpr float
ToRadians(angle Angle)
{
  return Angle.InternalData;
}

constexpr angle
Degrees(float DegreesValue)
{
  return { DegreesValue * (Pi() / 180.0f) };
}

constexpr float
ToDegrees(angle Angle)
{
  return { Angle.InternalData * (180.0f / Pi()) };
}

//
// Angle Normalization
//

bool
IsNormalized(angle Angle);

angle
Normalized(angle Angle);

angle
AngleBetween(angle A, angle B);

//
// Equality
//

/// Checks whether A and B are nearly equal with the given Epsilon.
bool
AreNearlyEqual(angle A, angle B, angle Epsilon = angle{ 0.0001f });

//]]~~
