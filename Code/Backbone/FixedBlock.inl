

template<size_t N, typename t_element>
template<typename t_index>
auto
fixed_block<N, t_element>::operator[](t_index Index)
  -> decltype(Data[Index])
{
  BoundsCheck(Index >= 0 && Index < Num);
  return Data[Index];
}

template<size_t N, typename t_element>
template<typename t_index>
auto
fixed_block<N, t_element>::operator[](t_index Index) const
  -> decltype(Data[Index])
{
  BoundsCheck(Index >= 0 && Index < Num);
  return Data[Index];
}

/// C++11 range API
template<size_t N, typename t_element>
auto
::begin(fixed_block<N, t_element>& Block)
  -> t_element*
{
  return &Block.Data[0];
}

/// C++11 range API
template<size_t N, typename t_element>
auto
::end(fixed_block<N, t_element>& Block)
  -> t_element*
{
  return begin(Block) + Block.N;
}

/// C++11 range API
template<size_t N, typename t_element>
auto
::begin(fixed_block<N, t_element> const& Block)
  -> t_element*
{
  return &Block.Data[0];
}

/// C++11 range API
template<size_t N, typename t_element>
auto
::end(fixed_block<N, t_element> const& Block)
  -> t_element*
{
  return begin(Block) + Block.N;
}

template<size_t N, typename t_element>
auto
::Slice(fixed_block<N, t_element>& Block)
  -> slice<t_element>
{
  return { N, begin(Block) };
}

template<size_t N, typename t_element, typename t_start_index, typename t_end_index>
auto
::Slice(fixed_block<N, t_element>& Block, t_start_index InclusiveStartIndex, t_end_index ExclusiveEndIndex)
  -> slice<t_element>
{
  return Slice(Slice(Block), InclusiveStartIndex, ExclusiveEndIndex);
}
