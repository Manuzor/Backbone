#pragma once

// TODO: Get rid of this?
#include <limits>
#include <cmath> // floor, ceil, round, etc.

//~~[[

#if !defined(BB_Platform_Windows)
  #error The Backbone is only working on windows platforms for now.
#endif

#if !defined(BB_Inline)
  #define BB_Inline inline
#endif

#if !defined(BB_ForceInline)
  #define BB_ForceInline inline
#endif

#define NoOp do{  }while(0)

#define Crash() *(int*)nullptr = 0

#if defined(BB_Enable_Assert)
  #define Assert(Expression) do{ if(!(Expression)) { Crash(); } } while(0)
#else
  #define Assert(Expression) NoOp
#endif

#if defined(BB_Enable_BoundsCheck)
  #define BoundsCheck(...) Assert(__VA_ARGS__)
#else
  #define BoundsCheck(...) NoOp
#endif

#ifdef DEBUG
  #define BB_Debugging 1
#endif

#if BB_Debugging
  #define DebugCode(...) __VA_ARGS__
#else
  #define DebugCode(...) /* Empty */
#endif

//
// ================
//

using int8  = char;
using int16 = short;
using int32 = int;
using int64 = long long;

using uint8  = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using bool32 = int32;

/// Generic unsigned integer if you don't care much about the actual size.
using uint = unsigned int;


/// Defines some array variants of types for better readability when used as
/// function parameters.
///
/// For example, a function `Foo` that accepts and array of 4 floats by
/// reference-to-const looks like this:
/// \code
/// void Foo(float const (&ParamName)[4]);
/// \endcode
///
/// Using these typedefs, this can be transformed into:
/// \code
/// void Foo(float_4 const& ParamName);
/// \endcode
#define DefineArrayTypes(TheType)\
  using TheType##_2   = TheType[2];\
  using TheType##_3   = TheType[3];\
  using TheType##_4   = TheType[4];\
  using TheType##_2x2 = TheType##_2[2];\
  using TheType##_2x3 = TheType##_2[3];\
  using TheType##_2x4 = TheType##_2[4];\
  using TheType##_3x2 = TheType##_3[2];\
  using TheType##_3x3 = TheType##_3[3];\
  using TheType##_3x4 = TheType##_3[4];\
  using TheType##_4x2 = TheType##_4[2];\
  using TheType##_4x3 = TheType##_4[3];\
  using TheType##_4x4 = TheType##_4[4];

DefineArrayTypes(float);
DefineArrayTypes(double);

DefineArrayTypes(int8);
DefineArrayTypes(int16);
DefineArrayTypes(int32);
DefineArrayTypes(int64);

DefineArrayTypes(uint8);
DefineArrayTypes(uint16);
DefineArrayTypes(uint32);
DefineArrayTypes(uint64);


//
// ================
//

/// Used to generate compile-time errors when the same name prefix is used in
/// different locations.
///
/// Usage:
/// \code
/// RESERVE_PREFIX(Mem);
///
/// // Use the "Mem" prefix here without having to worry too much about others
/// // using the same prefix.
// / void MemCopy( /* ... */ ) { /* ... */ }
///
/// // By convention, this should also be safe.
/// struct mem_thing { /* ... */ };
///
/// // The following would trigger a compile-time error.
/// // RESERVE_PREFIX(Mem);
///
/// \endcode
///
/// Note that this is not a fool-proof system, it's just a tool to help you
/// keep your code clean.
#define RESERVE_PREFIX(Prefix) struct reserved_prefix_##Prefix {}


//
// ================
//

struct memory_size
{
  uint64 InternalBytes;

  constexpr
  operator size_t() const
  {
    // TODO: Ensure safe conversion?
    return (size_t)InternalBytes;
  }
};

constexpr bool operator ==(memory_size A, memory_size B) { return A.InternalBytes == B.InternalBytes; }
constexpr bool operator !=(memory_size A, memory_size B) { return A.InternalBytes != B.InternalBytes; }
constexpr bool operator < (memory_size A, memory_size B) { return A.InternalBytes <  B.InternalBytes; }
constexpr bool operator <=(memory_size A, memory_size B) { return A.InternalBytes <= B.InternalBytes; }
constexpr bool operator > (memory_size A, memory_size B) { return A.InternalBytes >  B.InternalBytes; }
constexpr bool operator >=(memory_size A, memory_size B) { return A.InternalBytes >= B.InternalBytes; }

