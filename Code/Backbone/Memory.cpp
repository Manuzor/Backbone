#include "Memory.hpp"

#include <cstring>


//~~[[

auto
::MemCopyBytes(size_t NumBytes, void* Destination, void const* Source)
  -> void
{
  // Using memmove so that Destination and Source may overlap.
  std::memmove(Destination, Source, NumBytes);
}

auto
::MemSetBytes(size_t NumBytes, void* Destination, int Value)
  -> void
{
  std::memset(Destination, Value, NumBytes);
}

auto
::MemEqualBytes(size_t NumBytes, void const* A, void const* B)
  -> bool
{
  return MemCompareBytes(NumBytes, A, B) == 0;
}

auto
::MemCompareBytes(size_t NumBytes, void const* A, void const* B)
  -> int
{
  return std::memcmp(A, B, NumBytes);
}

auto
::MemAreOverlapping(size_t NumBytesA, void const* A, size_t NumBytesB, void const* B)
  -> bool
{
  auto LeftA = Reinterpret<size_t const>(A);
  auto RightA = LeftA + NumBytesA;

  auto LeftB = Reinterpret<size_t const>(B);
  auto RightB = LeftB + NumBytesB;

  return LeftB  >= LeftA && LeftB  <= RightA || // Check if LeftB  is in [A, A+NumBytesA]
         RightB >= LeftA && RightB <= RightA || // Check if RightB is in [A, A+NumBytesA]
         LeftA  >= LeftB && LeftA  <= RightB || // Check if LeftA  is in [B, B+NumBytesB]
         RightA >= LeftB && RightA <= RightB;   // Check if RightA is in [B, B+NumBytesB]
}

//]]~~
