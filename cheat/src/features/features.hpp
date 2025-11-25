#pragma once

namespace features
{
	namespace visuals
	{
		void run(bool render_esp);
	}

	namespace aimbot
	{
		void run();

		sdk::ClientFields_t* get_target(angle_info_t* angle_buffer, glm::vec3* point_buffer, sdk::transform_t** transform_buffer);
	}

	namespace exploits
	{
		void run();

		void hkDoAttack(il2cpp::Weapon_Firearms_o* _this, const il2cpp::MethodInfo* method);

		void hkDoJump(il2cpp::Movement_Movement_o* _this, void* unused, const il2cpp::MethodInfo* method);
	}

	namespace misc
	{
		void hkCreateClient(il2cpp::Multiplayer_Entity_Client_ClientPlayer_o* _this, const il2cpp::MethodInfo* method);

		void hkDestroyClient(il2cpp::Multiplayer_Entity_Client_ClientPlayer_o* _this, const il2cpp::MethodInfo* method);

		void hkSendMsg(il2cpp::Multiplayer_NetworkManager_o* _this, il2cpp::System_String_o* msg, int a3, const il2cpp::MethodInfo* method);

		il2cpp::System_String_o* hkSwearCheck(il2cpp::Chat_o* _this, il2cpp::System_String_o* message, const il2cpp::MethodInfo* method);

		void hkPlayVoice(il2cpp::Menu_VoiceLine_o* _this, int a2, const il2cpp::MethodInfo* method);

		void* hkShakeOnce(il2cpp::EZCameraShake_CameraShaker_o* _this, float a2, float a3, float a4, float a5, const il2cpp::MethodInfo* method);
	}
}