from pathlib import Path

OutFilePath = Path.cwd() / "Backbone.hpp"

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


with OutFilePath.open("w", newline=NewLine) as OutFile:
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

""")

  FileName = Path("Backbone", "Common.hpp")
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

  if SeparateInlineFile:
    assert False, "Not implemented."
  else:
    PrintLine("// ====================")
    PrintLine("// === Inline Files ===")
    PrintLine("// ====================")
    PrintLine("")

    FileName = Path("Backbone", "Common.inl")
    with (CodePath / FileName).open("r") as File:
      TheLines = TransformLines(File.readlines())
      PrintIncludeOrigin(FileName)
      PrintLine(TheLines)

    FileName = Path("Backbone", "Angle.inl")
    with (CodePath / FileName).open("r") as File:
      TheLines = TransformLines(File.readlines())
      PrintIncludeOrigin(FileName)
      PrintLine(TheLines)

    FileName = Path("Backbone", "Path.inl")
    with (CodePath / FileName).open("r") as File:
      TheLines = TransformLines(File.readlines())
      PrintIncludeOrigin(FileName)
      PrintLine(TheLines)
