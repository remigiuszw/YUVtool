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

.. _pixel_format_representation:

Pixel format representation
===========================

In order to provide a consistent method of describing and processing all the desired raw video formats the following format description has been proposed.

Decoded picture is a two-dimensional matrix of pixels. The two dimensions are called resolution. Pixels in the picture are grouped into rectangular groups called macropixels.

Color representation
--------------------

The full description of light spectrum would be a function :math:`mathbb{R} \to \mathbb{R}` assigning intensity to each light wave lenght. Such representation is both difficult to implement and unnecessary in terms of digital image processing. Human eye recognizes color by intensity, with which light stimulates three kinds of sensors. These sensors detect green, red and blue light. We choose the three intensities represented by real numbers in range :math:`<0; 1>` as canonical representation of color. In digital image prosessing it is common to additionaly keep transparency parameter with each pixel. Together with the three color parameters this constitutes an RGBA colorspace.

For various reasons color spaces different to RGBA have been designed. Such colorspaces are equivalent to RGBA in the sense color represented in those colorspaces can be transformed to RGBA and back. Most commonly such conversion cab be expressed in terms of linear transform:

.. math:: X_i = \sum^3_{j=0} M_{ij} Y_j + N_i

where: :math:`X_i` - intensities in RGBA representation, :math:`Y_i` - intensities in the other representation, :math:`M_{ij}, N_{i}` - transformation parameters.

Quantization
------------

Real numbers have no direct digital representation. Instead they are coded using floating point, fixed point or even tabular codes. In digital image processing, color component intensity number is represented by an integer number of a fixed bit-width :math:`x_i`.

.. math:: x_i = x_{i,min} + round \left( \frac{X_i - X_{i,min}}{X_{i,max} - X_{i,min}} \dot (x_{i,max} - x_{i,min}) \right)

This coding is, of course, lossy, i.e. reverse transform:

.. math:: X_i = X_{i,min} + \frac{x_i - x_{i,min}}{x_{i,max} - x_{i,min}} \dot (X_{i,max} - X_{i,min})

is not guaranteed to give original value.

Data layout
-----------

Each frame can be decomposed into one or more planes. Plane is divided into rows. Row is a sequence of entries. Entry is a group of bits interpreted as a number. There can be more than one row of entries corresponding to one row of macropixels. For each row of entries corresponding to a row of macropixels there is a specification of cyclic structure of widths of entries. The numeration of entries is common for all rows of entries corresponding to one row of macropixels.

.. _entries_in_plane:

.. figure:: images/entries_in_plane.svg

   Layout of entries in a plane. Dashed frame marks group of entries corresponding to single macropixel.

Groups of entries are specified for each plane separately. Each group of entries in a plane corresponds to one macropixel.

Macropixel
----------

Macropixel is a rectangular group of pixels coding of which can be specified in cyclic manner. Macropixel is described by widht, height (in pixels) and width times height pixel coding descriptions. Each pixel coding description consists of three (or four) component coding descriptions. Each component coding description specifies in which plane and in which entry the component intensity number is stored. Multiple component coding descriptions in macropixel might refer to the same intensity number. Not all entries have to be ever referred to (stuffing bits in unpacked formats).

Examples
--------

yuv420p
^^^^^^^

This is common 8-bit planar format. Colorspace is specified as in [BT.601]_ :

.. math::
   x_{i,min} = (16, 16, 16, 0),
   x_{i,max} = (235, 240, 240, 255)

.. math::
   M =
     \left[ \begin{array}{ rrrr }
       0.299                   & 0.587                 & 0.114                 & 0.000 \\
       \frac{0.701}{1.402}     & \frac{-0.587}{1.402}  & \frac{-0.114}{1.402}  & 0.000 \\
       \frac{-0.299}{1.772}    & \frac{-0.587}{1.772}  & \frac{0.886}{1.772}   & 0.000 \\
       0.000                   & 0.000                 & 0.000                 & 1.000
     \end{array} \right]^{-1}

.. math::
   N =
     \left[ \begin{array}{ r }
       0.000 \\
       0.000 \\
       0.000 \\
       0.000
     \end{array} \right]

Each macropixel is a group of 2x2 pixels. There are three planes. Plane 0 is two rows of entries per one row of macropixels. The planes 1 and 2 have one row of entries per macropixel row.

| Entries for plane 0:
| row 0: entry 0: 8-bit, entry 1: 8-bit
| row 1: entry 2: 8-bit, entry 3: 8-bit

| Entries for plane 1:
| row 0: entry 0: 8-bit

