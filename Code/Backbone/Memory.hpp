
#pragma once

#include "Common.hpp"

//~~[[

//
// Untyped / Raw Memory Functions
//

void
MemCopyBytes(size_t NumBytes, void* Destination, void const* Source);

void
MemSetBytes(size_t NumBytes, void* Destination, int Value);

bool
MemEqualBytes(size_t NumBytes, void const* A, void const* B);

int
MemCompareBytes(size_t NumBytes, void const* A, void const* B);

//
// Typed Copy
//

template<typename T, bool IsPlainOldData = false>
struct impl_mem_copy
{
  static void Do(size_t Num, T* Destination, T const* Source)
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
    MemCopyBytes(Num * sizeof(T), Reinterpret<void*>(Destination), Reinterpret<void const*>(Source));
  }
};

template<typename T>
constexpr void
MemCopy(size_t Num, T* Destination, T const* Source)
{
  impl_mem_copy<T, IsPOD<T>()>::Do(Num, Destination, Source);
}

//
// Typed Move
//

template<typename T, bool IsPlainOldData = false>
struct impl_mem_move
{
  static void Do(size_t Num, T* Destination, T const* Source)
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
  static constexpr void Do(size_t Num, T* Destination, T const* Source)
  {
    MemCopyBytes(Num * sizeof(T), Reinterpret<void*>(Destination), Reinterpret<void const*>(Source));
  }
};

template<typename T>
constexpr void
MemMove(size_t Num, T* Destination, T const* Source)
{
  impl_mem_move<T, IsPOD<T>()>::Do(Num, Destination, Source);
}

//
// Typed Set
//

template<typename T, bool IsPlainOldData = false>
struct impl_mem_set
{
  static void Do(size_t Num, T* Destination, T Value)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index] = Value;
    }
  }
};

template<typename T>
struct impl_mem_set<T, false>
{
  static constexpr void Do(size_t Num, T* Destination, T Value)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      MemCopyBytes(Destination + Index, &Value, sizeof(T));
    }
  }
};

template<typename T>
void
MemSet(T* Destination, T Value, size_t Num)
{
  impl_mem_set<T, IsPOD<T>()>::Do(Num, Destination, Move(Value));
}


//
// Typed Default Construction
//

template<typename T, bool IsPOD = false>
struct impl_mem_default_construct
{
  static void Do(size_t Num, T* Elements)
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
  static void Do(size_t Num, T* Elements)
  {
    MemSetBytes(Num * sizeof(T), Elements, 0);
  }
};

template<typename T>
constexpr void
MemDefaultConstruct(size_t Num, T* Elements)
{
  impl_mem_default_construct<T, IsPOD<T>()>::Do(Num, Elements);
}

//
// Typed Construction With Arguments
//

template<typename T, bool IsPOD = false>
struct impl_mem_construct
{
  template<typename... ArgTypes>
  static void Do(T* Ptr, size_t Num, ArgTypes&&... Args)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (Ptr + Index) T(Forward<ArgTypes>(Args)...);
    }
  }
};

template<typename T>
struct impl_mem_construct<T, true>
{
  static void Do(size_t Num, T* Ptr, T Value)
  {
    MemSet(Num, Ptr, Move(Value));
  }
};

template<typename T, typename... ArgTypes>
constexpr void
MemConstruct(size_t Num, T* Elements, ArgTypes&&... Args)
{
  impl_mem_construct<T, IsPOD<T>()>::Do(Num, Elements, Forward<ArgTypes>(Args)...);
}

//
// Typed Destruction
//

template<typename T, bool IsPOD = false>
struct impl_mem_destruct
{
  static void Do(size_t Num, T* Elements)
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
  static void Do(size_t Num, T* Elements)
  {
    // Nothing to do for PODs
  }
};

template<typename T>
constexpr void
MemDestruct(size_t Num, T* Elements)
{
  impl_mem_destruct<T, IsPOD<T>()>::Do(Num, Elements);
}

//]]~~
