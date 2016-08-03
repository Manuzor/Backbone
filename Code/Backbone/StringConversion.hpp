#pragma once

#include "Common.hpp"
#include "Slice.hpp"

//~~[[

//
// Conversion: String -> Floating Point
//

double
ImplConvertStringToDouble(slice<char const>* Source, bool* Success, double Fallback);

template<typename FloatType>
struct impl_convert_string_to_floating_point_helper
{
  template<typename CharType>
  static inline FloatType
  Do(slice<CharType> String, bool* Success = nullptr, FloatType Fallback = NaN<FloatType>())
  {
    auto Result = ImplConvertStringToDouble(Coerce<slice<char const>*>(&String), Success, Cast<double>(Fallback));
    return Cast<FloatType>(Result);
  }

  template<typename CharType>
  static inline FloatType
  Do(slice<CharType>* String, bool* Success = nullptr, FloatType Fallback = NaN<FloatType>())
  {
    auto Result = ImplConvertStringToDouble(Coerce<slice<char const>*>(String), Success, Cast<double>(Fallback));
    return Cast<FloatType>(Result);
  }
};

template<> struct impl_convert<double, slice<char>>        : public impl_convert_string_to_floating_point_helper<double> {};
template<> struct impl_convert<double, slice<char>*>       : public impl_convert_string_to_floating_point_helper<double> {};
template<> struct impl_convert<double, slice<char const>>  : public impl_convert_string_to_floating_point_helper<double> {};
template<> struct impl_convert<double, slice<char const>*> : public impl_convert_string_to_floating_point_helper<double> {};
template<> struct impl_convert<float,  slice<char>>        : public impl_convert_string_to_floating_point_helper<float>  {};
template<> struct impl_convert<float,  slice<char>*>       : public impl_convert_string_to_floating_point_helper<float>  {};
template<> struct impl_convert<float,  slice<char const>>  : public impl_convert_string_to_floating_point_helper<float>  {};
template<> struct impl_convert<float,  slice<char const>*> : public impl_convert_string_to_floating_point_helper<float>  {};

//
// Conversion: String -> Unsigned Integer
//

uint64
ImplConvertStringToInteger(slice<char const>* Source, bool* Success, uint64 Fallback);

int64
ImplConvertStringToInteger(slice<char const>* Source, bool* Success, int64 Fallback);

template<typename IntegerType>
struct impl_convert_string_to_integer_helper
{
  template<bool IsSigned> struct impl_max_integer_type;
  template<> struct impl_max_integer_type<true>  { using Type = int64;  };
  template<> struct impl_max_integer_type<false> { using Type = uint64; };

  using MaxIntegerType = typename impl_max_integer_type<IntIsSigned<IntegerType>()>::Type;

  template<typename CharType>
  static inline IntegerType
  Do(slice<CharType> String, bool* Success = nullptr, IntegerType Fallback = 0)
  {
    auto Result = ImplConvertStringToInteger(Coerce<slice<char const>*>(&String), Success, Cast<MaxIntegerType>(Fallback));
    return Convert<IntegerType>(Result);
  }

  template<typename CharType>
  static inline IntegerType
  Do(slice<CharType>* String, bool* Success = nullptr, IntegerType Fallback = 0)
  {
    auto Result = ImplConvertStringToInteger(Coerce<slice<char const>*>(String), Success, Cast<MaxIntegerType>(Fallback));
    return Convert<IntegerType>(Result);
  }
};

template<> struct impl_convert<int8,   slice<char>>        : public impl_convert_string_to_integer_helper<int8>   {};
template<> struct impl_convert<int8,   slice<char>*>       : public impl_convert_string_to_integer_helper<int8>   {};
template<> struct impl_convert<int8,   slice<char const>>  : public impl_convert_string_to_integer_helper<int8>   {};
template<> struct impl_convert<int8,   slice<char const>*> : public impl_convert_string_to_integer_helper<int8>   {};
template<> struct impl_convert<int16,  slice<char>>        : public impl_convert_string_to_integer_helper<int16>  {};
template<> struct impl_convert<int16,  slice<char>*>       : public impl_convert_string_to_integer_helper<int16>  {};
template<> struct impl_convert<int16,  slice<char const>>  : public impl_convert_string_to_integer_helper<int16>  {};
template<> struct impl_convert<int16,  slice<char const>*> : public impl_convert_string_to_integer_helper<int16>  {};
template<> struct impl_convert<int32,  slice<char>>        : public impl_convert_string_to_integer_helper<int32>  {};
template<> struct impl_convert<int32,  slice<char>*>       : public impl_convert_string_to_integer_helper<int32>  {};
template<> struct impl_convert<int32,  slice<char const>>  : public impl_convert_string_to_integer_helper<int32>  {};
template<> struct impl_convert<int32,  slice<char const>*> : public impl_convert_string_to_integer_helper<int32>  {};
template<> struct impl_convert<int64,  slice<char>>        : public impl_convert_string_to_integer_helper<int64>  {};
template<> struct impl_convert<int64,  slice<char>*>       : public impl_convert_string_to_integer_helper<int64>  {};
template<> struct impl_convert<int64,  slice<char const>>  : public impl_convert_string_to_integer_helper<int64>  {};
template<> struct impl_convert<int64,  slice<char const>*> : public impl_convert_string_to_integer_helper<int64>  {};

template<> struct impl_convert<uint8,  slice<char>>        : public impl_convert_string_to_integer_helper<uint8>  {};
template<> struct impl_convert<uint8,  slice<char>*>       : public impl_convert_string_to_integer_helper<uint8>  {};
template<> struct impl_convert<uint8,  slice<char const>>  : public impl_convert_string_to_integer_helper<uint8>  {};
template<> struct impl_convert<uint8,  slice<char const>*> : public impl_convert_string_to_integer_helper<uint8>  {};
template<> struct impl_convert<uint16, slice<char>>        : public impl_convert_string_to_integer_helper<uint16> {};
template<> struct impl_convert<uint16, slice<char>*>       : public impl_convert_string_to_integer_helper<uint16> {};
template<> struct impl_convert<uint16, slice<char const>>  : public impl_convert_string_to_integer_helper<uint16> {};
template<> struct impl_convert<uint16, slice<char const>*> : public impl_convert_string_to_integer_helper<uint16> {};
template<> struct impl_convert<uint32, slice<char>>        : public impl_convert_string_to_integer_helper<uint32> {};
template<> struct impl_convert<uint32, slice<char>*>       : public impl_convert_string_to_integer_helper<uint32> {};
template<> struct impl_convert<uint32, slice<char const>>  : public impl_convert_string_to_integer_helper<uint32> {};
template<> struct impl_convert<uint32, slice<char const>*> : public impl_convert_string_to_integer_helper<uint32> {};
template<> struct impl_convert<uint64, slice<char>>        : public impl_convert_string_to_integer_helper<uint64> {};
template<> struct impl_convert<uint64, slice<char>*>       : public impl_convert_string_to_integer_helper<uint64> {};
template<> struct impl_convert<uint64, slice<char const>>  : public impl_convert_string_to_integer_helper<uint64> {};
template<> struct impl_convert<uint64, slice<char const>*> : public impl_convert_string_to_integer_helper<uint64> {};

//]]~~
