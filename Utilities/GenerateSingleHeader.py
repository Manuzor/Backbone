from pathlib import Path

OutFilePath = Path.cwd() / "Backbone.hpp"

BeginMarker = "~~[["
EndMarker   = "]]~~"

ThisDir = Path(__file__).parent.resolve()
CodePath = Path(ThisDir.parent, "Code")

NewLine = "\n"


def TransformLines(Lines):
  Lower = 0
  for Line in Lines:
    Lower += 1
    if BeginMarker in Line:
      break
  else:
    Lower = 0

  Upper = len(Lines)
  for Line in reversed(Lines):
    Upper -= 1
    if EndMarker in Line:
      break
  else:
    Upper = len(Lines)

  return NewLine.join(L.rstrip("\r\n") for L in Lines[Lower : Upper])


with OutFilePath.open("w", newline=NewLine) as OutFile:
  def WriteLine(*args, **kwargs):
    print(*args, **kwargs, file=OutFile)

  WriteLine("""
// This file was generated using the tool Utilities/GenerateSingleHeader.py
// from the Backbone project.

#pragma once

#define BB_Inline inline

""")

  FileName = Path("Backbone", "Common.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    WriteLine("//")
    WriteLine("// From Source File: {}".format(FileName.as_posix()))
    WriteLine("//")
    WriteLine(TheLines)

  FileName = Path("Backbone", "Slice.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    WriteLine("//")
    WriteLine("// From Source File: {}".format(FileName.as_posix()))
    WriteLine("//")
    WriteLine(TheLines)

  FileName = Path("Backbone", "Path.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    WriteLine("//")
    WriteLine("// From Source File: {}".format(FileName.as_posix()))
    WriteLine("//")
    WriteLine(TheLines)

  FileName = Path("Backbone", "Path.inl")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    WriteLine("//")
    WriteLine("// From Source File: {}".format(FileName.as_posix()))
    WriteLine("//")
    WriteLine(TheLines)

  FileName = Path("Backbone", "FixedBlock.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    WriteLine("//")
    WriteLine("// From Source File: {}".format(FileName.as_posix()))
    WriteLine("//")
    WriteLine(TheLines)

  FileName = Path("Backbone", "FixedBlock.inl")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    WriteLine("//")
    WriteLine("// From Source File: {}".format(FileName.as_posix()))
    WriteLine("//")
    WriteLine(TheLines)