constexpr memory_size operator +(memory_size A, memory_size B) { return { A.InternalBytes + B.InternalBytes }; }
constexpr memory_size operator -(memory_size A, memory_size B) { return { A.InternalBytes - B.InternalBytes }; }
constexpr memory_size operator *(memory_size A, uint64 Scale) { return { A.InternalBytes * Scale };   }
constexpr memory_size operator *(uint64 Scale, memory_size A) { return { Scale * A.InternalBytes };   }
constexpr memory_size operator /(memory_size A, uint64 Scale) { return { A.InternalBytes / Scale };   }

void operator +=(memory_size& A, memory_size B);
void operator -=(memory_size& A, memory_size B);
void operator *=(memory_size& A, uint64 Scale);
void operator /=(memory_size& A, uint64 Scale);

constexpr memory_size Bytes(uint64 Amount) { return { Amount }; }
constexpr uint64 ToBytes(memory_size Size) { return Size.InternalBytes; }

constexpr memory_size KiB(uint64 Amount) { return { Amount * 1024 }; }
constexpr memory_size MiB(uint64 Amount) { return { Amount * 1024 * 1024 }; }
constexpr memory_size GiB(uint64 Amount) { return { Amount * 1024 * 1024 * 1024 }; }
constexpr memory_size TiB(uint64 Amount) { return { Amount * 1024 * 1024 * 1024 * 1024 }; }

constexpr memory_size KB(uint64 Amount) { return { Amount * 1000 }; }
constexpr memory_size MB(uint64 Amount) { return { Amount * 1000 * 1000 }; }
constexpr memory_size GB(uint64 Amount) { return { Amount * 1000 * 1000 * 1000 }; }
constexpr memory_size TB(uint64 Amount) { return { Amount * 1000 * 1000 * 1000 * 1000 }; }

template<typename OutputType = float> constexpr OutputType ToKiB(memory_size Size) { return OutputType(double(Size.InternalBytes) / 1024); }
template<typename OutputType = float> constexpr OutputType ToMiB(memory_size Size) { return OutputType(double(Size.InternalBytes) / 1024 / 1024); }
template<typename OutputType = float> constexpr OutputType ToGiB(memory_size Size) { return OutputType(double(Size.InternalBytes) / 1024 / 1024 / 1024); }
template<typename OutputType = float> constexpr OutputType ToTiB(memory_size Size) { return OutputType(double(Size.InternalBytes) / 1024 / 1024 / 1024 / 1024); }

template<typename OutputType = float> constexpr OutputType ToKB(memory_size Size) { return OutputType(double(Size.InternalBytes) / 1000); }
template<typename OutputType = float> constexpr OutputType ToMB(memory_size Size) { return OutputType(double(Size.InternalBytes) / 1000 / 1000); }
template<typename OutputType = float> constexpr OutputType ToGB(memory_size Size) { return OutputType(double(Size.InternalBytes) / 1000 / 1000 / 1000); }
template<typename OutputType = float> constexpr OutputType ToTB(memory_size Size) { return OutputType(double(Size.InternalBytes) / 1000 / 1000 / 1000 / 1000); }


constexpr uint32 SetBit(uint32 Bits, uint32 Position) { return Bits | (uint32(1) << Position); }
constexpr uint32 UnsetBit(uint32 Bits, uint32 Position) { return Bits & ~(uint32(1) << Position); }
constexpr bool IsBitSet(uint32 Bits, uint32 Position) { return !!(Bits & (uint32(1) << Position)); }

constexpr uint64 SetBit(uint64 Bits, uint64 Position) { return Bits | (uint64(1) << Position); }
constexpr uint64 UnsetBit(uint64 Bits, uint64 Position) { return Bits & ~(uint64(1) << Position); }
constexpr bool IsBitSet(uint64 Bits, uint64 Position) { return !!(Bits & (uint64(1) << Position)); }

constexpr bool IsPowerOfTwo(size_t Value) { return Value < 1 ? false : (Value & (Value - 1)) == 0; }

//
// ================
//

