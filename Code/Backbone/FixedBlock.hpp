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
    -> decltype(Data[Index]);

  template<typename t_index>
  auto
  operator[](t_index Index) const
    -> decltype(Data[Index]);
};

/// C++11 range API
template<size_t N, typename t_element>
t_element*
begin(fixed_block<N, t_element>& Block);

/// C++11 range API
template<size_t N, typename t_element>
t_element*
end(fixed_block<N, t_element>& Block);

/// C++11 range API
template<size_t N, typename t_element>
t_element*
begin(fixed_block<N, t_element> const& Block);

/// C++11 range API
template<size_t N, typename t_element>
t_element*
end(fixed_block<N, t_element> const& Block);

template<size_t N, typename t_element>
slice<t_element>
Slice(fixed_block<N, t_element>& Block);

template<size_t N, typename t_element, typename t_start_index, typename t_end_index>
slice<t_element>
Slice(fixed_block<N, t_element>& Block, t_start_index InclusiveStartIndex, t_end_index ExclusiveEndIndex);

//]]~~

#include "FixedBlock.inl"
