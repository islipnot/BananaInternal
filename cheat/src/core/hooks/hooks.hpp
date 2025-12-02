#pragma once

namespace hooks
{
	//
	//// HOOK RETURN POINTS
	//

	inline void (*oSendMsg)(il2cpp::Multiplayer_NetworkManager_o* _this, il2cpp::System_String_o* msg, int a3, const il2cpp::MethodInfo* method) = nullptr;

	inline il2cpp::System_String_o* (*oSwearCheck)(il2cpp::Chat_o* _this, il2cpp::System_String_o* message, const il2cpp::MethodInfo* method) = nullptr;

	inline void (*oPlayVoice)(il2cpp::Menu_VoiceLine_o* _this, int a2, const il2cpp::MethodInfo* method) = nullptr;

	inline void (*oDoAttack)(il2cpp::Weapon_Firearms_o* _this, const il2cpp::MethodInfo* method) = nullptr;

	inline void (*oDoReload)(il2cpp::Weapon_Firearms_o* _this, int used_ammo, bool send_to_server, const il2cpp::MethodInfo* method) = nullptr;

	inline void (*oDoJump)(il2cpp::Movement_Movement_o* _this, void* unused, const il2cpp::MethodInfo* method) = nullptr;

	inline void (*oCreateClient)(il2cpp::Multiplayer_Entity_Client_ClientPlayer_o* _this, const il2cpp::MethodInfo* method) = nullptr;

	inline void (*oDestroyClient)(il2cpp::Multiplayer_Entity_Client_ClientPlayer_o* _this, const il2cpp::MethodInfo* method) = nullptr;

	inline void* (*oShakeOnce)(il2cpp::EZCameraShake_CameraShaker_o* _this, float a2, float a3, float a4, float a5, const il2cpp::MethodInfo* method) = nullptr;

	//
	//// FORWARD DECLARATIONS
	//

	bool init();
}