template<typename T = float>
constexpr T
Pi() { return (T)3.14159265359; }

template<typename T = float>
constexpr T
E() { return (T)2.71828182845; }


//
// ================
//

template<typename t_type, size_t N>
constexpr size_t
ArrayCount(t_type(&)[N]) { return N; }

template<typename T> struct impl_size_of { enum { SizeInBytes = sizeof(T) }; };
template<>           struct impl_size_of<void>          : impl_size_of<uint8>          { };
template<>           struct impl_size_of<void const>    : impl_size_of<uint8 const>    { };
template<>           struct impl_size_of<void volatile> : impl_size_of<uint8 volatile> { };

/// Get the size of type T in bytes.
///
/// Same as sizeof(T) except it works also with 'void' (possibly cv-qualified) where a size of 1 byte is assumed.
template<typename T>
constexpr memory_size
SizeOf() { return Bytes(impl_size_of<T>::SizeInBytes); }

/// Reinterpretation of the given pointer in case t_pointer_type is `void`.
template<typename t_pointer_type>
constexpr t_pointer_type*
NonVoidPtr(t_pointer_type* Ptr)
{
  return Ptr;
}

/// Reinterpretation of the given pointer in case t_pointer_type is `void`.
constexpr uint8*
NonVoidPtr(void* Ptr)
{
  return reinterpret_cast<uint8*>(Ptr);
}

/// Reinterpretation of the given pointer in case t_pointer_type is `void`.
constexpr uint8 const*
NonVoidPtr(void const* Ptr)
{
  return reinterpret_cast<uint8 const*>(Ptr);
}

/// Reinterpretation of the given pointer in case t_pointer_type is `void`.
constexpr uint8 volatile*
NonVoidPtr(void volatile* Ptr)
{
  return reinterpret_cast<uint8 volatile*>(Ptr);
}

/// Advance the given pointer value by the given amount of bytes.
template<typename t_pointer_type, typename OffsetType>
constexpr t_pointer_type*
MemAddByteOffset(t_pointer_type* Pointer, OffsetType Offset)
{
  return reinterpret_cast<t_pointer_type*>((uint8*)Pointer + Offset);
}

/// Advance the given pointer value by the given amount times sizeof(t_pointer_type)
template<typename t_pointer_type, typename OffsetType>
constexpr t_pointer_type*
MemAddOffset(t_pointer_type* Pointer, OffsetType Offset)
{
  return MemAddByteOffset(Pointer, Offset * ToBytes(SizeOf<t_pointer_type>()));
}

// TODO: This is MSVC specific right now.
template<typename T> struct impl_is_pod { static constexpr bool Value = __is_pod(T); };
template<>           struct impl_is_pod<void>          : public impl_is_pod<uint8>          {};
template<>           struct impl_is_pod<void const>    : public impl_is_pod<uint8 const>    {};
template<>           struct impl_is_pod<void volatile> : public impl_is_pod<uint8 volatile> {};

/// Whether the given type T is a "plain old data" (POD) type.
///
/// The type 'void' is also considered POD.
template<typename T>
constexpr bool
IsPOD() { return impl_is_pod<T>::Value; }


template<typename NumberType> struct impl_negate { static constexpr NumberType Do(NumberType Value) { return -Value; } };
template<> struct impl_negate<uint8>  { static constexpr uint8  Do(uint8  Value) { return  Value; } };
template<> struct impl_negate<uint16> { static constexpr uint16 Do(uint16 Value) { return  Value; } };
template<> struct impl_negate<uint32> { static constexpr uint32 Do(uint32 Value) { return  Value; } };
template<> struct impl_negate<uint64> { static constexpr uint64 Do(uint64 Value) { return  Value; } };

template<typename NumberType>
NumberType
Negate(NumberType Value)
{
  return impl_negate<NumberType>::Do(Value);
}

template<typename T>
struct impl_is_integer_type { static bool const Value = false; };
template<> struct impl_is_integer_type<uint8>  { static bool const Value = true; };
template<> struct impl_is_integer_type< int8>  { static bool const Value = true; };
template<> struct impl_is_integer_type<uint16> { static bool const Value = true; };
template<> struct impl_is_integer_type< int16> { static bool const Value = true; };
template<> struct impl_is_integer_type<uint32> { static bool const Value = true; };
template<> struct impl_is_integer_type< int32> { static bool const Value = true; };
template<> struct impl_is_integer_type<uint64> { static bool const Value = true; };
template<> struct impl_is_integer_type< int64> { static bool const Value = true; };

