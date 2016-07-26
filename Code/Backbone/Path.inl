
//~~[[

auto
::ExtractPathDirectoryAndFileName(slice<char const> Path,
                                  slice<char const>* Out_Directory,
                                  slice<char const>* Out_Name,
                                  path_options Options)
  -> size_t
{
  size_t IndexOfLastSeparator = 0;
  for(size_t Index = 0; Index < Path.Num; ++Index)
  {
    if(Path[Index] == Options.Separator)
      IndexOfLastSeparator = Index;
  }

  if(Out_Directory) *Out_Directory = Slice(Path, 0, IndexOfLastSeparator);

  // Skip the separator itself.
  size_t NameSliceIndex = IndexOfLastSeparator + 1;

  if(Out_Name)
  {
    if(NameSliceIndex < Path.Num) *Out_Name = Slice(Path, NameSliceIndex, Path.Num);
    else                          *Out_Name = Slice(Path, Path.Num, Path.Num); // Produce an empty slice.
  }

  return IndexOfLastSeparator;
}

auto
::ExtractPathDirectoryAndFileName(slice<char> Path,
                                  slice<char>* Out_Directory, slice<char>* Out_Name,
                                  path_options Options)
  -> size_t
{
  return ExtractPathDirectoryAndFileName(AsConst(Path),
                                         Coerce<slice<char const>*>(Out_Directory), Coerce<slice<char const>*>(Out_Name),
                                         Options);
}

auto
::ConcatPaths(slice<char const> Head, slice<char const> Tail, slice<char> Buffer,
              path_options Options)
  -> slice<char>
{
  auto HeadTarget = Slice(Buffer, 0, Head.Num);
  auto TailTarget = Slice(Buffer, Head.Num + 1, Head.Num + 1 + Tail.Num);

  SliceCopy(HeadTarget, Head);
  Buffer[Head.Num] = Options.Separator;
  SliceCopy(TailTarget, Tail);

  auto Result = Slice(Buffer, 0, Head.Num + 1 + Tail.Num);
  if(Options.AppendNull) Buffer[Result.Num] = '\0';
  return Result;
}

template<typename CharType>
struct impl_find_file_extension
{
  static slice<CharType>
  Do(slice<CharType> FileName, path_options Options)
  {
    for(size_t Index = FileName.Num; Index > 0; --Index)
    {
      if(FileName[Index] == '.')
        return Slice(FileName, Index, FileName.Num);

      if(FileName[Index] == Options.Separator)
        break;
    }

    return { 0, OnePastLast(FileName) };
  }
};

auto
::FindFileExtension(slice<char> FileName, path_options Options)
  -> slice<char>
{
  return impl_find_file_extension<char>::Do(FileName, Options);
}

auto
::FindFileExtension(slice<char const> FileName, path_options Options)
  -> slice<char const>
{
  return impl_find_file_extension<char const>::Do(FileName, Options);
}

//]]~~
