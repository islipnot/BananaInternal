#include "pch.hpp"
#include "cfg.hpp"
#include "input.hpp"
#include "core/gui/gui.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT input::hkWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool cursor_modified = false;

	if (wParam == VK_INSERT && msg == WM_KEYUP)
	{
		const bool menu_status = !settings::show_menu;
		settings::show_menu = menu_status;

		if (!menu_status && cursor_modified)
		{
			ShowCursor(false);
			cursor_modified = false;
		}
	}

	if (settings::show_menu)
	{
		CURSORINFO info;
		info.cbSize = sizeof(info);

		GetCursorInfo(&info);

		if (!info.flags)
		{
			ShowCursor(true);
			
			RECT rect;
			GetClientRect(hwnd, &rect);

			POINT screen1 = { rect.left, rect.top };
			ClientToScreen(hwnd, &screen1);

			POINT screen2 = { rect.right, rect.bottom };
			ClientToScreen(hwnd, &screen2);

			rect.left   = screen1.x;
			rect.top    = screen1.y;
			rect.right  = screen2.x;
			rect.bottom = screen2.y;

			ClipCursor(&rect);

			cursor_modified = true;
		}

		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
		return true;
	}

	return input::oWndProc(hwnd, msg, wParam, lParam);
}