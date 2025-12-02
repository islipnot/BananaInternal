#include "pch.hpp"
#include "gui.hpp"
#include "cfg.hpp"
#include "core/input/input.hpp"
#include "sdk/interfaces.hpp"
#include "features/features.hpp"

//
//// GLOBALS
//

namespace g
{
	static ID3D11Device*           device        = nullptr;
	static ID3D11DeviceContext*    context       = nullptr;
	static ID3D11RenderTargetView* render_target = nullptr;
}

//
//// HELPERS
//

namespace ImGui
{
	static constexpr ImVec4 DarkGrey     { 0.060196f,  0.060196f,  0.060196f,  1.0f  };
	static constexpr ImVec4 DarkPurple   { 0.2156862f, 0.0117647f, 0.3686274f, 0.75f };
	static constexpr ImVec4 LightPurple  { 0.3156862f, 0.0217647f, 0.4686274f, 0.75f };
	static constexpr ImVec4 LightPurple2 { 0.8156862f, 0.5217647f, 0.9686274f, 0.75f };

	static void Description(const char* desc)
	{
		ImGui::SameLine();
		ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 0.50f }, desc);
	}

	static bool CheckboxEx(const char* label, bool* var)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   { 4.0f, 6.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  { 3.0f, 3.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,   1.5f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

		const bool v_status = *var;

		if (v_status)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg,        DarkPurple);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, LightPurple);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  LightPurple);
		}
		else 
		{
			ImGui::PushStyleColor(ImGuiCol_Border, { 0.1562745f, 0.1562745f, 0.1562745f, 1.0f }); // Light grey
		}

		const bool status = ImGui::Checkbox(label, var);

		ImGui::PopStyleColor(v_status ? 3 : 1);
		ImGui::PopStyleVar(4);

		return status;
	}

	static bool SliderFloatEx(const std::string& label, float* var, const float min, const float max)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { settings::imgui_style->FramePadding.x, 4.0f });
		ImGui::SetNextItemWidth(250.0f);

		const bool status = ImGui::SliderFloat(("##" + label).c_str(), var, min, max);
		ImGui::PopStyleVar();

		ImGui::Description(label.c_str());

		return status;
	}

	static void ComboEx(const std::string label, int* selection, const char* items[], int item_count)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { settings::imgui_style->FramePadding.x, 4.0f });
		ImGui::PushItemWidth(250.0f);

		if (ImGui::BeginCombo(("##" + label).c_str(), items[*selection]))
		{
			for (int i = 0; i < item_count; ++i)
			{
				const bool selected = *selection == i;

				if (selected)
				{
					constexpr ImVec4 green      = { 0.058824f, 0.309804f, 0.105882f, 1.0f };
					constexpr ImVec4 LightGreen = { 0.068824f, 0.409804f, 0.205882f, 1.0f };

					ImGui::PushStyleColor(ImGuiCol_Header,        green);
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, LightGreen);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive,  LightGreen);
				}

				if (ImGui::Selectable(items[i], selected) && *selection != i)
				{
					*selection = i;
				}

				if (selected)
				{
					ImGui::PopStyleColor(3);
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		
		ImGui::Description(label.c_str());
	}

	static void ToolTip(const char* text)
	{
		ImGui::SameLine();
		ImGui::TextColored(LightPurple2, "?");
		if (ImGui::IsItemHovered()) ImGui::SetTooltip(text);
	}
}

//
//// INITIALIZATION
//

#pragma warning(push)
#pragma warning(disable : 6387)

static void create_render_target(IDXGISwapChain* swap_chain)
{
	// Creating render target

	swap_chain->GetDevice(IID_PPV_ARGS(&g::device));
	g::device->GetImmediateContext(&g::context);

	ID3D11Texture2D* back_buffer = nullptr;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	g::device->CreateRenderTargetView(back_buffer, nullptr, &g::render_target);

	// Caching screen data

	D3D11_TEXTURE2D_DESC desc;
	back_buffer->GetDesc(&desc);
	back_buffer->Release();

	gui::client_area.x = static_cast<float>(desc.Width);
	gui::client_area.y = static_cast<float>(desc.Height);

	gui::screen_midpoint.x = static_cast<float>(desc.Width / 2);
	gui::screen_midpoint.y = static_cast<float>(desc.Height / 2);

	gui::fov_radius   = gui::screen_midpoint.x / (180.0f / cfg::aimbot::fov);
	gui::fov_distance = cfg::aimbot::fov * (gui::screen_midpoint.x / 180.0f);
}