template<typename T>
constexpr bool IsIntegerType() { return impl_is_integer_type<T>::Value; }

template<typename T>
struct impl_is_float_type { static bool const Value = false; };
template<> struct impl_is_float_type<float>  { static bool const Value = true; };
template<> struct impl_is_float_type<double> { static bool const Value = true; };

template<typename T>
constexpr bool IsFloatType() { return impl_is_float_type<T>::Value; }

template<typename T>
constexpr bool IsNumberType() { return IsFloatType<T>() || IsIntegerType<T>(); }

/// Get the number of bits of a given type.
///
/// Note: The type 'void' is not supported.
template<typename T>
constexpr size_t
NumBits() { return sizeof(T) * 8; }

template<typename T>
constexpr bool
IntIsSigned() { return ((T)-1) < 0; }

template<typename T>
constexpr T
IntMaxValue()
{
  return IntIsSigned<T>() ? (T(1) << (NumBits<T>() - 1)) - T(1)
                          : T(-1);
}

template<typename T>
constexpr T
IntMinValue()
{
  return IntIsSigned<T>() ? Negate(T(T(1) << (NumBits<T>() - 1)))
                          : T(0);
}

template<typename CharType> struct impl_is_digit_helper { static constexpr bool Do(CharType Char) { return Char >= '0' && Char <= '9'; } };
template<typename CharType> struct impl_is_digit;
template<> struct impl_is_digit<char> : public impl_is_digit_helper<char> {};

template<typename CharType>
constexpr bool
IsDigit(CharType Char)
{
  return impl_is_digit<rm_ref_const<CharType>>::Do(Char);
}

template<typename CharType>
struct impl_is_whitespace_helper
{
  static constexpr bool
  Do(CharType Char)
  {
    return Char == ' '  ||
           Char == '\n' ||
           Char == '\r' ||
           Char == '\t' ||
           Char == '\b';
  }
};

template<typename CharType> struct impl_is_whitespace;
template<> struct impl_is_whitespace<char> : public impl_is_whitespace_helper<char> {};

template<typename CharType>
constexpr bool
IsWhitespace(CharType Char)
{
  return impl_is_whitespace<rm_ref_const<CharType>>::Do(Char);
}

template<typename T> struct impl_nan;
// TODO: Cross-platform.
template<> struct impl_nan<float>  { static constexpr float  Value = std::numeric_limits<float>::quiet_NaN(); };
template<> struct impl_nan<double> { static constexpr double Value = std::numeric_limits<double>::quiet_NaN(); };

/// Returns a quiet Not-A-Number value of the given type.
template<typename T>
constexpr T
NaN()
{
  return impl_nan<T>::Value;
}

template<typename T> struct impl_is_nan;
template<> struct impl_is_nan<float>  { static constexpr bool Do(float  Value) { return Value != Value; } };
template<> struct impl_is_nan<double> { static constexpr bool Do(double Value) { return Value != Value; } };

template<typename T>
constexpr bool
IsNaN(T Value)
{
  return impl_is_nan<T>::Do(Value);
}

template<typename T> struct impl_rm_ref     { using Type = T; };
template<typename T> struct impl_rm_ref<T&> { using Type = T; };

template<typename T>
using rm_ref = typename impl_rm_ref<T>::Type;

template<typename T> struct impl_rm_const          { using Type = T; };
template<typename T> struct impl_rm_const<T const> { using Type = T; };

template<typename T>
using rm_const = typename impl_rm_const<T>::Type;

template<typename T>
using rm_ref_const = rm_const<rm_ref<T>>;

template<class t_type>
constexpr typename rm_ref<t_type>&&
Move(t_type&& Argument)
{
  // forward Argument as movable
  return static_cast<typename rm_ref<t_type>&&>(Argument);
}

template<typename t_type>
constexpr t_type&&
Forward(typename rm_ref<t_type>& Argument)
{
  return static_cast<t_type&&>(Argument);
}

