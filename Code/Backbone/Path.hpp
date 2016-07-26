#pragma once

#include "Slice.hpp"

//~~[[

enum
{
  Win32_FileSystemPathSeparator = '\\',
  Posix_FileSystemPathSeparator = '/',

  // TODO(Manu): Cross platform.
  Sys_FileSystemPathSeparator = Win32_FileSystemPathSeparator,
};

struct path_options
{
  char Separator = Sys_FileSystemPathSeparator;
  bool32 AppendNull = false;
};

/// Excludes the path separator in Out_Directory.
///
/// \return The index of the separator.
inline size_t
ExtractPathDirectoryAndFileName(slice<char const> Path, slice<char const>* Out_Directory, slice<char const>* Out_Name, path_options Options = {});

inline size_t
ExtractPathDirectoryAndFileName(slice<char> Path, slice<char>* Out_Directory, slice<char>* Out_Name, path_options Options = {});

inline slice<char>
ConcatPaths(slice<char const> Head, slice<char const> Tail, slice<char> Buffer, path_options Options = {});

inline slice<char>
FindFileExtension(slice<char> FileName, path_options Options = {});

inline slice<char const>
FindFileExtension(slice<char const> FileName, path_options Options = {});

//]]~~

#include "Path.inl"
