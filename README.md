MiniLib
=======

Legal notice
------------
Copyright Jonathan Karlsson 2013-2016

* This library is free to use, share, and modify for anyone, as long
  as credit goes where credit is due. In short, have the decency to
  credit the original author (and subsequent authors in case of modification).
* This code is provided "as-is", i.e. the author give no guarantees as to how
  well it will work, if at all, for whatever purpose.
* The author of this code takes no responsibility for any damages
  resulting directly or indirectly from the use of this code.

About
-----
* Minimal template library, MTL: Small replacement for common data types and
  algorithms found in STL.
* Minimal math library, MML: Small template-based linear algebra library.
* Minimal graphics/game library, MGL: Small set of graphics related operations and effects.
* Minimal parallel library, MPL: Includes a library for handling data parallel tasks using
  SIMD (SSE2, NEON).

Interdependence
---------------
* MTL depends on nothing (not even MML, MGL, or MPL).
* MML depends on nothing (not even MTL, MGL, or MPL).
* MPL depends on nothing (not even MTL, MML, or MGL).
* MGL depends on MTL and MML. May depend on MPL in the future.

Compiling
---------
* There should be nothing more than to include these files in
  your current project and compiling with your compiler of choice.
* Tested with current versions of g++, msvc, and clang.
* Performance mileage on MPL may vary as support for compilers and
  processor architectures must be added manually. There is always a
  scalar fallback mode (slower).

NOTES
-----
* This software should be considered in alpha stage.
* This library is still under construction. Current design
  may be subject to change.
* Missing or functionality that does not behave as expected
  is a very real risk.

Future changes
--------------
* MPL still needs re-tooling so heavy changes are to be expected. Design is not final.
