
//~~[[

auto
::IsNormalized(radians Radians)
  -> bool
{
  return Radians.Value >= 0.0f && Radians.Value < 2.0f * Pi32;
}

auto
::Normalized(radians Radians)
  -> radians
{
  return { Wrap(Radians.Value, 0.0f, 2.0f * Pi32) };
}

auto
::AngleBetween(radians A, radians B)
  -> radians
{
  // Taken from ezEngine who got it from here:
  // http://gamedev.stackexchange.com/questions/4467/comparing-angles-and-working-out-the-difference
  return { Pi32 - Abs(Abs(A.Value - B.Value) - Pi32) };
}

auto
::IsNormalized(degrees Degrees)
  -> bool
{
  return Degrees.Value >= 0.0f && Degrees.Value < 360.0f;
}

auto
::Normalized(degrees Degrees)
  -> degrees
{
  return { Wrap(Degrees.Value, 0.0f, 360.0f) };
}

auto
::AngleBetween(degrees A, degrees B)
  -> degrees
{
  // Taken from ezEngine who got it from here:
  // http://gamedev.stackexchange.com/questions/4467/comparing-angles-and-working-out-the-difference
  return { 180.0f - Abs(Abs(A.Value - B.Value) - 180.0f) };
}

auto
::AreNearlyEqual(radians A, radians B, radians Epsilon)
  -> bool
{
  return AreNearlyEqual(A.Value, B.Value, Epsilon.Value);
}

auto
::AreNearlyEqual(degrees A, degrees B, degrees Epsilon)
  -> bool
{
  return AreNearlyEqual(A.Value, B.Value, Epsilon.Value);
}

//]]~~
