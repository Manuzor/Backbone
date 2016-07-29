#pragma once

#include "Common.hpp"
#include "Memory.hpp"
#include <new>

//~~[[

constexpr size_t INVALID_INDEX = (size_t)-1;

template<typename ElementType>
struct slice
{
  using element_type = ElementType;

  size_t Num;
  element_type* Ptr;

  /// Test whether this slice is valid or not.
  ///
  /// A slice is considered valid if it does not point to null and contains at
  /// least one element. If `Num` is 0 or `Ptr` is `nullptr`, the slice is
  /// considered invalid (`false`).
  operator bool() const { return Num && Ptr; }

  /// Index operator to access elements of the slice.
  template<typename IndexType>
  auto
  operator[](IndexType Index) const
    -> decltype(Ptr[Index])
  {
    BoundsCheck(Index >= 0 && Index < Num);
    return Ptr[Index];
  }
};

template<>
struct slice<void>
{
  using element_type = void;

  size_t Num;
  element_type* Ptr;

  /// Test whether this slice is valid or not.
  ///
  /// A slice is considered valid if it does not point to null and contains at
  /// least one element.
  operator bool() const { return Num && Ptr; }
};

template<>
struct slice<void const>
{
  using element_type = void const;

  size_t Num;
  element_type* Ptr;

  /// Test whether this slice is valid or not.
  ///
  /// A slice is considered valid if it does not point to null and contains at
  /// least one element.
  operator bool() const { return Num && Ptr; }
};

template<typename T>
typename slice<T>::element_type*
First(slice<T> const& SomeSlice)
{
  return SomeSlice.Ptr;
}

template<typename T>
typename slice<T>::element_type*
Last(slice<T> const& SomeSlice)
{
  return MemAddOffset(First(SomeSlice), Max(size_t(1), SomeSlice.Num) - 1);
}

template<typename T>
typename slice<T>::element_type*
OnePastLast(slice<T> const& SomeSlice)
{
  return MemAddOffset(First(SomeSlice), SomeSlice.Num);
}

/// C++11 range API
template<typename T>
typename slice<T>::element_type*
begin(slice<T> const& SomeSlice)
{
  return First(SomeSlice);
}

/// C++11 range API
template<typename T>
typename slice<T>::element_type*
end(slice<T> const& SomeSlice)
{
  return OnePastLast(SomeSlice);
}

template<typename TargetType, typename SourceType>
slice<TargetType>
SliceReinterpret(slice<SourceType> SomeSlice)
{
  return Slice(Reinterpret<TargetType*>(First(SomeSlice)),
               Reinterpret<TargetType*>(OnePastLast(SomeSlice)));
}

template<typename SourceType>
slice<SourceType const>
AsConst(slice<SourceType> SomeSlice)
{
  return Slice(AsPtrToConst(First(SomeSlice)),
               AsPtrToConst(OnePastLast(SomeSlice)));
}

/// Concatenate two slices together.
///
/// \return The returned slice will be a subset of the given Buffer, which is
/// \used to write the actual result in.
template<typename ElementType>
slice<ElementType>
SliceConcat(slice<ElementType const> Head, slice<ElementType const> Tail, slice<ElementType> Buffer)
{
  BoundsCheck(Buffer.Num >= Head.Num + Tail.Num);
  size_t DestIndex = 0;
  for(auto Element : Head)
  {
    Buffer[DestIndex++] = Element;
  }
  for(auto Element : Tail)
  {
    Buffer[DestIndex++] = Element;
  }

  // DestIndex must now be the combined count of Head and Tail.
  Assert(DestIndex == Head.Num + Tail.Num);
  auto Result = Slice(Buffer, 0, DestIndex);
  return Result;
}

/// Create a union of both input spans. The resulting slice will contain everything
template<typename ElementType>
constexpr slice<ElementType>
SliceUnion(slice<ElementType> SliceA, slice<ElementType> SliceB)
{
  // A union only makes sense when both slices are overlapping.
  Assert(SlicesAreOverlapping(SliceA, SliceB));
  return { Min(First(SliceA), First(SliceB)), Max(OnePastLast(SliceA), OnePastLast(SliceB)) };
}

