Game of Life 3D
===============
![Screenshot](/screenshots/screenshot_small1.png?raw=true "screenshot")

This is an implementation of Game of Life with 3D view. It calculates the rules of 'Conway's Game of Life' with Compute Shader (DirectCompute) and displays cells with Direct X11 (using Direct3D, Direct2D and DirectWrite). It supports Windows 7 and Vista with DirectX 10/11 GPU.

From version 0.2.0, it supports to read an external DirectX Compute Shader file. You can write your own rules with Compute Shader.

Currently, the following technologies are used:

- C++ 14
- DirectX11 - Direct3D
- DirectCompute (Compute Shader)
- Direct2D
- DirectWrite
- Ribbon Framework
- TSF (Text Services Framework)
- MUI (Multilingual User Interface)

"Game of Life 3D" works on Windows 10/8/7 and Vista (with Platform Update).
[The original project site is here](http://gameoflife3d.codeplex.com/)

# Build

1. Install Visual Studio 2017 Community Edition. (with C++ Desktop Application Development, MFC(for ATL), C++/CLI and Windows SDK(10.0.17763.0))
2. Install [Boost 1.69 Windows binaries](https://sourceforge.net/projects/boost/files/boost-binaries/1.69.0/)
   (boost_1_69_0-msvc-14.1-32.exe and boost_1_69_0-msvc-14.1-64.exe).
3. Install [WiX Toolset 3.11](http://wixtoolset.org/) and [Wix Toolset Visual Studio 2017 Extension](https://marketplace.visualstudio.com/items?itemName=RobMensching.WixToolsetVisualStudio2017Extension).
4. Open "GameOfLife3D.sln".

# License

MIT License

Copyright 2019 Yohei Yoshihara

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
