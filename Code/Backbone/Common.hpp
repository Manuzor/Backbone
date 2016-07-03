#pragma once

// TODO Get rid of this?
#include <type_traits>

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

//
// ================
//

constexpr uint64 KiB(uint64 Amount) { return Amount      * (uint64)1024; }
constexpr uint64 MiB(uint64 Amount) { return KiB(Amount) * (uint64)1024; }
constexpr uint64 GiB(uint64 Amount) { return MiB(Amount) * (uint64)1024; }
constexpr uint64 TiB(uint64 Amount) { return GiB(Amount) * (uint64)1024; }

constexpr uint64 KB(uint64 Amount) { return Amount     * (uint64)1000; }
constexpr uint64 MB(uint64 Amount) { return KB(Amount) * (uint64)1000; }
constexpr uint64 GB(uint64 Amount) { return MB(Amount) * (uint64)1000; }
constexpr uint64 TB(uint64 Amount) { return GB(Amount) * (uint64)1000; }

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

// Used to get the size of a type with support for void where a size of 1 byte is assumed.
template<typename T> struct non_void { enum { Size = sizeof(T) }; };
template<>           struct non_void<void>       : non_void<uint8>       { };
template<>           struct non_void<void const> : non_void<uint8 const> { };

/// Reinterpretation of the given pointer in case t_pointer_type is `void`.
template<typename t_pointer_type>
inline constexpr t_pointer_type*
NonVoidPtr(t_pointer_type* Ptr)
{
  return Ptr;
}

/// Reinterpretation of the given pointer in case t_pointer_type is `void`.
inline constexpr uint8*
NonVoidPtr(void* Ptr)
{
  return reinterpret_cast<uint8*>(Ptr);
}

/// Reinterpretation of the given pointer in case t_pointer_type is `void`.
inline constexpr uint8 const*
NonVoidPtr(void const* Ptr)
{
  return reinterpret_cast<uint8 const*>(Ptr);
}

/// Advance the given pointer value by the given amount of bytes.
template<typename t_pointer_type, typename OffsetType>
inline constexpr t_pointer_type*
MemAddByteOffset(t_pointer_type* Pointer, OffsetType Offset)
{
  return reinterpret_cast<t_pointer_type*>((uint8*)Pointer + Offset);
}

/// Advance the given pointer value by the given amount times sizeof(t_pointer_type)
template<typename t_pointer_type, typename OffsetType>
inline constexpr t_pointer_type*
MemAddOffset(t_pointer_type* Pointer, OffsetType Offset)
{
  return MemAddByteOffset(Pointer, Offset * non_void<t_pointer_type>::Size);
}

template<typename T>
constexpr bool
IsPOD() { return std::is_pod<T>::value; }

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
  return IntIsSigned<T>() ? -(T(1) << (NumBits<T>() - 1))
                          : T(0);
}

template<typename t_type>
struct impl_rm_ref
{
  using Result = t_type;
};

template<typename t_type>
struct impl_rm_ref<t_type&>
{
  using Result = t_type;
};

template<typename t_type>
using rm_ref = typename impl_rm_ref<t_type>::Result;

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

// TODO(Manu): Add int_trait (platform specific because of DWORD etc.)

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
  return (I > 0) ? t_type(1) : (I < 0) ? t_type(-1) : t_type(0);
}

template<typename t_type>
constexpr t_type
Abs(t_type I)
{
  return Sign(I) * I;
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
AreNearlyEqual(double A, double B, double Epsilon);

bool
AreNearlyEqual(float A, float B, float Epsilon);

template<typename t_a, typename t_b>
inline void
Swap(t_a& A, t_b& B)
{
  auto Temp = A;
  A = B;
  B = Temp;
}

template<typename t_func_type>
struct _defer_impl
{
  t_func_type Func;

  template<typename t_in_func_type>
  _defer_impl(t_in_func_type InFunc) : Func{ Move(InFunc) } {};
  ~_defer_impl() { Func(); }
};

#define PRE_Concat2_Impl(A, B)  A ## B
#define PRE_Concat2(A, B)       PRE_Concat2_Impl(A, B)
#define PRE_Concat3(A, B, C)    PRE_Concat2(PRE_Concat2(A, B), C)
#define PRE_Concat4(A, B, C, D) PRE_Concat2(PRE_Concat2(A, B), PRE_Concat2(C, D))

// Usage:
// int i = 0;
// Defer(&, printf("Foo %d\n", i); i++ );
// Defer(&, printf("Bar %d\n", i); i++ );
//
// Output:
// Bar 0
// Foo 1
//
// \param CaptureSpec The lambda capture specification.
#define Defer(CaptureSpec, ...) auto PRE_Concat2(_DeferFunc, __LINE__) = [CaptureSpec](){ __VA_ARGS__; }; \
_defer_impl<decltype(PRE_Concat2(_DeferFunc, __LINE__))> PRE_Concat2(_Defer, __LINE__){ PRE_Concat2(_DeferFunc, __LINE__) }

//]]~~
