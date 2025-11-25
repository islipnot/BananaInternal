#pragma once

namespace gui
{
	// General globals

	inline HWND      hwnd            = nullptr;

	inline glm::vec2 client_area     = {};

	inline glm::vec2 screen_midpoint = {};

	inline float     fov_distance    = 0.0f;

	inline float     fov_radius      = 0.0f;

	inline float     box_scale       = 1.0f;

	inline float     health_scale    = 1.0f;

	// Hooks

	namespace hooks
	{
		// Hook return points

		inline HRESULT(*oPresent)(IDXGISwapChain*, UINT, UINT) = nullptr;

		inline HRESULT(*oResizeBuffers)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT) = nullptr;

		// Hook detours

		HRESULT present(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags);

		HRESULT ResizeBuffers(IDXGISwapChain* _this, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	}
}