template<typename t_type>
constexpr t_type&&
Forward(rm_ref<t_type>&& Argument)
{
  return static_cast<t_type&&>(Argument);
}

template<typename t_dest, typename t_source>
constexpr t_dest
Cast(t_source Value)
{
  return static_cast<t_dest>(Value);
}

template<typename t_dest, typename t_source>
constexpr t_dest
Reinterpret(t_source Value)
{
  return reinterpret_cast<t_dest>(Value);
}

/// Coerce value of some type to another.
///
/// Basically just a more explicit C-style cast.
template<typename t_dest, typename t_source>
t_dest
Coerce(t_source Value)
{
  t_dest Result = (t_dest)Value;
  return Result;
}

template<typename t_type>
t_type const&
AsConst(t_type& Value)
{
  return const_cast<t_type const&>(Value);
}

template<typename t_type>
t_type const*
AsPtrToConst(t_type* Value)
{
  return const_cast<t_type const*>(Value);
}

template<typename ToType, typename FromType>
struct impl_convert
{
  static constexpr ToType
  Do(FromType const& Value)
  {
    return Cast<ToType>(Value);
  }
};

template<typename ToType, typename FromType, typename... ExtraTypes>
ToType
Convert(FromType const& From, ExtraTypes&&... Extra)
{
  using UnqualifiedToType   = rm_ref_const<ToType>;
  using UnqualifiedFromType = rm_ref_const<FromType>;
  using Impl = impl_convert<UnqualifiedToType, UnqualifiedFromType>;
  return Impl::Do(From, Forward<ExtraTypes>(Extra)...);
}

/// Asserts on overflows and underflows when converting signed or unsigned
/// integers.
///
/// TODO(Manu): Implement what is documented below (needs int_trait).
/// In case of error in non-asserting builds, this will return the
/// corresponding min / max value, instead of letting the overflow / underflow
/// happen.
template<typename DestIntegerType, typename SrcIntegerType>
inline DestIntegerType
SafeConvertInt(SrcIntegerType Value)
{
  auto Result = (DestIntegerType)Value;
  auto RevertedResult = (SrcIntegerType)Result;
  Assert(RevertedResult == Value); // Otherwise something went wrong in the conversion step (overflow/underflow).
  return Result;
}

/// \return 1 for a positive number, -1 for a negative number, 0 otherwise.
template<typename t_type>
constexpr t_type
Sign(t_type I)
{
  return t_type(I > 0 ? 1 : I < 0 ? -1 : 0);
}

template<typename T> struct impl_abs { static constexpr T Do(T Value) { return Sign(Value) * Value; } };
template<> struct impl_abs<uint8>  { static constexpr uint8  Do(uint8  Value) { return Value; } };
template<> struct impl_abs<uint16> { static constexpr uint16 Do(uint16 Value) { return Value; } };
template<> struct impl_abs<uint32> { static constexpr uint32 Do(uint32 Value) { return Value; } };
template<> struct impl_abs<uint64> { static constexpr uint64 Do(uint64 Value) { return Value; } };
template<> struct impl_abs<int8>  { static inline int8  Do(int8  Value) { return Value < 0 ? -Value : Value; } };
template<> struct impl_abs<int16> { static inline int16 Do(int16 Value) { return Value < 0 ? -Value : Value; } };
template<> struct impl_abs<int32> { static inline int32 Do(int32 Value) { return Value < 0 ? -Value : Value; } };
template<> struct impl_abs<int64> { static inline int64 Do(int64 Value) { return Value < 0 ? -Value : Value; } };

template<typename t_type>
constexpr t_type
Abs(t_type Value)
{
  return impl_abs<t_type>::Do(Value);
  // return Sign(I) * I;
}

template<typename t_a_type, typename t_b_type>
constexpr t_a_type
Min(t_a_type A, t_b_type B)
{
  return (B < A) ? Coerce<t_a_type>(B) : A;
}

template<typename t_a_type, typename t_b_type>
constexpr t_a_type
Max(t_a_type A, t_b_type B)
{
  return (B > A) ? Coerce<t_a_type>(B) : A;
}

template<typename t_value_type, typename t_lower_bound_type, typename t_upper_bound_type>
constexpr t_value_type
Clamp(t_value_type Value, t_lower_bound_type LowerBound, t_upper_bound_type UpperBound)
{
  return UpperBound < LowerBound ? Value : Min(UpperBound, Max(LowerBound, Value));
}

