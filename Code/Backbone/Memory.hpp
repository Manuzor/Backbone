
#pragma once

#include "Common.hpp"

//~~[[


/// \defgroup Memory manipulation functions
///
/// Provides functions to work on chunks of memory.
///
/// Unlike C standard functions such as memcpy and memset, these functions
/// respect the type of the input objects. Refer to the table below to find
/// which C standard functionality is covered by which of the functions
/// defined here.
///
/// C Standard Function | Untyped/Bytes                   | Typed
/// ------------------- | ------------------------------- | -----
/// memcopy, memmove    | MemCopyBytes                    | MemCopy, MemCopyConstruct, MemMove, MemMoveConstruct
/// memset              | MemSetBytes                     | MemSet, MemConstruct
/// memcmp              | MemCompareBytes, MemEqualBytes  | -
///
///
/// All functions are optimized for POD types.
///
/// @{

RESERVE_PREFIX(Mem);

/// Copy NumBytes from Source to Destination.
///
/// Destination and Source may overlap.
void
MemCopyBytes(memory_size Size, void* Destination, void const* Source);

/// Fill NumBytes in Destination with the value
void
MemSetBytes(memory_size Size, void* Destination, int Value);

bool
MemEqualBytes(memory_size Size, void const* A, void const* B);

int
MemCompareBytes(memory_size Size, void const* A, void const* B);

bool
MemAreOverlapping(memory_size SizeA, void const* A, memory_size SizeB, void const* B);


/// Calls the constructor of all elements in Destination with Args.
///
/// Args may be empty in which case all elements get default-initialized.
template<typename T, typename... ArgTypes>
void
MemConstruct(size_t Num, T* Destination, ArgTypes&&... Args);

/// Destructs all elements in Destination.
template<typename T>
void
MemDestruct(size_t Num, T* Destination);

/// Copy all elements from Source to Destination.
///
/// Destination and Source may overlap.
template<typename T>
void
MemCopy(size_t Num, T* Destination, T const* Source);

/// Copy all elements from Source to Destination using T's constructor.
///
/// Destination and Source may NOT overlap. Destination is assumed to be
/// uninitialized.
template<typename T>
void
MemCopyConstruct(size_t Num, T* Destination, T const* Source);

/// Move all elements from Source to Destination using T's constructor.
///
/// Destination and Source may overlap.
template<typename T>
void
MemMove(size_t Num, T* Destination, T* Source);

/// Move all elements from Source to Destination using T's constructor and destruct Source afterwards.
///
/// Destination and Source may NOT overlap. Destination is assumed to be
/// uninitialized.
template<typename T>
void
MemMoveConstruct(size_t Num, T* Destination, T* Source);

/// Assign the default value of T to all elements in Destination.
template<typename T>
void
MemSet(size_t Num, T* Destination);

/// Assign Item to all elements in Destination.
template<typename T>
void
MemSet(size_t Num, T* Destination, T const& Item);

template<typename TA, typename TB>
bool
MemAreOverlapping(size_t NumA, TA const* A, size_t NumB, TB const* B)
{
  return ::MemAreOverlapping(NumA * SizeOf<TA>(), Reinterpret<void const*>(A),
                             NumB * SizeOf<TB>(), Reinterpret<void const*>(B));
}


//
// Implementation Details
//

// MemConstruct

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_construct
{
  template<typename... ArgTypes>
  inline static void
  Do(size_t Num, T* Destination, ArgTypes&&... Args)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(Forward<ArgTypes>(Args)...);
    }
  }
};

template<typename T>
struct impl_mem_construct<T, true>
{
  inline static void
  Do(size_t Num, T* Destination)
  {
    MemSetBytes(Num * SizeOf<T>(), Destination, 0);
  }

  inline static void
  Do(size_t Num, T* Destination, T const& Item)
  {
    // Blit Item over each element of Destination.
    for(size_t Index = 0; Index < Num; ++Index)
    {
      MemCopyBytes(SizeOf<T>(), &Destination[Index], &Item);
    }
  }
};

template<typename T, typename... ArgTypes>
inline auto
MemConstruct(size_t Num, T* Destination, ArgTypes&&... Args)
  -> void
{
  impl_mem_construct<T, IsPOD<T>()>::Do(Num, Destination, Forward<ArgTypes>(Args)...);
}


// MemDestruct

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_destruct
{
  inline static void
  Do(size_t Num, T* Destination)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index].~T();
    }
  }
};

template<typename T>
struct impl_mem_destruct<T, true>
{
  inline static void
  Do(size_t Num, T* Destination)
  {
    // Nothing to do for POD types.
  }
};

