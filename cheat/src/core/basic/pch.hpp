#pragma once

#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <d3d11.h>

#include <unordered_map>
#include <iostream>
#include <utility>
#include <format>
#include <string>
#include <thread>
#include <math.h>
#include <mutex>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

#include "minhook/MinHook.h"

#include "resources/fonts.hpp"
#include "core/utils/address.hpp"
#include "core/utils/sigs.hpp"
#include "sdk/il2cpp.h"

struct angle_info_t
{
	float pitch;
	float yaw;
};

namespace current_module
{
	inline HMODULE handle = nullptr;
}

__forceinline consteval uint32_t hash(const wchar_t* input, const uint32_t val = 0x811C9DC5u) noexcept
{
	wchar_t ch = input[0];
	return !ch ? val : hash(input + 1, (val ^ ch) * 0x01000193u);
}

__forceinline constexpr uint32_t hash_rt(const wchar_t* input, int sz) noexcept
{
	uint32_t hash = 0x811C9DC5u;

	for (int i = 0; i < sz; ++i)
	{
		hash = (hash ^ input[i]) * 0x01000193u;
	}

	return hash;
}

static_assert(sizeof(void*) == 8U, "bsi.dll must compile for x64");