#include "pch.hpp"
#include "cfg.hpp"
#include "sdk/interfaces.hpp"
#include "features.hpp"
#include "core/hooks/hooks.hpp"

namespace features::misc
{
	// [INTERNAL USAGE] Prevents crashes when trying to access a player as it's being created
	void hkCreateClient(il2cpp::Multiplayer_Entity_Client_ClientPlayer_o* _this, const il2cpp::MethodInfo* method)
	{
		const std::lock_guard guard(sdk::internal::player_mutex);

		hooks::oCreateClient(_this, method);
	}

	// [INTERNAL USAGE] Prevents crashes when trying to access a player as it's destroyed
	void hkDestroyClient(il2cpp::Multiplayer_Entity_Client_ClientPlayer_o* _this, const il2cpp::MethodInfo* method)
	{
		const std::lock_guard guard(sdk::internal::player_mutex);

		hooks::oDestroyClient(_this, method);
	}

	// Bypasses chat filters by replacing common letters with identical unicode characters
	void hkSendMsg(il2cpp::Multiplayer_NetworkManager_o* _this, il2cpp::System_String_o* msg, int a3, const il2cpp::MethodInfo* method)
	{
		if (cfg::misc::no_chat_filter)
		{
			auto str = reinterpret_cast<PWSTR>(&msg->fields._firstChar);

			for (int i = 0, sz = msg->fields._stringLength; i < sz; ++i)
			{
				WCHAR& ch = str[i];
				ch = std::tolower(ch);

				switch (ch)
				{
				case L'a': { ch = L'а'; break; }
				case L'c': { ch = L'с'; break; }
				case L'd': { ch = L'ԁ'; break; }
				case L'e': { ch = L'е'; break; }
				case L'h': { ch = L'һ'; break; }
				case L'i': { ch = L'і'; break; }
				case L'j': { ch = L'ј'; break; }
				case L'o': { ch = L'о'; break; }
				case L'p': { ch = L'р'; break; }
				case L'x': { ch = L'х'; break; }
				case L'y': { ch = L'у'; break; }
				}
			}
		}

		hooks::oSendMsg(_this, msg, a3, method);
	}

	// Part of the chat filter bypass, preventing client filtering of unmodified messages
	il2cpp::System_String_o* hkSwearCheck(il2cpp::Chat_o* _this, il2cpp::System_String_o* message, const il2cpp::MethodInfo* method)
	{
		return cfg::misc::no_chat_filter ? message : hooks::oSwearCheck(_this, message, method);
	}

	// Disables the annoying voice. There's a setting for this, but it usually doesn't work
	void hkPlayVoice(il2cpp::Menu_VoiceLine_o* _this, int a2, const il2cpp::MethodInfo* method)
	{
		if (!cfg::misc::block_voice)
		{
			hooks::oPlayVoice(_this, a2, method);
		}
	}

	// Prevents screen shake for any reason, notably when shooting or being shot
	void* hkShakeOnce(il2cpp::EZCameraShake_CameraShaker_o* _this, float a2, float a3, float a4, float a5, const il2cpp::MethodInfo* method)
	{
		return cfg::misc::no_shake ? nullptr : hooks::oShakeOnce(_this, a2, a3, a4, a5, method);
	}
}