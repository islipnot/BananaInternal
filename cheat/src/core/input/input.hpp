#pragma once

namespace input
{
	inline LRESULT(*oWndProc)(HWND, UINT, WPARAM, LPARAM) = nullptr;

	LRESULT hkWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}