template<typename T>
inline auto
MemDestruct(size_t Num, T* Destination)
  -> void
{
  impl_mem_destruct<T, IsPOD<T>()>::Do(Num, Destination);
}


// MemCopy

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_copy
{
  inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    if(Destination == Source)
      return;

    if(MemAreOverlapping(Num, Destination, Num, Source) && Destination < Source)
    {
      // Copy backwards.
      for(size_t Index = Num; Index > 0;)
      {
        --Index;
        Destination[Index] = Source[Index];
      }
    }
    else
    {
      // Copy forwards.
      for(size_t Index = 0; Index < Num; ++Index)
      {
        Destination[Index] = Source[Index];
      }
    }
  }
};

template<typename T>
struct impl_mem_copy<T, true>
{
  inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    MemCopyBytes(SizeOf<T>() * Num, Destination, Source);
  }
};

template<typename T>
inline auto
MemCopy(size_t Num, T* Destination, T const* Source)
  -> void
{
  impl_mem_copy<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MemCopyConstruct

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_copy_construct
{
  inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed.
    Assert(!MemAreOverlapping(Num, Destination, Num, Source));

    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(Source[Index]);
    }
  }
};

template<typename T>
struct impl_mem_copy_construct<T, true>
{
  inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed. Even though in
    // the POD case here it doesn't make a difference, it might help to catch
    // bugs since this can't be intentional.
    Assert(!MemAreOverlapping(Num, Destination, Num, Source));

    MemCopy(Num, Destination, Source);
  }
};

template<typename T>
inline auto
MemCopyConstruct(size_t Num, T* Destination, T const* Source)
  -> void
{
  impl_mem_copy_construct<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MemMove

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_move
{
  inline static void
  Do(size_t Num, T* Destination, T* Source)
  {
    if(Destination == Source)
      return;

    if(MemAreOverlapping(Num, Destination, Num, Source))
    {
      if(Destination < Source)
      {
        // Move forward
        for(size_t Index = 0; Index < Num; ++Index)
        {
          Destination[Index] = Move(Source[Index]);
        }

        // Destroy the remaining elements in the back.
        size_t const NumToDestruct = Source - Destination;
        MemDestruct(NumToDestruct, MemAddOffset(Source, Num - NumToDestruct));
      }
      else
      {
        // Move backward
        for(size_t Index = Num; Index > 0;)
        {
          --Index;
          Destination[Index] = Move(Source[Index]);
        }

        // Destroy the remaining elements in the front.
        size_t const NumToDestruct = Destination - Source;
        MemDestruct(NumToDestruct, Source);
      }
    }
    else
    {
      // Straight forward: Move one by one, then destruct all in Source.
      for(size_t Index = 0; Index < Num; ++Index)
      {
        Destination[Index] = Move(Source[Index]);
      }
      MemDestruct(Num, Source);
    }
  }
};

template<typename T>
struct impl_mem_move<T, true> : public impl_mem_copy<T, true> {};

template<typename T>
inline auto
MemMove(size_t Num, T* Destination, T* Source)
  -> void
{
  impl_mem_move<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MemMoveConstruct

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_move_construct
{
  inline static void
  Do(size_t Num, T* Destination, T* Source)
  {
    // When using the constructor, overlapping is not allowed.
    Assert(!MemAreOverlapping(Num, Destination, Num, Source));

    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(Move(Source[Index]));
    }
    MemDestruct(Num, Source);
  }
};

template<typename T>
struct impl_mem_move_construct<T, true>
{
  inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed. Even though in
    // the POD case here it doesn't make a difference, it might help to catch
    // bugs since this can't be intentional.
    Assert(!MemAreOverlapping(Num, Destination, Num, Source));

    MemCopy(Num, Destination, Source);
  }
};

template<typename T>
inline auto
MemMoveConstruct(size_t Num, T* Destination, T* Source)
  -> void
{
  impl_mem_move_construct<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MemSet

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_set
{
  inline static void
  Do(size_t Num, T* Destination)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index] = {};
    }
  }

  inline static void
  Do(size_t Num, T* Destination, T const& Item)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index] = Item;
    }
  }
};

template<typename T>
struct impl_mem_set<T, true> : public impl_mem_construct<T, true> {};

template<typename T>
inline auto
MemSet(size_t Num, T* Destination)
  -> void
{
  impl_mem_set<T, IsPOD<T>()>::Do(Num, Destination);
}

template<typename T>
inline auto
MemSet(size_t Num, T* Destination, T const& Item)
  -> void
{
  impl_mem_set<T, IsPOD<T>()>::Do(Num, Destination, Item);
}

/// @}
//]]~~
