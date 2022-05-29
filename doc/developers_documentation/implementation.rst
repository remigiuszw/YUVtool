.. _implementation:

Implementation
**************

Programming language
====================

The implementation language is C++. The features used are strictly limited to standard C++11. Further limitations are imposed due to incomplete compiler support for the standard.

The project is expected to compile cleanly on following compilers:

* GCC 4.9 or later,
* clang 3.5 or later

Microsoft Visual Studio does not fully support C++11 and as such is not supported. Gcc is available for Windows, so this poses no threat to cross platform support.

Build system
============

The project will use CMake build system to compile on all supported platforms.

Graphical user interface library
================================

The following libraries for use in the implementation of graphical user interface (GUI) have been concerned. All the following libraries are cross-platform and free software.

gtkmm
-----

The gtkmm library is bindings to GTK library. The application programming interface (API) is the one naturally fitting the objective programming style. All the constructs including widgets placement and configuration, signal declaration and connection, etc. are expressed in the C++ language itself making the library pleasant and intuitive to learn. Deployment of software based on gtkmm on MS Windows is a little bit problematic as it demand inclusion of big (several MBs) collection of runtime dependencies. The most important problem is poor support for OpenGL rendering (gtkglextmm) which doesn't seem to be maintained. The poor OpenGL support has been overcome by use of SFML, which integrates nicely with gtkmm and provides full support for OpenGL.

Qt
--

The widely used toolkit is backed up by strong commercial support. The disqualifying disadvantage is the demand for strong modification of C++ language. The modification results in learning big portion of solution for problems already solved in the language itself. Also the resulting code style of GUI becomes inconsistent with other part of the project.

wxWidgets
---------

The API is intuitive apart from the signaling part which implemented using the ugly MFC style and macros. The library is widely employed in both free and commercial software, but not as widely as QT. The employment ensures continued development and maintenance. The support for OpenGL is satisfactory.

Juce
----

Nice API and OpenGL support, but the resulting interfaces look a little bit unnatural.

FLTK
----
Odd, state machine based API (OpenGL-like) contradicts with e.g. the idea of parallel execution. The signaling part of API incorporates an intriguing idea of passing not handled signals to parent widget. There is a good OpenGL support. The resulting interfaces look a little bit unnatural.

Conclusion
----------

Although the first choice has been made to use wxWidgets library, the choice has been changed in the course of further development thanks to discovery of SFML. In the mean time, gtkmm got native OpenGL support, so dependancy on SFML has been removed.

XML library
===========

Tinyxml2 has been chosen as xml parsing library.

Scalability and performance
===========================

Target picture size is 64*1024 x 64*1024 pixels at 16-bit color depth, four color components. There are two performance issues. Operating memory constraints and disk read performance.

The target picture size corresponds to 32 GiB. While it is feasible to store such large data on disk it is inacceptable to store it in operating memory. One might ask, how and for what purpose should we process or display a picture if it cannot be stored in operating memory? After all, the common dispaly size usually does not exceed 1920*1080 pixels at 8-bit color depth. While demand for such pictures is rather foggy, there are two possible cases where such picture can be displayed on commonly available devices. Either only part of the picture is displayed or the picture has to be scaled down. In both cases data volume which has to be held during display operation is at most of the size of the screen.

Minimal volume of the data which has to be read from the disk is the volume of the data which will contribute to what is displayed. Reads should be restricted to this data region. Disk reads are known to be optimal when done in sequential manner. This is not necessary the natural one. E.g. in case of planar formats derivation of RGB values for given pixel demands reads from three different planes. Acquiring data pixel by pixel is inefficient from the disk performance perspective in this case.

In order to conform to the above constraints and to provide optimal solution, observation has been made, that 64-pixel high stripe of picture fits 32 MiB of operating memory even in the worts case scenario. Such memory overhead is way within expectation even for application which works in many instances and coexists with other applications. Operating on 64-pixel high and full picture wide stripes of data provides good compromise between disk access performance and processing seqencing.
