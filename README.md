Game of Life 3D
===============

This is an implementation of Game of Life with 3D view. It calculates the rules of 'Conway's Game of Life' with Compute Shader (DirectCompute) and displays cells with Direct X11 (using Direct3D, Direct2D and DirectWrite). It supports Windows 7 and Vista with DirectX 10/11 GPU.

From version 0.2.0, it supports to read an external DirectX Compute Shader file. You can write your own rules with Compute Shader.

<p>Currently, the following technologies are used:</p>
- C++ 11
- DirectX11 - Direct3D
- DirectCompute (Compute Shader)
- Direct2D
- DirectWrite
- Ribbon Framework
- TSF (Text Services Framework)
- MUI (Multilingual User Interface)

<p>"Game of Life 3D" works on Windows 10/8/7 and Vista (with Platform Update).</p>
[The original project site is here](http://gameoflife3d.codeplex.com/)

# Install

1. Install Visual Studio 2015 Community Edition.
2. Install [Boost 1.59 Windows binaries](http://sourceforge.net/projects/boost/files/boost-binaries/1.59.0/)
   (boost_1_59_0-msvc-14.0-64.exe and boost_1_59_0-msvc-14.0-32.exe).
3. Install [WiX Toolset 3.10](http://wixtoolset.org/).
