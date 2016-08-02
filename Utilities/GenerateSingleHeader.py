from pathlib import Path
import argparse

Parser = argparse.ArgumentParser(description="Pack all Backbone sources into as few source files as possible to make distribution easier.")
Parser.add_argument("outpath",
                    type=Path,
                    default=Path.cwd(),
                    nargs="?",
                    help="")

Args = Parser.parse_args()

OutBasePath = Args.outpath
OutHeaderFilePath = OutBasePath / "Backbone.hpp"
OutTranslationUnitFilePath = OutBasePath / "Backbone.cpp"

BeginMarker = "~~[["
EndMarker   = "]]~~"

ThisDir = Path(__file__).parent.resolve()
CodePath = Path(ThisDir.parent, "Code")

NewLine = "\n"

SeparateInlineFile = False


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


with OutHeaderFilePath.open("w", newline=NewLine) as OutFile:
  def PrintLine(*args, **kwargs):
    print(*args, **kwargs, file=OutFile)

  def PrintIncludeOrigin(FileName):
    FileNameStr = FileName.as_posix()
    PrintLine("// ============{}====".format("=" * len(FileNameStr)))
    PrintLine("// === Source: {} ===".format(FileNameStr))
    PrintLine("// ============{}====".format("=" * len(FileNameStr)))

  PrintLine("""
// This file was generated using the tool Utilities/GenerateSingleHeader.py
// from the Backbone project.

#pragma once

// For placement-new.
#include <new>

// For std::numeric_limits
#include <limits>

""")

  FileName = Path("Backbone", "Common.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "Memory.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "Slice.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "Angle.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "Path.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "FixedBlock.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "StringConversion.hpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  if SeparateInlineFile:
    assert False, "Not implemented."
  else:
    PrintLine("// ====================")
    PrintLine("// === Inline Files ===")
    PrintLine("// ====================")
    PrintLine("")

    FileName = Path("Backbone", "Path.inl")
    with (CodePath / FileName).open("r") as File:
      TheLines = TransformLines(File.readlines())
      PrintIncludeOrigin(FileName)
      PrintLine(TheLines)

with OutTranslationUnitFilePath.open("w", newline=NewLine) as OutFile:
  def PrintLine(*args, **kwargs):
    print(*args, **kwargs, file=OutFile)

  def PrintIncludeOrigin(FileName):
    FileNameStr = FileName.as_posix()
    PrintLine("// ============{}====".format("=" * len(FileNameStr)))
    PrintLine("// === Source: {} ===".format(FileNameStr))
    PrintLine("// ============{}====".format("=" * len(FileNameStr)))

  PrintLine("""
// This file was generated using the tool Utilities/GenerateSingleHeader.py
// from the Backbone project.

#include "Backbone.hpp"

#include <cmath>
#include <cstring>

""")

  FileName = Path("Backbone", "Common.cpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "Slice.cpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "Memory.cpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "Angle.cpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)

  FileName = Path("Backbone", "StringConversion.cpp")
  with (CodePath / FileName).open("r") as File:
    TheLines = TransformLines(File.readlines())
    PrintIncludeOrigin(FileName)
    PrintLine(TheLines)
