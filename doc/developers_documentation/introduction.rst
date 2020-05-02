.. _introduction:

Introduction
************

Reviewing the currently available YUV viewer software we realized there is a lack of free (as in freedom), cross-platform YUV viewer application. The need for such application encouraged us to develop one.

Of the currectly available choices [YUVViewer]_ seems to be closest to the requirements of the current project. This one is neither open source nor cross-platform, though. Many of the design decision of this project are based on this appplication.

Desired features
================

We would like to include following features:
  
* support for configurable resolution,
* support for planar and interleaved formats,
* support for configurable bit depth (8-bit, 10-bit, etc.), configured for each plane separately,
* support for configurable plane ordering and meaning (monochrome, YUV, RGB, CMY, etc.),
* fast image rendering, scaling and rotation using hardware support (OpenGL),
* comparison of frames originating from different files,
* recognition of the video format based on the file name,
* display of pixel data in hex format,
* conversion between different formats
* basic YUV editing features: hex edit, frames insertion, deletion, etc.
* encoding the description of YUV format in supplementary per YUV file files,
* project and/or workspace files containing: the list of open files, positions in each file, comparisons

Features will be added gradually, beginning with the core ones. However, a design allowing easy integration of the rest of them, will be maintained throughout the entire development period.
