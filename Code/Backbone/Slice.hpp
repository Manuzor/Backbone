#pragma once

#include "Common.hpp"

//~~[[

template<typename t_element>
struct slice
{
  using element_type = t_element;

  size_t Num;
  element_type* Data;

  /// Test whether this slice is valid or not.
  ///
  /// A slice is considered valid if it does not point to null and contains at
  /// least one element.
  operator bool() const { return Num && Data; }

  /// Index operator to access elements of the slice.
  template<typename t_index_type>
  auto
  operator[](t_index_type Index) const
    -> decltype(Data[Index])
  {
    BoundsCheck(Index >= 0 && Index < Num);
    return Data[Index];
  }
};

template<>
struct slice<void>
{
  using element_type = void;

  size_t Num;
  element_type* Data;

  /// Test whether this slice is valid or not.
  ///
  /// A slice is considered valid if it does not point to null and contains at
  /// least one element.
  operator bool() const { return Num && Data; }
};

template<>
struct slice<void const>
{
  using element_type = void const;

  size_t Num;
  element_type* Data;

  /// Test whether this slice is valid or not.
  ///
  /// A slice is considered valid if it does not point to null and contains at
  /// least one element.
  operator bool() const { return Num && Data; }
};

template<typename t_type>
typename slice<t_type>::element_type*
First(slice<t_type> const& SomeSlice)
{
  return SomeSlice.Data;
}

template<typename t_type>
typename slice<t_type>::element_type*
Last(slice<t_type> const& SomeSlice)
{
  return MemAddOffset(First(SomeSlice), Max(1, SomeSlice.Num) - 1);
}

template<typename t_type>
typename slice<t_type>::element_type*
OnePastLast(slice<t_type> const& SomeSlice)
{
  return MemAddOffset(First(SomeSlice), SomeSlice.Num);
}

/// C++11 range API
template<typename t_type>
typename slice<t_type>::element_type*
begin(slice<t_type> const& SomeSlice)
{
  return First(SomeSlice);
}

/// C++11 range API
template<typename t_type>
typename slice<t_type>::element_type*
end(slice<t_type> const& SomeSlice)
{
  return OnePastLast(SomeSlice);
}

template<typename t_target, typename t_source>
slice<t_target>
SliceReinterpret(slice<t_source> SomeSlice)
{
  return CreateSlice(Reinterpret<t_target*>(First(SomeSlice)),
                     Reinterpret<t_target*>(OnePastLast(SomeSlice)));
}

template<typename t_source>
slice<t_source const>
SliceAsConst(slice<t_source> SomeSlice)
{
  return CreateSlice(AsPtrToConst(First(SomeSlice)),
                     AsPtrToConst(OnePastLast(SomeSlice)));
}

/// Concatenate two slices together.
///
/// \return The returned slice will be a subset of the given Buffer, which is
/// \used to write the actual result in.
template<typename t_element>
slice<t_element>
SliceConcat(slice<t_element const> Head, slice<t_element const> Tail, slice<t_element> Buffer)
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
template<typename t_element>
constexpr slice<t_element>
SliceUnion(slice<t_element> SliceA, slice<t_element> SliceB)
{
  // A union only makes sense when both slices are overlapping.
  Assert(SlicesAreOverlapping(SliceA, SliceB));
  return { Min(First(SliceA), First(SliceB)), Max(OnePastLast(SliceA), OnePastLast(SliceB)) };
}

template<typename t_element_a, typename t_element_b>
constexpr bool
SlicesAreDisjoint(slice<t_element_a> SliceA, slice<t_element_b> SliceB)
{
  return Last(SliceA) < First(SliceB) || First(SliceA) > Last(SliceB);
}

/// Whether SliceA and SliceB overlap.
/// \see Contains
template<typename t_element_a, typename t_element_b>
bool
SlicesAreOverlapping(slice<t_element_a> SliceA, slice<t_element_b> SliceB)
{
  auto UnionOfAB = SliceUnion(SliceA, SliceB);
  return SliceContains(UnionOfAB, SliceA) || SliceContains(UnionOfAB, SliceA);
}