// TODO: Make this a constexpr
template<typename t_value_type, typename t_lower_bound_type, typename t_upper_bound_type>
t_value_type
Wrap(t_value_type Value, t_lower_bound_type LowerBound, t_upper_bound_type UpperBound)
{
  const auto BoundsDelta = (Coerce<t_lower_bound_type>(UpperBound) - LowerBound);
  while(Value >= UpperBound) Value -= BoundsDelta;
  while(Value < LowerBound)  Value += BoundsDelta;
  return Value;
  // return Value >= UpperBound ? Value - BoundsDelta :
  //        Value <  LowerBound ? Value + BoundsDelta :
  //                              Value;
}

double
Pow(double Base, double Exponent);

float
Pow(float Base, float Exponent);

template<typename ReturnType = double, typename BaseType, typename ExponentType>
constexpr ReturnType
Pow(BaseType Base, ExponentType Exponent) { return (ReturnType)Pow((double)Base, (double)Exponent); }

double
Mod(double Value, double Divisor);

float
Mod(float Value, float Divisor);

double
Sqrt(double Value);

float
Sqrt(float Value);

template<typename ReturnType = double, typename T>
constexpr ReturnType
Sqrt(T Value) { return (ReturnType)Sqrt((double)Value); }

float
InvSqrt(float Value);

//
// RoundDown
//
template<typename OutputType, typename InputType> struct impl_round_down
{
  static inline OutputType
  Do(InputType Value)
  {
    return Convert<OutputType>(std::floor(Value));
  }
};

/// Also known as the floor-function.
template<typename OutputType, typename InputType>
inline OutputType
RoundDown(InputType Value)
{
  return impl_round_down<OutputType, InputType>::Do(Value);
}

//
// RoundUp
//
template<typename OutputType, typename InputType> struct impl_round_up
{
  static inline OutputType
  Do(InputType Value)
  {
    return Convert<OutputType>(std::ceil(Value));
  }
};

/// Also known as the ceil-function.
template<typename OutputType, typename InputType>
inline OutputType
RoundUp(InputType Value)
{
  return impl_round_up<OutputType, InputType>::Do(Value);
}


//
// RoundTowardsZero
//
template<typename OutputType, typename InputType> struct impl_round_towards_zero
{
  static inline OutputType
  Do(InputType Value)
  {
    return Value > 0 ? RoundDown<OutputType>(Value) : RoundUp<OutputType>(Value);
  }
};

/// Round towards zero.
///
/// Equivalent to \code Value > 0 ? RoundDown(Value) : RoundUp(Value) \endcode
template<typename OutputType, typename InputType>
inline OutputType
RoundTowardsZero(InputType Value)
{
  return impl_round_towards_zero<OutputType, InputType>::Do(Value);
}


//
// RoundAwayFromZero
//
template<typename OutputType, typename InputType> struct impl_round_away_from_zero
{
  static inline OutputType
  Do(InputType Value)
  {
    return Value > 0 ? RoundUp<OutputType>(Value) : RoundDown<OutputType>(Value);
  }
};

/// Round away from zero.
///
/// Equivalent to \code Value > 0 ? RoundUp(Value) : RoundDown(Value) \endcode
template<typename OutputType, typename InputType>
inline OutputType
RoundAwayFromZero(InputType Value)
{
  return impl_round_away_from_zero<OutputType, InputType>::Do(Value);
}


//
// Round
//
template<typename OutputType, typename InputType> struct impl_round
{
  static inline OutputType
  Do(InputType Value)
  {
    return RoundDown<OutputType>(Value + InputType(0.5f));
  }
};

/// Round to the nearest integral value.
template<typename OutputType, typename InputType>
inline OutputType
Round(InputType Value)
{
  return impl_round<OutputType, InputType>::Do(Value);
}


