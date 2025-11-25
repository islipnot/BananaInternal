#include "pch.hpp"
#include "interfaces.hpp"
#include "il2cpp_hlp.hpp"

//
//// GLOBALS
//

namespace g
{
	// Methods

	static void*  (*domain_assembly_open)(void* unused, const char* name) = nullptr;
	static void** (*jinfo_get_method)(void* domain_assembly) = nullptr;
	static void*  (*class_from_name)(const void* method, const char* class_namespace, const char* class_name) = nullptr;
	static void** (*class_get_method_from_name)(const void* _class, const char* method_name, int method_impl) = nullptr;

	// General

	void* default_method = nullptr;
	void* default_class  = nullptr;
}

//
//// API
//

namespace il2cpp_hlp
{
	static bool get_assembly_method(void* buffer, const char* name)
	{
		void* assembly = g::domain_assembly_open(nullptr, name);
		if (!assembly) return false;

		*reinterpret_cast<void**>(buffer) = g::jinfo_get_method(assembly);

		return *reinterpret_cast<void**>(buffer) != nullptr;
	}

	bool get_class(void* buffer, const char* class_namespace, const char* class_name, const char* method_name)
	{
		void* method;

		if (!method_name)
		{
			method = g::default_method;
			if (!method) return false;
		}
		else if (!get_assembly_method(&method, method_name))
		{
			return false;
		}

		*reinterpret_cast<void**>(buffer) = g::class_from_name(method, class_namespace, class_name);

		return *reinterpret_cast<void**>(buffer) != nullptr;
	}

	bool get_method(void* buffer, const char* method_name, int method_impl, const char* class_namespace, const char* class_name, const char* class_method_name)
	{
		void* _class;

		if (!class_name)
		{
			_class = g::default_class;
			if (!_class) return false;
		}
		else if (!get_class(&_class, class_namespace, class_name, class_method_name))
		{
			return false;
		}

		void** pMethod = g::class_get_method_from_name(_class, method_name, method_impl);
		if (!pMethod) return false;

		*reinterpret_cast<void**>(buffer) = *pMethod;

		return true;
	}

	bool hook_method(const char* method_name, const char* class_namespace, const char* class_name, void* detour, void* original)
	{
		void* method_ptr;
		if (!get_method(&method_ptr, method_name, -1, class_namespace, class_name))
		{
			return false;
		}

		return MH_CreateHook(method_ptr, detour, reinterpret_cast<void**>(original)) == MH_OK;
	}

	bool set_method(const char* method_name)
	{
		return get_assembly_method(&g::default_method, method_name);
	}

	bool set_class(const char* class_namespace, const char* class_name, const char* method_name)
	{
		return get_class(&g::default_class, class_namespace, class_name, method_name);
	}

	bool init()
	{
		auto get_export = []<typename t>(t buffer, const char* name)
		{
			*reinterpret_cast<void**>(buffer) = GetProcAddress(sdk::internal::hGameAssembly, name);
			return *buffer != nullptr;
		};

		if (!get_export(&g::domain_assembly_open, "il2cpp_domain_assembly_open"))
			return false;

		if (!get_export(&g::jinfo_get_method, "jinfo_get_method"))
			return false;

		if (!get_export(&g::class_from_name, "il2cpp_class_from_name"))
			return false;

		if (!get_export(&g::class_get_method_from_name, "il2cpp_class_get_method_from_name"))
			return false;

		return true;
	}
}