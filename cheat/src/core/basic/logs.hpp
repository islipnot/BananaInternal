#pragma once

#ifdef DEVELOPER_MODE

template <typename... t> void dbg_msg(std::format_string<t...> fmt, t&&... args)
{
	std::cout << std::format(fmt, std::forward<t>(args)...);
}

#else

#define dbg_msg(...)

#endif

namespace err
{
	enum error_code : uint32_t
	{
		MINHOOK_INIT_FAILED,   // MH_Initialize() failed
		INTERFACE_INIT_FAILED, // interfaces::init() failed
		HOOK_INIT_FAILED,      // hooks::init() failed
	};

	inline void log(uint32_t err_code)
	{
		wchar_t lpText[15];
		wsprintfW(lpText, L"Error code: %lu", err_code);

		MessageBoxW(nullptr, lpText, L"ERROR: bsi.dll", MB_ICONERROR | MB_OK | MB_TOPMOST);
		FreeLibrary(current_module::handle);
	}
}