template<typename ElementTypeA, typename ElementTypeB>
constexpr bool
SlicesAreDisjoint(slice<ElementTypeA> SliceA, slice<ElementTypeB> SliceB)
{
  return Last(SliceA) < First(SliceB) || First(SliceA) > Last(SliceB);
}

/// Whether SliceA and SliceB overlap.
/// \see Contains
template<typename ElementTypeA, typename ElementTypeB>
bool
SlicesAreOverlapping(slice<ElementTypeA> SliceA, slice<ElementTypeB> SliceB)
{
  auto UnionOfAB = SliceUnion(SliceA, SliceB);
  return SliceContains(UnionOfAB, SliceA) || SliceContains(UnionOfAB, SliceA);
}

/// Whether SliceA completely contains SliceB.
/// \see AreOverlapping
template<typename ElementTypeA, typename ElementTypeB>
constexpr bool
SliceContains(slice<ElementTypeA> SliceA, slice<ElementTypeB> SliceB)
{
  return First(SliceA) <= First(SliceB) && OnePastLast(SliceA) >= OnePastLast(SliceB);
}

template<typename ElementType>
constexpr slice<ElementType>
Slice(size_t Num, ElementType* Ptr)
{
  return { Num, Ptr };
}

template<typename ElementType>
slice<ElementType>
Slice(ElementType* FirstPtr, ElementType* OnePastLastPtr)
{
  auto OnePastLastPtr_ = NonVoidPtr(OnePastLastPtr);
  auto FirstPtr_       = NonVoidPtr(FirstPtr);

  DebugCode(
  {
    auto A = Reinterpret<size_t>(FirstPtr);
    auto B = Reinterpret<size_t>(OnePastLastPtr);
    auto Delta = Max(A, B) - Min(A, B);
    Assert(Delta % SizeOf<ElementType>() == 0);
  });

  slice<ElementType> Result;
  Result.Num = OnePastLastPtr_ <= FirstPtr_ ? 0 : OnePastLastPtr_ - FirstPtr_;
  Result.Ptr = FirstPtr;
  return Result;
}

template<typename ElementType, size_t N>
constexpr slice<ElementType>
Slice(ElementType (&Array)[N])
{
  return { N, &Array[0] };
}

/// Create a char slice from a static char array, excluding '\0'.
template<size_t N>
constexpr slice<char const>
SliceFromString(char const(&StringLiteral)[N])
{
  return { N - 1, &StringLiteral[0] };
}

/// \param StringPtr Must be null-terminated.
slice<char const>
SliceFromString(char const* StringPtr);

/// \param StringPtr Must be null-terminated.
slice<char>
SliceFromString(char* StringPtr);

/// Creates a new slice from an existing slice.
///
/// \param InclusiveStartIndex The index to start slicing from.
/// \param ExclusiveEndIndex The index of the first excluded element.
template<typename ElementType, typename StartIndexType, typename EndIndexType>
slice<ElementType>
Slice(slice<ElementType> SomeSlice, StartIndexType InclusiveStartIndex, EndIndexType ExclusiveEndIndex)
{
  Assert(InclusiveStartIndex <= ExclusiveEndIndex);
  slice<ElementType> Result;
  Result.Num = ExclusiveEndIndex - InclusiveStartIndex;
  Result.Ptr = MemAddOffset(SomeSlice.Ptr, InclusiveStartIndex);
  BoundsCheck(SliceContains(SomeSlice, Result));
  return Result;
}

/// Creates a new slice from an existing one, trimming elements at the beginning.
template<typename ElementType, typename AmountType>
constexpr slice<ElementType>
SliceTrimFront(slice<ElementType> SomeSlice, AmountType Amount)
{
  return {
    Amount > SomeSlice.Num ? 0 : SomeSlice.Num - Amount,
    MemAddOffset(SomeSlice.Ptr, Amount)
  };
}

/// Creates a new slice from an existing one, trimming elements at the beginning.
template<typename ElementType, typename AmountType>
constexpr slice<ElementType>
SliceTrimBack(slice<ElementType> SomeSlice, AmountType Amount)
{
  return {
    Amount > SomeSlice.Num ? 0 : SomeSlice.Num - Amount,
    SomeSlice.Ptr
  };
}

