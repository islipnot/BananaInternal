#include "pch.hpp"
#include "logs.hpp"
#include "core/gui/gui.hpp"
#include "core/input/input.hpp"
#include "core/hooks/hooks.hpp"
#include "sdk/interfaces.hpp"

static void thread_main()
{
#ifdef DEVELOPER_MODE
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

    if (MH_Initialize() != MH_OK)
    {
        err::log(err::MINHOOK_INIT_FAILED);
        return;
    }

    if (!sdk::init())
    {
        err::log(err::INTERFACE_INIT_FAILED);
        return;
    }

    if (!hooks::init())
    {
        err::log(err::HOOK_INIT_FAILED);
        return;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        current_module::handle = hModule;
        std::thread(thread_main).detach();
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
        // Unhooking WndProc

        if (input::oWndProc && gui::hwnd)
        {
            SetWindowLongPtrW(gui::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(input::oWndProc));
        }

        // Uninitializing

        MH_RemoveHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    return TRUE;
}