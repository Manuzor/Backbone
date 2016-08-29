#include "Memory.hpp"

#include <cstring>


//~~[[

auto
::MemCopyBytes(memory_size Size, void* Destination, void const* Source)
  -> void
{
  // Using memmove so that Destination and Source may overlap.
  std::memmove(Destination, Source, ToBytes(Size));
}

auto
::MemSetBytes(memory_size Size, void* Destination, int Value)
  -> void
{
  std::memset(Destination, Value, ToBytes(Size));
}

auto
::MemEqualBytes(memory_size Size, void const* A, void const* B)
  -> bool
{
  return MemCompareBytes(Size, A, B) == 0;
}

auto
::MemCompareBytes(memory_size Size, void const* A, void const* B)
  -> int
{
  return std::memcmp(A, B, ToBytes(Size));
}

auto
::MemAreOverlapping(memory_size SizeA, void const* A, memory_size SizeB, void const* B)
  -> bool
{
  auto LeftA = Reinterpret<size_t const>(A);
  auto RightA = LeftA + ToBytes(SizeA);

  auto LeftB = Reinterpret<size_t const>(B);
  auto RightB = LeftB + ToBytes(SizeB);

  return LeftB  >= LeftA && LeftB  <= RightA || // Check if LeftB  is in [A, A+NumBytesA]
         RightB >= LeftA && RightB <= RightA || // Check if RightB is in [A, A+NumBytesA]
         LeftA  >= LeftB && LeftA  <= RightB || // Check if LeftA  is in [B, B+NumBytesB]
         RightA >= LeftB && RightA <= RightB;   // Check if RightA is in [B, B+NumBytesB]
}

//]]~~