// Project a value from [LowerBound, UpperBound] to [0, 1]
// Example:
//   auto Result = NormalizeValue<float>(15, 10, 30); // == 0.25f
template<typename t_result, typename t_value_type, typename t_lower_bound_type, typename t_upper_bound_type>
constexpr t_result
NormalizeValue(t_value_type Value, t_lower_bound_type LowerBound, t_upper_bound_type UpperBound)
{
  return UpperBound <= LowerBound ?
         t_result(0) : // Bogus bounds.
         Cast<t_result>(Value - LowerBound) / Cast<t_result>(UpperBound - LowerBound);
}

bool
AreNearlyEqual(double A, double B, double Epsilon = 1e-4);

bool
AreNearlyEqual(float A, float B, float Epsilon = 1e-4f);

inline bool
IsNearlyZero(double A, double Epsilon = 1e-4) { return AreNearlyEqual(A, 0, Epsilon); }

inline bool
IsNearlyZero(float A, float Epsilon = 1e-4f) { return AreNearlyEqual(A, 0, Epsilon); }

template<typename t_a, typename t_b>
inline void
Swap(t_a& A, t_b& B)
{
  auto Temp = A;
  A = B;
  B = Temp;
}

/// Maps the given float Value from [0, 1] to [0, MaxValueOf(UNormType)]
template<typename UNormType>
UNormType constexpr
FloatToUNorm(float Value)
{
  return Cast<UNormType>(Clamp((Value * IntMaxValue<UNormType>()) + 0.5f, 0.0f, IntMaxValue<UNormType>()));
}

/// Maps the given unsigned byte Value from [0, 255] to [0, 1]
template<typename UNormType>
float constexpr
UNormToFloat(UNormType Value)
{
  return Clamp(Cast<float>(Value) / IntMaxValue<UNormType>(), 0.0f, 1.0f);
}

/// \see InitStruct
template<typename T>
struct impl_init_struct
{
  // Return an initialized instance of T.
  template<typename... ArgTypes>
  static constexpr T
  Create(ArgTypes&&... Args) { return { Forward<ArgTypes>(Args)... }; }
};

/// Utility function to initialize a struct of the given type with a chance
/// for centralized specialization.
///
/// To control the default or non-default construction behavior of a certain
/// struct the template \c impl_init_struct can be specialized and a Create()
/// function must be provided.
///
/// \see impl_init_struct
template<typename T, typename... ArgTypes>
inline auto
InitStruct(ArgTypes&&... Args)
  -> decltype(impl_init_struct<rm_ref<T>>::Create(Forward<ArgTypes>(Args)...))
{
  // Note: specializations for impl_init_struct are found in VulkanHelper.inl
  return impl_init_struct<rm_ref<T>>::Create(Forward<ArgTypes>(Args)...);
}

struct impl_defer
{
  template<typename LambdaType>
  struct defer
  {
    LambdaType Lambda;
    defer(LambdaType InLambda) : Lambda{ Move(InLambda) } {}
    ~defer() { Lambda(); }
  };

  template<typename t_in_func_type>
  defer<t_in_func_type> operator =(t_in_func_type InLambda) { return { Move(InLambda) }; }
};

#define PRE_Concat2_Impl(A, B)  A ## B
#define PRE_Concat2(A, B)       PRE_Concat2_Impl(A, B)
#define PRE_Concat3(A, B, C)    PRE_Concat2(PRE_Concat2(A, B), C)
#define PRE_Concat4(A, B, C, D) PRE_Concat2(PRE_Concat2(A, B), PRE_Concat2(C, D))

/// Defers execution of code until the end of the current scope.
///
/// Usage:
///   int i = 0;
///   Defer [&](){ i++; printf("Foo %d\n", i); };
///   Defer [&](){ i++; printf("Bar %d\n", i); };
///   Defer [=](){      printf("Baz %d\n", i); };
///
/// Output:
///   Baz 0
///   Bar 1
///   Foo 2
///
/// \param CaptureSpec The lambda capture specification.
//#define Defer(Lambda) impl_defer<decltype(Lambda)> PRE_Concat2(_Defer, __LINE__){ Lambda }
#define Defer auto PRE_Concat2(_Defer, __LINE__) = impl_defer() =


/// Helper macro to define an opaque handle type.
///
/// Usage:
///   DefineOpaqueHandle(Foo);
///   /*...*/
///   Foo CreateFoo(); // Returns a Foo.
#define DefineOpaqueHandle(Name) using Name = struct Name ## _OPAQUE*

//]]~~
