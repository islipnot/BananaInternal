#pragma once

struct address
{
	std::uint8_t* ptr;

	__forceinline address()
	{
		this->ptr = nullptr;
	}

	template <typename t = std::uint8_t*> __forceinline address(t addr)
	{
		this->ptr = (std::uint8_t*)(addr);
	}

	template <typename t> __forceinline t get()
	{
		return t(this->ptr);
	}

	template <typename t = address> __forceinline t deref()
	{
		return t(*reinterpret_cast<void**>(this->ptr));
	}

	__forceinline address rel32()
	{
		return !this->ptr ? nullptr : address(this->ptr + *reinterpret_cast<int*>(this->ptr) + sizeof(int));
	}

	__forceinline address add(const std::uintptr_t bytes)
	{
		return !this->ptr ? nullptr : address(this->ptr + bytes);
	}

	__forceinline address sub(const std::uintptr_t bytes)
	{
		return !this->ptr ? nullptr : address(this->ptr - bytes);
	}
};