| Entries for plane 2:
| row 0: entry 0: 8-bit

| Coding of pixels in macropixel:
| pixel 0:
| Y: plane 0, entry 0
| U: plane 1, entry 0
| V: plane 2, entry 0
| pixel 1:
| Y: plane 0, entry 1
| U: plane 1, entry 0
| V: plane 2, entry 0
| pixel 2:
| Y: plane 0, entry 2
| U: plane 1, entry 0
| V: plane 2, entry 0
| pixel 3:
| Y: plane 0, entry 3
| U: plane 1, entry 0
| V: plane 2, entry 0

uyvy
^^^^

Colorspace is the same as for yuv420p. Macropixel is 2x1 pixels and there is just one plane with one row of entries per row of macropixels. 

| Entries for plane 0:
| row 0: entry 0: 8-bit, entry 1: 8-bit, entry 2: 8-bit, entry 3: 8-bit

| Coding of pixels in macropixel:
| pixel 0:
| Y: plane 0, entry 1
| U: plane 0, entry 0
| V: plane 0, entry 2
| pixel 1:
| Y: plane 0, entry 3
| U: plane 0, entry 0
| V: plane 0, entry 2

Xml companion format
====================

In order to conveniently store parameters necessary to display picture an xml based format has been designed. Thanks to the flexibility offered by xml, different kind of entities can be represented and interconnected in consistent way.

