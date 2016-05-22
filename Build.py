import os
import sys
from pathlib import *
import subprocess
from itertools import chain


ThisDir = Path(__file__).parent.resolve()
BuildPath = ThisDir / "Build"
CodePath  = ThisDir / "Code"
TestsPath = ThisDir / "Tests"

VSBaseDir = Path(os.environ["VS140COMNTOOLS"], "..", "..").resolve()

Compiler = VSBaseDir / "VC" / "bin" / "amd64" / "cl.exe"
assert Compiler.exists, "Unable to find x64 msvc compiler ({})".format(Compiler)


CommonCompilerOptions = [
  "/nologo",  # Supress compiler info print.
  "/Z7",      # /Z{7|i|I} Debug information. (7 = in obj, i = in pbd)
  "/FC",      # Use full pathnames in diagnostics
  "/Od",      # Disable optimizations
  "/Oi",      # Generate intrinsic functions
  "/WX",      # Treat warnings as errors
  "/W4",      # /W<n> Set warning level (default n=1)
  "/wd4201",  # /wd<n> Disable warning number n.
  "/wd4100",  # /wd<n> Disable warning number n.
  "/wd4189",  # /wd<n> Disable warning number n.
  "/Gm-",     # /Gm[-] Enable minimal rebuild
  "/GR-",     # /GR[-]   Enable runtime type information
  "/EHa-",    # /EHa[-]  Something about exception handling
  "/TP",      # /TP      Compile all as .cpp
  "/MTd",     # /MD[d] /MT[d] Dynamically or statically linked C run-time library (d = debug)

  "/I{}".format(CodePath), # Add the Code path as include dir.
];

CommonPreprocessorDefines = [
  "/DBB_Platform_Windows",   # For a win32 build.
  "/DBB_Platform_x64",       # For x64 builds.
  "/DBB_Enable_Assert",      # Enable assertions.
  "/DBB_Enable_BoundsCheck", # Enable bounds checking.

  #
  # Windows specific
  #
  "/DNOMINMAX",                # Exclude annoying Windows macros.
  "/DWIN32_LEAN_AND_MEAN",     # Strip some rarely used Windows stuff.
  "/D_CRT_SECURE_NO_WARNINGS", # Shut up windows.h's warnings
];

CommonLinkerOptions = [
  "/DEBUG",
  "/INCREMENTAL:NO",
];

def RunProcess(*args):
  sys.stdout.flush()
  sys.stderr.flush()
  Result = subprocess.run(*args, stdout=sys.stdout, stderr=sys.stderr)
  sys.stdout.flush()
  sys.stderr.flush()
  return Result

class CompilationException(Exception):
  pass

def Compile(Name, AdditionalCompilerOptions, AdditionalLinkerOptions, PrintCommand=False):
  print("Compiling: {}".format(Name))

  Command = [str(Compiler)]

  #
  # Compiler options
  #
  Command.extend(str(Opt) for Opt in CommonCompilerOptions)
  Command.extend(str(Opt) for Opt in CommonPreprocessorDefines)
  Command.extend(str(Opt) for Opt in AdditionalCompilerOptions)

  #
  # Linker options
  #
  Command.append("/link")
  Command.extend(str(Opt) for Opt in CommonLinkerOptions)
  Command.extend(str(Opt) for Opt in AdditionalLinkerOptions)

  #
  # Run the command.
  #

  if PrintCommand:
    print(Command)
  Result = RunProcess(Command)
  if Result.returncode != 0:
    raise CompilationException("Unable to compile {}".format(Name))

#
# Targets
#

def Target_Clean(Args):
  DryRun = "DryRun" in Args and Args["DryRun"] is True
  Patterns = [ "*.obj", "*.pdb", "*.lib", "*.exe", "*.dll", "*.ilk", "*.exp", "*.cpp" ]
  print("In '{}':".format(BuildPath))
  for Pattern in Patterns:
    for File in BuildPath.glob(Pattern):
      print("  Removing: {}".format(File.relative_to(BuildPath)))
      if not DryRun:
        File.unlink()

# Compile and optionally run the tests.
def Target_Tests(Args):
  BuildPath.mkdir(parents=True, exist_ok=True)
  os.chdir(str(BuildPath))

  Name = "BackboneTests"
  ExeName = "{}.exe".format(Name)
  CompilerOptions = [
    "/Fe{}".format(ExeName),
    "/EHsc", # Enable exceptions for Catch
    "/MP8",  # Miltithreaded compilation
  ]
  CompilerOptions.extend(TestsPath.glob("*.cpp"))
  LinkerOptions = [
    "/PDB:{}.pdb".format(Name),
  ]
  try:
    Compile(Name, CompilerOptions, LinkerOptions, PrintCommand=False)
  except: raise
  else:
    if "Run" in Args and Args["Run"] is True:
      RunProcess([ExeName])

TargetDispatchTable = {
  "Clean": Target_Clean,
  "Tests": Target_Tests,
}

ArgValueConversion = {
  "Run" : lambda ArgValue: bool(ArgValue) if len(ArgValue) > 0 else True,
  "DryRun" : lambda ArgValue: bool(ArgValue) if len(ArgValue) > 0 else True,
}

def Main():
  TargetName = None
  Args = {}
  Verbose = False

  for Arg in sys.argv[1:]:
    if Arg.startswith("-"):
      ArgName, _, ArgValue = Arg[1:].partition("=")
      try:
        Args[ArgName] = ArgValueConversion[ArgName](ArgValue)
      except:
        print("Unknown argument: {}".format(Arg), file=sys.stderr)
        raise
    else:
      if TargetName is not None:
        print("More than one target is given, ignoring the previous ones.", file=sys.stderr)
      TargetName = Arg

  if TargetName is None:
    print("No target name given.", file=sys.stderr)
    return

  try:
    Target = TargetDispatchTable[TargetName]
  except:
    print("Unknown target name: {}".format(TargetName), file=sys.stderr)
    raise
  else:
    try:
      Target(Args)
    except:
      if Verbose:
        raise

if __name__ == '__main__':
  Main()
