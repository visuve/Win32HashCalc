#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVer.h>
#include <Windows.h>
#include <bcrypt.h>

#include <cassert>
#include <iostream>
#include <filesystem>
#include <array>
#include <span>
#include <sstream>
#include <string_view>
#include <fstream>