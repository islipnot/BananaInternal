#include "pch.hpp"
#include "logs.hpp"
#include "hooks.hpp"
#include "sdk/interfaces.hpp"
#include "features/features.hpp"
#include "core/gui/gui.hpp"
#include "sdk/il2cpp_hlp.hpp"

bool hooks::init()
{ 
	auto create_hook = [](void* detour, void* original, const region_data& region, const char* signature, int offset)
	{
		uint8_t* const addr = sigs::find(region, signature).ptr;
		if (!addr) return false;

		return MH_CreateHook(addr + offset, detour, reinterpret_cast<void**>(original)) == MH_OK;
	};
	
	// gameoverlayrenderer64.dll

	{
		// This module hooks a bunch of IDXGISwapChain virtual methods, so im just hooking it's hooks (steam does this for every game)

		const region_data gameoverlayrenderer = sigs::get_code_section(L"gameoverlayrenderer64.dll");
		if (!gameoverlayrenderer.base)
			return false;

		if (!create_hook(gui::hooks::present, &gui::hooks::oPresent, gameoverlayrenderer, "41 8B E8 8B F2", -22))
			return false;

		if (!create_hook(gui::hooks::ResizeBuffers, &gui::hooks::oResizeBuffers, gameoverlayrenderer, "48 8B F0 E8 ? ? ? ? 48 8B D5", -67))
			return false;
	}

	// GameAssembly.dll

	{
		il2cpp_hlp::set_method("Assembly-CSharp.dll");

		if (!il2cpp_hlp::hook_method("DoAttack", "Weapon", "Firearms",                         features::exploits::hkDoAttack,  &hooks::oDoAttack))
			return false;

		if (!il2cpp_hlp::hook_method("SwearCheck", "\0", "Chat",                               features::misc::hkSwearCheck,    &hooks::oSwearCheck))
			return false;

		if (!il2cpp_hlp::hook_method("PlayVoice", "Menu", "VoiceLine",                         features::misc::hkPlayVoice,     &hooks::oPlayVoice))
			return false;

		if (!il2cpp_hlp::hook_method("PLCMHLDAMPK", "Movement", "Movement",                    features::exploits::hkDoJump,    &hooks::oDoJump))
			return false;

		if (!il2cpp_hlp::hook_method(".ctor", "Multiplayer.Entity.Client", "ClientPlayer",     features::misc::hkCreateClient, &hooks::oCreateClient))
			return false;

		if (!il2cpp_hlp::hook_method("OnDestroy", "Multiplayer.Entity.Client", "ClientPlayer", features::misc::hkDestroyClient, &hooks::oDestroyClient))
			return false;

		if (!il2cpp_hlp::hook_method("ShakeOnce", "EZCameraShake", "CameraShaker",             features::misc::hkShakeOnce,     &hooks::oShakeOnce))
			return false;

		if (!il2cpp_hlp::hook_method("SendMsg", "Multiplayer", "NetworkManager",               features::misc::hkSendMsg,       &hooks::oSendMsg))
			return false;

		if (!il2cpp_hlp::hook_method("DoReload", "Weapon", "Firearms",                         features::exploits::hkDoReload,  &hooks::oDoReload))
			return false;
	}

	// Patches

	{
		// GC will hit an int3 when rendering skeleton esp sometimes due to garbage collection for an unrecognized thread in Object.GetName calls.
		// Patching this has no effect on the game in any way.

		const address thread_check = sigs::find_in_game_assembly("0F 8C ? ? ? ? 83 3D").add(0x25);
		if (!thread_check.ptr) return false;

		DWORD old;
		VirtualProtect(thread_check.ptr, 0x1C, PAGE_EXECUTE_READWRITE, &old);
		memset(thread_check.ptr, 0x90, 0x1C);
		VirtualProtect(thread_check.ptr, 0x1C, old, &old);
	}

	return MH_EnableHook(MH_ALL_HOOKS) == MH_OK;
}