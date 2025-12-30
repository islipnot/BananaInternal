#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winternl.h>
#include <TlHelp32.h>

#include <iostream>
#include <fstream>
#include <filesystem>

namespace err
{
	template <typename... t> void log(std::format_string<t...> fmt, t&&... args)
	{
		std::cerr << "ERROR: " << std::format(fmt, std::forward<t>(args)...) << '\n';
	}

	template <typename... t> void log(std::wformat_string<t...> fmt, t&&... args)
	{
		std::wcerr << L"ERROR: " << std::format(fmt, std::forward<t>(args)...) << L'\n';
	}

	template <typename... t> void log_ex(std::format_string<t...> fmt, t&&... args)
	{
		std::cerr << "ERROR: " << std::format(fmt, std::forward<t>(args)...) << "\nError code: 0x" << std::hex << GetLastError() << std::dec << '\n';
	}
}

static_assert(sizeof(void*) == 8U, "Loader must compile for x64");