static void init_gui(IDXGISwapChain* swap_chain)
{
	// Getting D3D11 device/context and creating render target

	create_render_target(swap_chain);

	// Creating ImGui context

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Enabling DPI awareness

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	SetProcessDPIAware();

	// Configuring ImGui style

	constexpr ImVec4 clear          = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr ImVec4 black          = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr ImVec4 grey           = { 0.0901960f, 0.0901960f, 0.0901960f, 1.0f };
	constexpr ImVec4 DarkGreyC      = { 0.0601960f, 0.0601960f, 0.0601960f, 1.0f };
	constexpr ImVec4 LightGrey      = { 0.1562745f, 0.1562745f, 0.1562745f, 1.0f };
	constexpr ImVec4 DarkGrey_Child = { 0.0501960f, 0.0501960f, 0.0501960f, 1.0f };
	constexpr ImVec4 DarkPurpleC    = { 0.2156862f, 0.0117647f, 0.3686274f, 0.75f };
	constexpr ImVec4 LightPurpleC   = { 0.3156862f, 0.0217647f, 0.4686274f, 0.75f };
	constexpr ImVec4 LightPurple2   = { 0.4156862f, 0.0317647f, 0.5686274f, 0.75f };

	settings::imgui_style = &ImGui::GetStyle();
	{
		settings::imgui_style->TabRounding     = 0.0f;
		settings::imgui_style->FrameBorderSize = 1.0f;
		settings::imgui_style->FramePadding    = { 7.0f, 7.0f };
		settings::imgui_style->Colors[ImGuiCol_ResizeGrip]        = clear;
		settings::imgui_style->Colors[ImGuiCol_ResizeGripActive]  = clear;
		settings::imgui_style->Colors[ImGuiCol_ResizeGripHovered] = clear;
		settings::imgui_style->Colors[ImGuiCol_TitleBgActive]     = black;
		settings::imgui_style->Colors[ImGuiCol_TitleBg]           = black;
		settings::imgui_style->Colors[ImGuiCol_WindowBg]          = grey;
		settings::imgui_style->Colors[ImGuiCol_Button]            = DarkGreyC;
		settings::imgui_style->Colors[ImGuiCol_ButtonActive]      = LightGrey;
		settings::imgui_style->Colors[ImGuiCol_ButtonHovered]     = LightGrey;
		settings::imgui_style->Colors[ImGuiCol_PopupBg]           = DarkGreyC;
		settings::imgui_style->Colors[ImGuiCol_Header]            = DarkGreyC;
		settings::imgui_style->Colors[ImGuiCol_HeaderActive]      = LightGrey;
		settings::imgui_style->Colors[ImGuiCol_HeaderHovered]     = LightGrey;
		settings::imgui_style->Colors[ImGuiCol_FrameBg]           = DarkGreyC;
		settings::imgui_style->Colors[ImGuiCol_FrameBgHovered]    = LightGrey;
		settings::imgui_style->Colors[ImGuiCol_FrameBgActive]     = LightGrey;
		settings::imgui_style->Colors[ImGuiCol_ChildBg]           = DarkGrey_Child;
		settings::imgui_style->Colors[ImGuiCol_CheckMark]         = black;
		settings::imgui_style->Colors[ImGuiCol_Tab]               = DarkGreyC;
		settings::imgui_style->Colors[ImGuiCol_TabActive]         = LightPurpleC;
		settings::imgui_style->Colors[ImGuiCol_TabHovered]        = LightPurple2;
		settings::imgui_style->Colors[ImGuiCol_SliderGrab]        = DarkPurpleC;
		settings::imgui_style->Colors[ImGuiCol_SliderGrabActive]  = LightPurpleC;
		settings::imgui_style->Colors[ImGuiCol_Separator]         = DarkGreyC;
	}

	settings::imgui_io = &ImGui::GetIO();
	settings::imgui_io->ConfigWindowsResizeFromEdges = false;
	settings::imgui_io->Fonts->AddFontFromMemoryCompressedTTF(font_resources::aileron_compressed_data, font_resources::aileron_compressed_size, 16.0f);

	// Hooking WndProc

	DXGI_SWAP_CHAIN_DESC desc;
	swap_chain->GetDesc(&desc);
	gui::hwnd = desc.OutputWindow;

	input::oWndProc = reinterpret_cast<decltype(input::oWndProc)>(SetWindowLongPtrW(gui::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(input::hkWndProc)));

	// Initializing backends

	ImGui_ImplWin32_Init(gui::hwnd);
	ImGui_ImplDX11_Init(g::device, g::context);
}

#pragma warning(pop)

//
//// RENDERING
//

static __forceinline bool esp_enabled()
{
	return cfg::visuals::boxes || cfg::visuals::health || cfg::visuals::skeleton || cfg::visuals::glow || cfg::aimbot::snaplines;
}