/// Whether SliceA completely contains SliceB.
/// \see AreOverlapping
template<typename t_element_a, typename t_element_b>
constexpr bool
SliceContains(slice<t_element_a> SliceA, slice<t_element_b> SliceB)
{
  return First(SliceA) <= First(SliceB) && OnePastLast(SliceA) >= OnePastLast(SliceB);
}

template<typename t_element>
constexpr slice<t_element>
CreateSlice(size_t Num, t_element* Data)
{
  return { Num, Data };
}

template<typename t_element>
slice<t_element>
CreateSlice(t_element* Begin, t_element* End)
{
  auto EndInt = Reinterpret<size_t>(End);
  auto BeginInt = Reinterpret<size_t>(Begin);

  slice<t_element> Result;
  Result.Num = EndInt < BeginInt ? 0 : BeginInt - EndInt;
  Result.Data = Begin;
  return Result;
}

template<typename t_element, size_t N>
slice<t_element>
CreateSlice(t_element (&Array)[N])
{
  return { N, &Array[0] };
}

/// Create a char slice from a static char array, excluding '\0'.
template<size_t N>
slice<char const>
CreateSliceFromString(char const(&StringLiteral)[N])
{
  return { N - 1, &StringLiteral[0] };
}

/// \param StringPtr Must be null-terminated.
inline
slice<char const>
CreateSliceFromString(char const* StringPtr)
{
  auto Seek = StringPtr;
  size_t Count = 0;
  while(*Seek++) ++Count;
  return { Count, StringPtr };
}

/// \param StringPtr Must be null-terminated.
inline
slice<char>
CreateSliceFromString(char* StringPtr)
{
  auto Constified = Coerce<char const*>(StringPtr);
  auto Result = CreateSliceFromString(Constified);
  return CreateSlice(Coerce<char*>(First(Result)),
                     Coerce<char*>(First(Result)));
}

/// Creates a new slice from an existing slice.
///
/// \param InclusiveStartIndex The index to start slicing from.
/// \param ExclusiveEndIndex The index of the first excluded element.
template<typename t_element, typename t_start_index, typename t_end_index>
slice<t_element>
Slice(slice<t_element> SomeSlice, t_start_index InclusiveStartIndex, t_end_index ExclusiveEndIndex)
{
  Assert(InclusiveStartIndex <= ExclusiveEndIndex);
  slice<t_element> Result;
  Result.Num = ExclusiveEndIndex - InclusiveStartIndex;
  Result.Data = MemAddOffset(SomeSlice.Data, InclusiveStartIndex);
  BoundsCheck(SliceContains(SomeSlice, Result));
  return Result;
}

/// Copies the contents of one slice into another.
///
/// If the number of elements don't match, the minimum of both is used.
template<typename t_type>
size_t
SliceCopy(slice<t_type> Target, slice<t_type const> Source)
{
  size_t Amount = Min(Target.Num, Source.Num);
  // TODO(Manu): memcpy?
  // size_t Bytes = Amount * sizeof(t_type);
  // ::memcpy(Target.Data, Source.Data, Bytes);
  for(size_t Index = 0; Index < Amount; ++Index)
  {
    Target.Data[Index] = Source.Data[Index];
  }
  return Amount;
}

/// Compares the contents of the two slices for equality.
///
/// Two slices are deemed equal if they have the same number of elements and
/// each individual element in A compares equal to the corresponding element
/// in B in the order they appear in.
template<typename t_element_a, typename t_element_b>
bool
operator ==(slice<t_element_a> A, slice<t_element_b> B)
{
  if(A.Num != B.Num) return false;

  auto A_ = NonVoidPtr(First(A));
  auto B_ = NonVoidPtr(First(B));
  // if(A_ == B_) return true;
  if(Coerce<size_t>(A_) == Coerce<size_t>(B_)) return true;

  auto NumElements = A.Num;
  while(NumElements)
  {
    if(*A_ == *B_)
      --NumElements;
    else
      return false;
  }

  return true;
}

template<typename t_element_a, typename t_element_b>
bool
operator !=(slice<t_element_a> A, slice<t_element_b> B)
{
  return !(A == B);
}

//]]~~
