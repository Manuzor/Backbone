#pragma once

#include "Common.hpp"
#include "Slice.hpp"

//~~[[

template<size_t N, typename t_element>
struct fixed_block
{
  using element_type = t_element;

  static constexpr size_t Num = N;
  element_type Data[Num];

  template<typename t_index>
  auto
  operator[](t_index Index)
    -> decltype(Data[Index])
  {
    BoundsCheck(Index >= 0 && Index < Num);
    return Data[Index];
  }

  template<typename t_index>
  auto
  operator[](t_index Index) const
    -> decltype(Data[Index])
  {
    BoundsCheck(Index >= 0 && Index < Num);
    return Data[Index];
  }
};

template<size_t N, typename T>
constexpr typename fixed_block<N, T>::element_type*
First(fixed_block<N, T>& Block)
{
  return &Block.Data[0];
}

template<size_t N, typename T>
constexpr typename fixed_block<N, T>::element_type const*
First(fixed_block<N, T> const& Block)
{
  return &Block.Data[0];
}

template<size_t N, typename T>
constexpr typename fixed_block<N, T>::element_type*
Last(fixed_block<N, T>& Block)
{
  return MemAddOffset(First(Block), N - 1);
}

template<size_t N, typename T>
constexpr typename fixed_block<N, T>::element_type const*
Last(fixed_block<N, T> const& Block)
{
  return MemAddOffset(First(Block), N - 1);
}

template<size_t N, typename T>
constexpr typename fixed_block<N, T>::element_type*
OnePastLast(fixed_block<N, T>& Block)
{
  return MemAddOffset(First(Block), N);
}

template<size_t N, typename T>
constexpr typename fixed_block<N, T>::element_type const*
OnePastLast(fixed_block<N, T> const& Block)
{
  return MemAddOffset(First(Block), N);
}

/// C++11 range API
template<size_t N, typename t_element>
constexpr t_element*
begin(fixed_block<N, t_element>& Block)
{
  return First(Block);
}

/// C++11 range API
template<size_t N, typename t_element>
t_element*
end(fixed_block<N, t_element>& Block)
{
  return OnePastLast(Block);
}

/// C++11 range API
template<size_t N, typename t_element>
t_element*
begin(fixed_block<N, t_element> const& Block)
{
  return First(Block);
}

/// C++11 range API
template<size_t N, typename t_element>
t_element*
end(fixed_block<N, t_element> const& Block)
{
  return OnePastLast(Block);
}

template<size_t N, typename t_element>
slice<t_element>
Slice(fixed_block<N, t_element>& Block)
{
  return { N, begin(Block) };
}

template<size_t N, typename t_element>
slice<t_element const>
Slice(fixed_block<N, t_element> const& Block)
{
  return { N, begin(Block) };
}

template<size_t N, typename t_element, typename t_start_index, typename t_end_index>
slice<t_element>
Slice(fixed_block<N, t_element>& Block, t_start_index InclusiveStartIndex, t_end_index ExclusiveEndIndex)
{
  return Slice(Slice(Block), InclusiveStartIndex, ExclusiveEndIndex);
}

template<size_t N, typename t_element, typename t_start_index, typename t_end_index>
slice<t_element const>
Slice(fixed_block<N, t_element> const& Block, t_start_index InclusiveStartIndex, t_end_index ExclusiveEndIndex)
{
  return Slice(Slice(Block), InclusiveStartIndex, ExclusiveEndIndex);
}

//]]~~
