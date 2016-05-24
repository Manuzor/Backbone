#pragma once

#include "Slice.hpp"

//~~[[

// TODO(Manu): Cross platform.
enum { SystemFileSystemPathSeparator = '\\', };

struct path_options
{
  char Separator = SystemFileSystemPathSeparator;
  bool32 AppendNull = false;
};

/// Excludes the path separator in Out_Directory.
///
/// \return The index of the separator.
BB_Inline size_t
ExtractPathDirectoryAndFileName(slice<char const> Path, slice<char const>* Out_Directory, slice<char const>* Out_Name, path_options Options = path_options());

BB_Inline size_t
ExtractPathDirectoryAndFileName(slice<char> Path, slice<char>* Out_Directory, slice<char>* Out_Name, path_options Options = path_options());

BB_Inline slice<char>
ConcatPaths(slice<char const> Head, slice<char const> Tail, slice<char> Buffer, path_options Options = path_options());

//]]~~

#include "Path.inl"