static __forceinline void setup_menu()
{
	ImGui::SetNextWindowSize({ 600.0f, 450.0f }, ImGuiCond_FirstUseEver);
	ImGui::PushStyleColor(ImGuiCol_Border, { 0.2156862f, 0.0117647f, 0.3686274f, 1.0f }); // Dark purple
	ImGui::Begin("RanchoHook", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
	ImGui::PopStyleColor();
}

static __forceinline void setup_frame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

static __forceinline void end_frame()
{
	ImGui::Render();
	g::context->OMSetRenderTargets(1, &g::render_target, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

HRESULT gui::hooks::present(IDXGISwapChain* _this, UINT SyncInterval, UINT Flags)
{
	static bool initialized = false;

	if (!initialized)
	{
		init_gui(_this);
		initialized = true;
	}

	const bool in_game = sdk::main::game_mgr->is_in_game();

	if (cfg::aimbot::enabled && !cfg::aimbot::silent_aim && in_game)
	{
		features::aimbot::run();
	}

	const bool render_esp     = in_game && esp_enabled();
	const bool render_visuals = render_esp || cfg::aimbot::show_fov || cfg::aimbot::snaplines;
	const bool render_frame   = settings::show_menu || render_visuals;

	if (render_frame)
	{
		setup_frame();
	}

	if (settings::show_menu)
	{
		setup_menu();
		
		if (ImGui::BeginTabBar("tabs"))
		{
			if (ImGui::BeginTabItem("Visuals"))
			{
				ImGui::CheckboxEx("Skeleton ESP", &cfg::visuals::skeleton);

				ImGui::CheckboxEx("Box ESP",      &cfg::visuals::boxes);

				ImGui::CheckboxEx("Health ESP",   &cfg::visuals::health);

				ImGui::CheckboxEx("Glow",         &cfg::visuals::glow);

				ImGui::CheckboxEx("Show team",    &cfg::visuals::show_team);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Aimbot"))
			{
				ImGui::CheckboxEx("Aimbot",         &cfg::aimbot::enabled);

				ImGui::CheckboxEx("Silent aim",     &cfg::aimbot::silent_aim);

				ImGui::CheckboxEx("Vis checks",     &cfg::aimbot::vis_check);

				ImGui::CheckboxEx("360 FOV",        &cfg::aimbot::ignore_fov);

				ImGui::CheckboxEx("Show FOV",       &cfg::aimbot::show_fov);

				ImGui::CheckboxEx("Show snaplines", &cfg::aimbot::snaplines);
				
				if (ImGui::SliderFloatEx("FOV",     &cfg::aimbot::fov,       1.0f, 180.0f))
				{
					gui::fov_radius   = gui::screen_midpoint.x / (180.0f / cfg::aimbot::fov);
					gui::fov_distance = cfg::aimbot::fov * (gui::screen_midpoint.x / 180.0f);
				}

				ImGui::ComboEx("Targeting mode",    &cfg::aimbot::aim_mode, cfg::aimbot::targeting_modes, _countof(cfg::aimbot::targeting_modes));

				ImGui::ComboEx("Preferred bone",    &cfg::aimbot::aim_bone, cfg::aimbot::bone_names,      _countof(cfg::aimbot::bone_names));

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Exploits"))
			{
				ImGui::CheckboxEx("Auto shoot",    &cfg::exploits::auto_shoot);

				ImGui::CheckboxEx("Quick reload",  &cfg::exploits::quick_reload);

				ImGui::CheckboxEx("Magic bullet",  &cfg::exploits::magic_bullet);
				ImGui::ToolTip("Teleports bullets directly to players (works with melee)");

				ImGui::CheckboxEx("No recoil",     &cfg::exploits::no_recoil);

				ImGui::CheckboxEx("No spread",     &cfg::exploits::no_spread);

				ImGui::CheckboxEx("No-clip",       &cfg::exploits::noclip);

				ImGui::CheckboxEx("Infinite jump", &cfg::exploits::inf_jump);

				ImGui::SliderFloatEx("Jump force", &cfg::exploits::jump_mult, 1.0f, 5.0f);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc"))
			{
				ImGui::CheckboxEx("No camera shake",     &cfg::misc::no_shake);

				ImGui::CheckboxEx("Disable voice lines", &cfg::misc::block_voice);
				ImGui::ToolTip("The in-game setting to disable these horrible voice lines is broken...");

				ImGui::CheckboxEx("Bypass chat filters", &cfg::misc::no_chat_filter);
				ImGui::ToolTip("Due to how this works, messages are converted to lowercase");

				if (ImGui::SliderFloatEx("Aspect ratio", &cfg::misc::aspect_ratio, 0.5f, 5.0f))
				{
					sdk::fn::set_aspect_ratio(sdk::main::main_cam->cam, cfg::misc::aspect_ratio, nullptr);
					
					gui::box_scale = 0.954203f / cfg::misc::aspect_ratio;
					gui::health_scale = 1.10f / cfg::misc::aspect_ratio;
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		
		ImGui::End();
	}

	if (in_game)
	{
		features::exploits::run();
	}

	if (render_visuals)
	{
		features::visuals::run(render_esp);
	}

	if (render_frame)
	{
		end_frame();
	}

	return gui::hooks::oPresent(_this, SyncInterval, Flags);
}

HRESULT gui::hooks::ResizeBuffers(IDXGISwapChain* _this, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	if (g::render_target)
	{
		g::render_target->Release();
	}

	const HRESULT status = gui::hooks::oResizeBuffers(_this, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	if (!g::render_target)
	{
		return status;
	}

	ImGui_ImplDX11_InvalidateDeviceObjects();

	if (SUCCEEDED(status))
	{
		create_render_target(_this);
	}

	return status;
}