#pragma once

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
//#if defined(DEBUG) || defined(_DEBUG)
//#pragma  comment(lib, "d3dx11d.lib")
//#else
//#pragma  comment(lib, "d3dx11.lib")
//#endif
//#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "d3d10_1.lib")
//#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "Propsys.lib")
#pragma comment(lib, "Uxtheme.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "xpsprint.lib")
#pragma comment(lib, "Prntvpt.lib")

#ifdef _M_X64

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/GameOfLife3DLib.lib")
#pragma comment(lib, "../x64/Debug/GameOfLife3D_colorsetdef.lib")
#else
#pragma comment(lib, "../x64/Release/GameOfLife3DLib.lib")
#pragma comment(lib, "../x64/Release/GameOfLife3D_colorsetdef.lib")
#endif

#else

#ifdef _DEBUG
#pragma comment(lib, "../Debug/GameOfLife3DLib.lib")
#pragma comment(lib, "../Debug/GameOfLife3D_colorsetdef.lib")
#else
#pragma comment(lib, "../Release/GameOfLife3DLib.lib")
#pragma comment(lib, "../Release/GameOfLife3D_colorsetdef.lib")
#endif

#endif
