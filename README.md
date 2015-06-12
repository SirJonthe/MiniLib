MiniLib
=======

Legal notice
------------
Copyright Jonathan Karlsson 2013-2015

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
* Minimal graphics/game library, MGL: Small object-based game engine with a
  built-in software renderer. Hardware accelerated API:s can be included in a
  drop-in fashion. This component requires a platform library to work (i.e. platform-
  independent SDL, or native Win32). NOTE: For users that do not care about game
  functionality the library is designed to work without that component, i.e. by
  removing/omitting the MGL folder from your project
* Minimal parallel library, MPL: Includes a library for handling data parallel tasks;
  built-in SIMD (SSE2, NEON) and multi-threading (OpenMP).

Interdependence
---------------
* MTL depends on nothing (not even MML, MGL, or MPL).
* MML depends on nothing (not even MTL, MGL, or MPL).
* MGL depends on nothing (not even MTL, MML, or MGL).
* MGL depends on MTL and MML. May depend on MPL in the future.

Compiling
---------
* There should be nothing more than to include these files in
  your current project and compiling with your compiler of choice.
* Tested with current versions of g++, msvc, and clang.
* Performance mileage on MPL may vary as support for compilers and
  processor architectures must be added manually. There is always a
  scalar fallback mode (slower). In order to take advantage of multi-
  threading you have to enable OpenMP support when compiling (see
  your compiler instructions for specific information on how to do that)

NOTES
-----
* This software should be considered in alpha stage.
* This library is still under construction. Current design
  may be subject to change.
* Missing or functionality that does not behave as expected
  is a very real risk.

Future changes
--------------
* Rather than actually being a game engine, MGL will be refactored
  to become a toolbox for graphics related tasks.
* MPL still needs working in many aspects.
* MAGE2D might one day be refactored into a small general-purpose
  game engine (for 2D *and* 3D) built on top of the new version of
  MGL.
