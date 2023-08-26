#pragma once

// ----------------  WINDOWS  ---------------- //
#ifdef _WIN32

#define NOMINMAX

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <tlhelp32.h>
#include <psapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "shlwapi.lib")

#endif
// ----------------  WINDOWS  ---------------- //



// -----------  STANDARD INCLUDES  ----------- //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <errno.h>
#include <thread>
#include <cassert>
#include <concepts>
#include <fstream>
#include <streambuf>
#include <filesystem>
#include <optional>
#include <queue>
#include <set>
#include <limits>
#include <algorithm>
#include <stack>
#include <queue>
#include <semaphore>
#include <unordered_map>
#include <string_view>
#include <stacktrace>
#include <cstdlib>
#include <array>
#include <coroutine>
#include <numeric>
#include <functional>
// -----------  STANDARD INCLUDES  ----------- //



// ------------  SDL, OpenGL, V8 ------------- //
#include <v8.h>
#include <libplatform/libplatform.h>
// ------------  SDL, OpenGL, V8 ------------- //



// ----------  Typedefs & Macros ------------ //
using Path = std::filesystem::path;

#undef ERROR // name colission
// ----------  Typedefs & Macros ------------ //
