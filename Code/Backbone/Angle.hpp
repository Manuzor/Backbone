#pragma once

#include <Backbone/Common.hpp>

//~~[[

struct radians { float Value; };

constexpr inline bool    operator ==(radians A, radians B)   { return A.Value == B.Value;    }
constexpr inline bool    operator !=(radians A, radians B)   { return A.Value != B.Value;    }
constexpr inline bool    operator < (radians A, radians B)   { return A.Value <  B.Value;    }
constexpr inline bool    operator <=(radians A, radians B)   { return A.Value <= B.Value;    }
constexpr inline bool    operator > (radians A, radians B)   { return A.Value >  B.Value;    }
constexpr inline bool    operator >=(radians A, radians B)   { return A.Value >= B.Value;    }

constexpr inline radians operator + (radians A, radians B)   { return { A.Value + B.Value }; }
constexpr inline radians operator - (radians A, radians B)   { return { A.Value - B.Value }; }
constexpr inline radians operator * (radians A, float Scale) { return { A.Value * Scale };   }
constexpr inline radians operator * (float Scale, radians A) { return { Scale * A.Value };   }
constexpr inline radians operator / (radians A, float Scale) { return { A.Value / Scale };   }

inline void operator +=(radians& A, radians B)   { A.Value += B.Value; }
inline void operator -=(radians& A, radians B)   { A.Value -= B.Value; }
inline void operator *=(radians& A, float Scale) { A.Value *= Scale; }
inline void operator /=(radians& A, float Scale) { A.Value /= Scale; }


struct degrees { float Value; };

constexpr inline bool    operator ==(degrees A, degrees B)   { return A.Value == B.Value;    }
constexpr inline bool    operator !=(degrees A, degrees B)   { return A.Value != B.Value;    }
constexpr inline bool    operator < (degrees A, degrees B)   { return A.Value <  B.Value;    }
constexpr inline bool    operator <=(degrees A, degrees B)   { return A.Value <= B.Value;    }
constexpr inline bool    operator > (degrees A, degrees B)   { return A.Value >  B.Value;    }
constexpr inline bool    operator >=(degrees A, degrees B)   { return A.Value >= B.Value;    }

constexpr inline degrees operator + (degrees A, degrees B)   { return { A.Value + B.Value }; }
constexpr inline degrees operator - (degrees A, degrees B)   { return { A.Value - B.Value }; }
constexpr inline degrees operator * (degrees A, float Scale) { return { A.Value * Scale };   }
constexpr inline degrees operator * (float Scale, degrees A) { return { Scale * A.Value };   }
constexpr inline degrees operator / (degrees A, float Scale) { return { A.Value / Scale };   }

inline void operator +=(degrees& A, degrees B)   { A.Value += B.Value; }
inline void operator -=(degrees& A, degrees B)   { A.Value -= B.Value; }
inline void operator *=(degrees& A, float Scale) { A.Value *= Scale; }
inline void operator /=(degrees& A, float Scale) { A.Value /= Scale; }

//
// Angle Conversion
//

constexpr inline degrees
ToDegrees(radians Radians)
{
  return { Radians.Value * (180.0f / Pi()) };
}

constexpr inline radians
ToRadians(degrees Degrees)
{
  return { Degrees.Value * (Pi() / 180.0f) };
}

//
// Angle Normalization
//

BB_Inline bool
IsNormalized(radians Radians);

BB_Inline radians
Normalized(radians Radians);

BB_Inline radians
AngleBetween(radians A, radians B);

BB_Inline bool
IsNormalized(degrees Degrees);

BB_Inline degrees
Normalized(degrees Degrees);

BB_Inline degrees
AngleBetween(degrees A, degrees B);

//
// Equality
//

/// Checks whether A and B are nearly equal with the given Epsilon.
BB_Inline bool
AreNearlyEqual(radians A, radians B, radians Epsilon = radians{ 0.0001f });

BB_Inline bool
AreNearlyEqual(degrees A, degrees B, degrees Epsilon = degrees{ 0.01f   });

//]]~~

#include "Angle.inl"
