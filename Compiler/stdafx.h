#pragma once

#ifndef IBC_USE_STD_FILESYSTEM
#   define IBC_USE_STD_FILESYSTEM
#endif // IBC_USE_STD_FILESYSTEM

#include <fstream>
#include <iostream>
#ifdef IBC_USE_STD_FILESYSTEM
#   include <filesystem>
#endif // IBC_USE_STD_FILESYSTEM
#include <cassert>
#include <ostream>
#include <sstream>
#include <vector>
#include <map>
#include <deque>
#include <cctype>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <unordered_map>

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#   define NOMINMAX // Fixes the conflicts with STL
#   include <Windows.h>
#   include <wincon.h>
#endif

#include "MurmurHash3.h"
#include "colored_cout.h"
