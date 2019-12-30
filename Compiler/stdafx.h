#pragma once
#include <memory>
#include <cstring>
#include <fstream>
#include <iostream>
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
#include <filesystem>

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#   define NOMINMAX // Fixes the conflicts with STL
#   include <Windows.h>
#   include <wincon.h>
#elif __linux__
#   include <unistd.h>
#endif

#include "MurmurHash3.h"
#include "colored_cout.h"
