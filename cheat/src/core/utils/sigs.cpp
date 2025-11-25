#include "pch.hpp"
#include "sigs.hpp"
#include "sdk/interfaces.hpp"

namespace sigs
{
	// Get the base and size of a specified module's code section

	region_data get_code_section(PCWSTR module_name)
	{
		const auto base = reinterpret_cast<BYTE*>(GetModuleHandleW(module_name));
		if (!base) return {};

		const auto nt_headers = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
		const DWORD size_of_code = nt_headers->OptionalHeader.SizeOfCode;

		const IMAGE_SECTION_HEADER* sh = IMAGE_FIRST_SECTION(nt_headers);

		for (WORD i = 0, sz = nt_headers->FileHeader.NumberOfSections; i < sz; ++i, ++sh)
		{
			if (sh->SizeOfRawData == size_of_code && sh->Characteristics & IMAGE_SCN_MEM_EXECUTE)
			{
				return region_data{ base + sh->VirtualAddress, size_of_code };
			}
		}

		return {};
	}

	// Search a manually provided memory range for a pattern

	address find(const region_data& region, PCSTR pattern)
	{
		const int pattern_sz = static_cast<int>(std::count(pattern, pattern + strlen(pattern), ' ') + 1);

		for (BYTE* base = region.base, *end = (base + region.size) - pattern_sz; base < end; ++base)
		{
			bool found = true;

			for (int i = 0, pattern_index = 0; i < pattern_sz; ++i, pattern_index += 2)
			{
				if (pattern[pattern_index] == '?')
					continue;

				if (base[i] != std::stoi(&pattern[pattern_index], nullptr, 16))
				{
					found = false;
					break;
				}

				++pattern_index;
			}

			if (found) return base;
		}

		return nullptr;
	}

	// Search GameAssembly.dll for a specific pattern

	address find_in_game_assembly(PCSTR pattern)
	{
		const auto base = reinterpret_cast<BYTE*>(sdk::internal::hGameAssembly);
		const auto nt_headers = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
		const IMAGE_SECTION_HEADER* sh = IMAGE_FIRST_SECTION(nt_headers);

		for (WORD i = 0, sz = nt_headers->FileHeader.NumberOfSections; i < sz; ++i, ++sh)
		{
			if (!strcmp(reinterpret_cast<const char*>(sh->Name), ".text"))
			{
				return find({ base + sh->VirtualAddress, sh->SizeOfRawData }, pattern);
			}
		}

		return nullptr;
	}
}