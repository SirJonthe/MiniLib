MiniLib
-------

Legal notice
------------
Copyright Jonathan Karlsson 2013-2014

* This library is free to use, share, and modify for anyone, as long
  as credit goes where credit is due. In short, have the decency to
  credit the original author (and subsequent authors in case of modification).
* This code is provided "as-is", i.e. the author give no guarantees as to how
  well it will work, if at all, for whatever purpose.
* The author of this code takes no responsibility for any damages
  resulting directly or indirectly from the use of this code.

About
-----
* Includes a small replacement for common data types and
  algorithms found in STL.
* Includes a small template-based linear algebra library.
* Includes a small object-based game engine with a built-in
  software renderer. Hardware accelerated API:s can be included
  in a drop-in fashion. This component requires a platform library
  to work (i.e. platform-independent SDL, or native Win32).
* For users that do not care about game functionality the
  library is designed to work without that component, i.e.
  by removing/omitting the MGL folder from your project
* Barring a few changes the same can be said about the common
  library's independence from the math library and vice versa.
  Currently only MML/mmlFixed.h depends on MTL/mtlString.h.

Compiling
---------
* There should be nothing more than to include these files in
  your current project and compiling with your compiler of choice.
* Tested with current versions of g++, msvc, and clang.

NOTES
-----
* This software should be considered in alpha stage.
* This library is still under construction. Current design
  may be subject to change.
  
