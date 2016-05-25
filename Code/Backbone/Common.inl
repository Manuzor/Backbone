
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
