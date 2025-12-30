#include "pch.hpp"

#pragma warning(disable : 6387, justification : "kernelbase.dll!LoadLibraryW will always be present")

static bool inject_dll(const HANDLE hProcess, const std::wstring& dll_path)
{
	// Writing DLL path to process

	const size_t sz = dll_path.size() * sizeof(WCHAR);

	void* const pPath = VirtualAllocEx(hProcess, nullptr, sz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pPath)
	{
		err::log_ex("failed to allocate space for DLL path in target process\n");
		return false;
	}

	if (!WriteProcessMemory(hProcess, pPath, dll_path.data(), sz, nullptr))
	{
		VirtualFreeEx(hProcess, pPath, 0, MEM_RELEASE);
		err::log_ex("failed to write DLL path to target process");
		return false;
	}

	// Remotely calling LoadLibraryW

	const HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(GetModuleHandle(L"kernelbase.dll"), "LoadLibraryW")), pPath, 0, nullptr);
	if (!hThread)
	{
		VirtualFreeEx(hProcess, pPath, 0, MEM_RELEASE);
		err::log_ex("failed to create remote thread");
		return false;
	}

	// Waiting for thread to exit and freeing LoadLibraryW argument

	WaitForSingleObject(hThread, 5000);
	VirtualFreeEx(hProcess, pPath, 0, MEM_RELEASE);

	DWORD code;
	GetExitCodeThread(hThread, &code);
	CloseHandle(hThread);

	if (code == STILL_ACTIVE)
	{
		err::log("injection timed out (likely failed)");
		return false;
	}

	return true;
}

static DWORD get_bs_pid()
{
	const HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE)
	{
		err::log_ex("failed to create process snapshot");
		return 0;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	DWORD pid = 0;

	if (Process32First(snap, &pe32))
	{
		do
		{
			if (!wcscmp(pe32.szExeFile, L"Banana Shooter.exe"))
			{
				pid = pe32.th32ProcessID;
				break;
			}
		} while (Process32Next(snap, &pe32));
	}

	CloseHandle(snap);

	if (!pid)
	{
		err::log("failed to find Banana Shooter.exe, make sure game is launched");
	}

	return pid;
}

int main()
{
	// Getting Banana Shooter.exe.exe PID

	std::cout << "Locating process...\n";

	const DWORD pid = get_bs_pid();
	if (!pid) return 1;

	// Getting DLL path

	std::cout << "Locating bsi.dll...\n";

	const std::wstring dll_path = (std::filesystem::current_path() / "bsi.dll").wstring();
	if (!std::filesystem::exists(dll_path))
	{
		err::log(L"could not find \"{}\"", dll_path);
		return 2;
	}

	// Injecting DLL

	std::cout << "Injecting DLL...\n";

	const HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION, false, pid);
	if (!hProcess)
	{
		err::log_ex("failed to open handle to Banana Shooter.exe.exe");
		return 3;
	}

	if (!inject_dll(hProcess, dll_path))
	{
		return 4;
	}

	CloseHandle(hProcess);

	std::cout << "Injection successful!\n";
	return 0;
}