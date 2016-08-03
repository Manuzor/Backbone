#include "StringConversion.hpp"

//~~[[

static slice<char const>
TrimWhitespaceFront(slice<char const> String)
{
  while(true)
  {
    if(String.Num == 0)
      return String;

    if(!IsWhitespace(String[0]))
      return String;

    String = SliceTrimFront(String, 1);
  }
}

auto
::ImplConvertStringToDouble( slice<char const>* Source, bool* Success, double Fallback)
  -> double
{
  if(Source == nullptr)
  {
    if(Success)
      *Success = false;

    return Fallback;
  }

  auto String = *Source;
  String = TrimWhitespaceFront(String);

  bool Sign = false;

  if(String.Num == 0)
  {
    if(Success)
      *Success = false;

    return Fallback;
  }

  switch(String[0])
  {
  case '+':
    String = SliceTrimFront(String, 1);
    break;
  case '-':
    Sign = true;
    String = SliceTrimFront(String, 1);
    break;
  default:
    break;
  }

  uint64 NumericalPart = 0;
  bool HasNumericalPart = false;

  while(String.Num > 0 && IsDigit(String[0]))
  {
    NumericalPart *= 10;
    NumericalPart += String[0] - '0';
    HasNumericalPart = true;
    String = SliceTrimFront(String, 1);
  }

  if (!HasNumericalPart)
  {
    if(Success)
      *Success = false;

    return Fallback;
  }

  auto Value = (double)NumericalPart;

  bool HasDecimalPoint = String.Num > 0 && String[0] == '.';

  if(!HasDecimalPoint && String.Num == 0 || (String[0] != '.' && String[0] != 'e' && String[0] != 'E'))
  {
    if(Success)
      *Success = true;

    *Source = String;
    return Sign ? -Value : Value;
  }

  if(HasDecimalPoint)
  {
    String = SliceTrimFront(String, 1);
    uint64 DecimalPart = 0;
    uint64 DecimalDivider = 1;

    while(String.Num > 0 && IsDigit(String[0]))
    {
        DecimalPart *= 10;
        DecimalPart += String[0] - '0';
        DecimalDivider *= 10;
        String = SliceTrimFront(String, 1);
    }

    Value += (double)DecimalPart / (double)DecimalDivider;
  }

  if(String.Num == 0 || (String[0] != 'e' && String[0] != 'E'))
  {
    if(Success)
      *Success = true;

    *Source = String;
    return Sign ? -Value : Value;
  }
  else if(String[0] == 'e' || String[0] == 'E')
  {
    String = SliceTrimFront(String, 1);
    bool ExponentSign = false;

    switch(String[0])
    {
    case '+':
      String = SliceTrimFront(String, 1);
      break;
    case '-':
      ExponentSign = true;
      String = SliceTrimFront(String, 1);
      break;
    default:
      break;
    }

    uint64 ExponentPart = 0;
    while(String.Num > 0 && IsDigit(String[0]))
    {
        ExponentPart *= 10;
        ExponentPart += String[0] - '0';
        String = SliceTrimFront(String, 1);
    }

    int64 ExponentValue = 1;
    while(ExponentPart > 0)
    {
      ExponentValue *= 10;
      --ExponentPart;
    }

    Value = (ExponentSign ? (Value / ExponentValue) : (Value * ExponentValue));
  }

  if(Success)
    *Success = true;

  *Source = String;
  return Sign ? -Value : Value;
}

template<typename IntegerType>
IntegerType
ImplConvertStringToIntegerHelper(slice<char const>* Source, bool* Success, IntegerType Fallback)
{
  if(Source == nullptr)
  {
    if(Success)
      *Success = false;

    return Fallback;
  }

  auto String = *Source;
  String = TrimWhitespaceFront(String);

  bool Sign = false;

  if(String.Num == 0)
  {
    if(Success)
      *Success = false;

    return Fallback;
  }

  switch(String[0])
  {
  case '+':
    String = SliceTrimFront(String, 1);
    break;
  case '-':
    Sign = true;
    String = SliceTrimFront(String, 1);
    break;
  default:
    break;
  }

  uint64 NumericalPart = 0;
  bool HasNumericalPart = false;

  while(String.Num > 0 && IsDigit(String[0]))
  {
    NumericalPart *= 10;
    NumericalPart += String[0] - '0';
    HasNumericalPart = true;
    String = SliceTrimFront(String, 1);
  }

  if (!HasNumericalPart)
  {
    if(Success)
      *Success = false;

    return Fallback;
  }

  auto Value = Convert<IntegerType>(NumericalPart);

  if(Sign)
  {
    if(IntIsSigned<IntegerType>())
    {
      Value = Negate(Value);
    }
    else
    {
      // Unsigned types cannot have a '-' sign.

      if(Success)
        *Success = false;

      return Fallback;
    }
  }

  if(Success)
    *Success = true;

  *Source = String;
  return Value;
}

auto
::ImplConvertStringToInteger(slice<char const>* Source, bool* Success, uint64 Fallback)
  -> uint64
{
  return ImplConvertStringToIntegerHelper<uint64>(Source, Success, Fallback);
}

auto
::ImplConvertStringToInteger(slice<char const>* Source, bool* Success, int64 Fallback)
  -> int64
{
  return ImplConvertStringToIntegerHelper<int64>(Source, Success, Fallback);
}

//]]~~
