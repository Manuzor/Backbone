
#include "Slice.hpp"

//~~[[

auto
::SliceFromString(char const* StringPtr)
  -> slice<char const>
{
  auto Seek = StringPtr;
  size_t Count = 0;
  while(*Seek++) ++Count;
  return { Count, StringPtr };
}

auto
::SliceFromString(char* StringPtr)
  -> slice<char>
{
  auto Constified = Coerce<char const*>(StringPtr);
  auto ConstResult = SliceFromString(Constified);
  slice<char> Result;
  Result.Num = ConstResult.Num;
  Result.Ptr = Coerce<char*>(ConstResult.Ptr);
  return Result;
}

//]]~~