.. ~ \subsection{Sequence.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT sequence picture*>
.. ~ \end{verbatim}

.. ~ \subsection{Picture.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT picture (picture_format, data)>
.. ~ \end{verbatim}

.. ~ \subsection{Picture format.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT picture_format (colorspace, planes, macropixel_coding)>
.. ~ \end{verbatim}

.. ~ \subsection{Colorspace.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT colorspace component+>
.. ~ \end{verbatim}

.. ~ \subsection{Component.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT component coefficient+>
.. ~ \end{verbatim}

.. ~ \subsection{Coefficient.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT coefficient #PCDATA>
.. ~ \end{verbatim}

.. ~ \subsection{Planes.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT planes plane+>
.. ~ \end{verbatim}

.. ~ \subsection{Plane.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT plane entry_row+>
.. ~ \end{verbatim}

.. ~ \subsection{Entry row.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT entry_row entry+>
.. ~ \end{verbatim}

.. ~ \subsection{Entry.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT entry #PCDATA>
.. ~ \end{verbatim}

.. ~ \subsection{Macropixel coding.}
.. ~ \begin{verbatim}
.. ~ <!ELEMENT macropixel_coding #PCDATA>
.. ~ \end{verbatim}

.. ~ Tag: \verb|<>|

.. ~ \subsection{Data}

.. ~ Tag \verb|<data>|

.. ~ \subsection{Example.}

.. ~ File \verb|yuvtool_xml.xsd|:
.. ~ \begin{verbatim}
.. ~ <?xml version="1.0" encoding="UTF-8" ?>
.. ~ <xs:schema
.. ~ xmlns:xs="http://www.w3.org/2001/XMLSchema"
.. ~ targetNamespace="yuvtool_xml"
.. ~ xmlns="yuvtool_xml"
.. ~ elementFormDefault="qualified">

.. ~ <xs:complexType name="picture_sequence_type">
.. ~   <xs:sequence>
.. ~     <xs:element ref="picture" minOccur=1/>
.. ~   </xs:sequence>
.. ~ </xs:complexType>

.. ~ <xs:complexType name="picture_type">
.. ~   <xs:sequence>
.. ~     <xs:element name="picture_paramters" type="picture_parameters_type/>
.. ~     <xs:element name="data" type="xs:string"/>
.. ~   </xs:sequence>
.. ~   <xs:attribute name="repeat" type="xs:positiveInteger">
.. ~ </xs:complexType>

.. ~ <xs:complexType name="picture_parameters_type">
.. ~   <xs:sequence>
.. ~     <xs:element ref="pixel_format"/>
.. ~     <xs:element name="picture_size" type="picture_size_type"/>
.. ~     <xs:element name="stride" type="xs:positiveInteger"/>
.. ~     <xs:element name="offset" type="xs:positiveInteger"/>
.. ~   </xs:sequence>
.. ~ </xs:complexType>

.. ~ <xs:complexType name="picture_format_type">
.. ~   <xs:sequence>
.. ~     <xs:element name="planes" type="plane_type"/>
.. ~     <xs:element name="color_space" type="color_space_type/>
.. ~     <xs:element name="macropixel_coding" type="macropixel_coding_type"/>
.. ~   </xs:sequence>
.. ~ </xs:complexType>

.. ~ <xs:complexType name="planes_type">
.. ~   <xs:sequence>
.. ~     <xs:element name="plane" type="plane_type" maxOccur="4"/>
.. ~   </xs:sequence>
.. ~ </xs:complexType>

.. ~ <xs:complexType name="plane_type">
.. ~   <xs:sequence>
.. ~     <xs:element name="entry_row" type="entry_row_type" maxOccur="unbounded"/>
.. ~   </xs:sequence>
.. ~ </xs:complexType>

.. ~ <xs:complexType name="entry_row_type">
.. ~   <xs:sequence>
.. ~     <xs:element name="entry" type="xs:positiveInteger" maxOccur="unbounded"/>
.. ~   </xs:sequence>
.. ~ </xs:complexType>

.. ~ <xs:complexType name="color_space_type">
.. ~   <xs:sequence>
.. ~     <xs:element name="component" type="component_type" maxOccur="4"/>
.. ~   </xs:sequence>
.. ~ </xs:complexType>

.. ~ <xs:complexType name="component_type">
.. ~   <xs:sequence>
.. ~ 	<xs:element name="coefficient" type="xs:double" maxOccur="4"/>
.. ~   </xs:sequence>
.. ~ </xs:complesType>

.. ~ <xs:complexType name="macropixel_coding_type">
.. ~   <xs:sequence>
.. ~     <xs:element name="coded_pixel" type="coded_pixel_type" maxOccur="unbounded"/>
.. ~   </xs:sequence>
.. ~   <xs:attribute name="size_x" type="xs:positiveInteger"/>
.. ~   <xs:attribute name="size_y" type="xs:positiveInteger"/>
.. ~ </xs:complexType>

.. ~ <xs:complexType name="coded_pixel_type">
.. ~   <xs:sequence>
.. ~ 	<xs:element name="plane_index" type="xs:positiveInteger"/>
.. ~ 	<xs:element name="row_index" type="xs:positiveInteger"/>
.. ~ 	<xs:element name="entry_index" type="xs:positiveInteger"/>
.. ~   </xs:sequence>
.. ~ </xs:complexType>

.. ~ <xs:element name="picture_sequence" type="picture_sequence_type"/>
.. ~ <xs:element name="picture" type="picture_type"/>
.. ~ <xs:element name="picture_paramters" type="picture_parameters_type"/>
.. ~ <xs:element name="pixel_format" type="pixel_format_type"/>
.. ~ <xs:element name="data" type="xs:string"/>

.. ~ </xs:schema> 
.. ~ \end{verbatim}

.. ~ \begin{verbatim}
.. ~ <?xml version="1.0" encoding="UTF-8" ?>
.. ~ <picture_sequence
.. ~ xmlns="yuvtool_xml"
.. ~ xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
.. ~ xsi:schemaLocation="yuvtool_xml yuvtool_xml.xsd">
.. ~ </picture_sequence>
.. ~ \end{verbatim}

Scalability and performance
===========================

Target picture size is 64*1024 x 64*1024 pixels at 16-bit color depth, four color components. There are two performance issues. Operating memory constraints and disk read performance.

The target picture size corresponds to 32 GiB. While it is feasible to store such large data on disk it is inacceptable to store it in operating memory. One might ask, how and for what purpose should we process or display a picture if it cannot be stored in operating memory? After all, the common dispaly size usually does not exceed 1920*1080 pixels at 8-bit color depth. While demand for such pictures is rather foggy, there are two possible cases where such picture can be displayed on commonly available devices. Either only part of the picture is displayed or the picture has to be scaled down. In both cases data volume which has to be held during display operation is at most of the size of the screen.

Minimal volume of the data which has to be read from the disk is the volume of the data which will contribute to what is displayed. Reads should be restricted to this data region. Disk reads are known to be optimal when done in sequential manner. This is not necessary the natural one. E.g. in case of planar formats derivation of RGB values for given pixel demands reads from three different planes. Acquiring data pixel by pixel is inefficient from the disk performance perspective in this case.

In order to conform to the above constraints and to provide optimal solution, observation has been made, that 64-pixel high stripe of picture fits 32 MiB of operating memory even in the worts case scenario. Such memory overhead is way within expectation even for application which works in many instances and coexists with other applications. Operating on 64-pixel high and full picture wide stripes of data provides good compromise between disk access performance and processing seqencing.
