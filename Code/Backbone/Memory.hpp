
#pragma once

#include "Common.hpp"

//~~[[

//
// Untyped / Raw Memory Functions
//

void
MemCopyBytes(void* Destination, void const* Source, size_t NumBytes);

void
MemSetBytes(void* Destination, int Value, size_t NumBytes);

bool
MemEqualBytes(void const* A, void const* B, size_t NumBytes);

int
MemCompareBytes(void const* A, void const* B, size_t NumBytes);

//
// Typed Copy
//

template<typename T, bool IsPlainOldData = false>
struct impl_mem_copy
{
  static void Do(T* Destination, T const* Source, size_t Num)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index] = Source[Index];
    }
  }
};

template<typename T>
struct impl_mem_copy<T, false>
{
  static constexpr void Do(T* Destination, T const* Source, size_t Num)
  {
    MemCopyBytes(Reinterpret<void*>(Destination), Reinterpret<void const*>(Source), Num * sizeof(T));
  }
};

template<typename T>
constexpr void
MemCopy(T* Destination, T const* Source, size_t Num)
{
  impl_mem_copy<T, IsPOD<T>()>::Do(Destination, Source, Num);
}

//
// Typed Move
//

template<typename T, bool IsPlainOldData = false>
struct impl_mem_move
{
  static void Do(T* Destination, T const* Source, size_t Num)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index] = Source[Index];
    }
  }
};

template<typename T>
struct impl_mem_move<T, false>
{
  static constexpr void Do(T* Destination, T const* Source, size_t Num)
  {
    MemCopyBytes(Reinterpret<void*>(Destination), Reinterpret<void const*>(Source), Num * sizeof(T));
  }
};

template<typename T>
constexpr void
MemMove(T* Destination, T const* Source, size_t Num)
{
  impl_mem_move<T, IsPOD<T>()>::Do(Destination, Source, Num);
}


//
// Default Construction
//

template<typename T, bool IsPOD = false>
struct impl_mem_default_construct
{
  static void Do(T* Elements, size_t Num)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (Elements + Index) T;
    }
  }
};

template<typename T>
struct impl_mem_default_construct<T, true>
{
  static void Do(T* Elements, size_t Num)
  {
    MemSetBytes(Elements, 0, Num * sizeof(T));
  }
};

template<typename T>
constexpr void
MemDefaultConstruct(T* Elements, size_t Num)
{
  impl_mem_default_construct<T, IsPOD<T>()>::Do(Elements, Num);
}

//
// Typed Destruction
//

template<typename T, bool IsPOD = false>
struct impl_mem_destruct
{
  static void Do(T* Elements, size_t Num)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Elements[Index].~T();
    }
  }
};

template<typename T>
struct impl_mem_destruct<T, true>
{
  static void Do(T* Elements, size_t Num)
  {
    // Nothing to do for PODs
  }
};

template<typename T>
constexpr void
MemDestruct(T* Elements, size_t Num)
{
  impl_mem_destruct<T, IsPOD<T>()>::Do(Elements, Num);
}

//]]~~
