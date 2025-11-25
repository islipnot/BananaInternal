#pragma once

struct region_data
{
	BYTE* base = nullptr;
	DWORD size = 0u;
};

namespace sigs
{
	// Get the base and size of a specified module's code section
	region_data get_code_section(PCWSTR module_name);

	// Search a manually provided memory range for a pattern
	address find(const region_data& region, PCSTR pattern);

	// Search GameAssembly.dll for a specific pattern
	address find_in_game_assembly(PCSTR pattern);
}