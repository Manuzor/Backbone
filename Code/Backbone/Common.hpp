#pragma once

//~~[[

#if !defined(BB_Platform_Windows)
  #error The Backbone is only working on windows platforms for now.
#endif

#define NoOp do{  }while(0)

#define Crash() *(int*)nullptr = 0

#if defined(BB_Enable_Assert)
  #define Assert(Expression) do{ if(!(Expression)) { Crash(); } } while(0)
#else
  #define Assert(Expression) NoOp
#endif

#if defined(BB_Enable_BoundsCheck)
  #define BoundsCheck(Expression) Assert(Expression)
#else
  #define BoundsCheck(Expression) NoOp
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


template<typename t_type, size_t N>
constexpr size_t
ArrayCount(t_type(&)[N]) { return N; }

// Used to get the size of a type with support for void where a size of 1 byte is assumed.
template<typename T> struct non_void             { enum { Size = sizeof(T) }; };
template<>           struct non_void<void>       { enum { Size = 1         }; };
template<>           struct non_void<void const> { enum { Size = 1         }; };

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

template<class t_type> inline
typename rm_ref<t_type>&&
Move(t_type&& Argument)
{
  // forward Argument as movable
  return static_cast<typename rm_ref<t_type>&&>(Argument);
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
  return (I > 0) ? 1 : (I < 0) ? -1 : 0;
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
  return (Value < LowerBound) ? Coerce<t_value_type>(LowerBound) :
         (Value > UpperBound) ? Coerce<t_value_type>(UpperBound) : Value;
}

// TODO(Manu): Make it wrap a LowerBound as well.
template<typename t_value_type, typename t_lower_bound_type, typename t_upper_bound_type>
constexpr t_value_type
Wrap(t_value_type Value, t_lower_bound_type LowerBound, t_upper_bound_type UpperBound)
{
  return Value > Coerce<t_value_type>(UpperBound) ? Value - Coerce<t_value_type>(UpperBound) - 1 :
         Value < Coerce<t_value_type>(LowerBound) ? Value + Coerce<t_value_type>(LowerBound) - 1 :
                                               Value;
}

// Project a value from [LowerBound, UpperBound] to [0, 1]
// Example:
//   auto Result = NormalizeValue<real32>(15, 10, 30); // == 0.25f
template<typename t_result, typename t_value_type, typename t_lower_bound_type, typename t_upper_bound_type>
constexpr t_result
NormalizeValue(t_value_type Value, t_lower_bound_type LowerBound, t_upper_bound_type UpperBound)
{
  return (t_result)(Value - LowerBound) / (t_result)(UpperBound - LowerBound);
}

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
#define Defer(CaptureSpec, ...) auto _DeferFunc##__LINE__ = [CaptureSpec](){ __VA_ARGS__; }; \
_defer_impl<decltype(_DeferFunc##__LINE__)> _Defer##__LINE__{ _DeferFunc##__LINE__ }

//]]~~
