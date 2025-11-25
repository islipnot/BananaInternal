#pragma once

namespace il2cpp_hlp
{
	bool get_class(void* buffer, const char* class_namespace, const char* class_name, const char* method_name = nullptr);

	bool get_method(void* buffer, const char* method_name, int method_impl = -1, const char* class_namespace = nullptr, const char* class_name = nullptr, const char* class_method_name = nullptr);

	bool hook_method(const char* method_name, const char* class_namespace, const char* class_name, void* detour, void* original);

	bool set_method(const char* method_name);

	bool set_class(const char* class_namespace, const char* class_name, const char* method_name = nullptr);

	bool init();
}