/// Copies the contents of one slice into another.
///
/// If the number of elements don't match, the minimum of both is used.
template<typename T>
size_t
SliceCopy(slice<T> Target, slice<T const> Source)
{
  size_t Amount = Min(Target.Num, Source.Num);
  MemCopy(Amount, Target.Ptr, Source.Ptr);
  return Amount;
}

/// Set each element of Target to Value.
template<typename T>
void
SliceSet(slice<T> Target, T const Value)
{
  MemSet(Target.Num, Target.Ptr, Forward<T const>(Value));
}

template<typename T>
size_t
SliceMove(slice<T> Target, slice<T const> Source)
{
  // TODO Optimize for PODs

  size_t Amount = Min(Target.Num, Source.Num);
  for(size_t Index = 0; Index < Amount; ++Index)
  {
    Target.Ptr[Index] = Move(Source.Ptr[Index]);
  }
  return Amount;
}

template<typename T>
void
SliceDefaultConstruct(slice<T> Target)
{
  MemDefaultConstruct(Target.Num, Target.Ptr);
}

template<typename T>
size_t
SliceCopyConstruct(slice<T> Target, slice<T const> Source)
{
  size_t Amount = Min(Target.Num, Source.Num);
  MemCopy(Amount, Target.Ptr, SourcePtr);
  return Amount;
}

template<typename T>
size_t
SliceMoveConstruct(slice<T> Target, slice<T const> Source)
{
  size_t Amount = Min(Target.Num, Source.Num);
  MemMove(Amount, Target.Ptr, SourcePtr);
  return Amount;
}

template<typename T>
void
SliceDestruct(slice<T> Target)
{
  MemDestruct(Target.Num, Target.Ptr);
}

template<typename T, typename NeedleType>
size_t
SliceCountUntil(slice<T const> Haystack, const NeedleType& Needle)
{
  size_t Index = 0;

  for(auto& Straw : Haystack)
  {
    if(Straw == Needle)
      return Index;
    ++Index;
  }

  return INVALID_INDEX;
}

/// Counts up until \c Predicate(ElementOfHaystack, Needle) returns \c true.
template<typename T, typename NeedleType, typename PredicateType>
size_t
SliceCountUntil(slice<T const> Haystack, const NeedleType& Needle, PredicateType Predicate)
{
  size_t Index = 0;

  for(auto& Straw : Haystack)
  {
    if(Predicate(Straw, Needle))
      return Index;
    ++Index;
  }

  return INVALID_INDEX;
}


/// Compares the contents of the two slices for equality.
///
/// Two slices are deemed equal if they have the same number of elements and
/// each individual element in A compares equal to the corresponding element
/// in B in the order they appear in.
template<typename ElementTypeA, typename ElementTypeB>
bool
operator ==(slice<ElementTypeA> A, slice<ElementTypeB> B)
{
  if(A.Num != B.Num) return false;

  auto A_ = NonVoidPtr(First(A));
  auto B_ = NonVoidPtr(First(B));
  // if(A_ == B_) return true;
  if(Coerce<size_t>(A_) == Coerce<size_t>(B_)) return true;

  auto NumElements = A.Num;
  while(NumElements)
  {
    if(*A_ != *B_)
      return false;

    ++A_;
    ++B_;
    --NumElements;
  }

  return true;
}


template<typename ElementType>
bool
operator ==(slice<ElementType> Slice, nullptr_t)
{
  return !Cast<bool>(Slice);
}
template<typename ElementType>
bool
operator !=(slice<ElementType> Slice, nullptr_t)
{
  return Cast<bool>(Slice);
}

template<typename ElementType>
bool
operator ==(nullptr_t, slice<ElementType> Slice)
{
  return !(Slice == nullptr);
}
template<typename ElementType>
bool
operator !=(nullptr_t, slice<ElementType> Slice)
{
  return Slice != nullptr;
}

template<typename ElementTypeA, typename ElementTypeB>
bool
operator !=(slice<ElementTypeA> A, slice<ElementTypeB> B)
{
  return !(A == B);
}

template<typename ElementType>
constexpr size_t
SliceByteSize(slice<ElementType> S)
{
  return S.Num * SizeOf<ElementType>();
}

//]]~~
