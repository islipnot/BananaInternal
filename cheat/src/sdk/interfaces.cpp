#include "pch.hpp"
#include "cfg.hpp"
#include "core/gui/gui.hpp"
#include "il2cpp_hlp.hpp"
#include "interfaces.hpp"

namespace sdk
{
	bool init()
	{
		// Resolving GameAssembly.dll data

		sdk::internal::hGameAssembly = GetModuleHandleW(L"GameAssembly.dll");
		if (!sdk::internal::hGameAssembly) 
			return false;

		if (!il2cpp_hlp::init())
			return false;
		
		void* temp = nullptr;

		//
		//// Assembly-CSharp.dll
		//

		if (!il2cpp_hlp::set_method("Assembly-CSharp.dll"))
			return false;

		if (!il2cpp_hlp::get_class(&sdk::main::game_mgr, "Manager", "GameManager"))
			return false;

		if (!il2cpp_hlp::get_class(&temp, "Manager", "PrefabManager"))
			return false;

		sdk::main::prefab_mgr = &reinterpret_cast<il2cpp::Manager_PrefabManager_c*>(temp)->static_fields->Instance->fields;

		if (!il2cpp_hlp::get_class(&temp, "Multiplayer", "NetworkManager"))
			return false;

		sdk::main::network_mgr = reinterpret_cast<il2cpp::Multiplayer_NetworkManager_c*>(temp)->static_fields;

		if (!il2cpp_hlp::get_class(&temp, "Weapon", "WeaponManager"))
			return false;

		sdk::main::weapon_mgr = reinterpret_cast<il2cpp::Weapon_WeaponManager_c*>(temp)->static_fields;

		if (!il2cpp_hlp::get_class(&temp, "Multiplayer.Entity.Client", "ClientPlayer"))
			return false;

		sdk::main::player_data = reinterpret_cast<decltype(sdk::main::player_data)>(reinterpret_cast<il2cpp::Multiplayer_Entity_Client_ClientPlayer_c*>(temp)->static_fields);

		if (!il2cpp_hlp::get_class(&temp, "PlayerCameraController", "MoveCamera"))
			return false;

		sdk::main::main_cam = reinterpret_cast<decltype(sdk::main::main_cam)>(&reinterpret_cast<il2cpp::PlayerCameraController_MoveCamera_c*>(temp)->static_fields->instance->fields);

		if (!il2cpp_hlp::get_class(&temp, "Movement", "PlayerMovement"))
			return false;

		sdk::main::player_movement = reinterpret_cast<decltype(sdk::main::player_movement)>(reinterpret_cast<il2cpp::Movement_PlayerMovement_c*>(temp)->static_fields);

		if (!il2cpp_hlp::get_method(&sdk::fn::toggle_noclip, "ChangeNoClip", -1, "Movement", "PlayerMovement"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::get_weapon, "GetWeapon", -1, "Multiplayer.Entity.Client", "ClientPlayer"))
			return false;
		
		//
		//// UnityEngine.CoreModule.dll
		//

		if (!il2cpp_hlp::set_method("UnityEngine.CoreModule.dll"))
			return false;

		// UnityEngine.Transform

		if (!il2cpp_hlp::set_class("UnityEngine", "Transform"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::transform_get_forward,     "get_forward"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::transform_look_at,         "LookAt", 1))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::transform_get_position,    "get_position_Injected"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::transform_set_position,    "set_position_Injected"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::transform_get_rotation,    "get_rotation_Injected"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::transform_set_rotation,    "set_rotation_Injected"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::transform_set_pos_and_rot, "SetPositionAndRotation_Injected"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::transform_rotate,          "Rotate", 3))
			return false;

		// UnityEngine.Camera

		if (!il2cpp_hlp::set_class("UnityEngine", "Camera"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::world_to_screen_point, "WorldToScreenPoint_Injected"))
			return false;

		if (!il2cpp_hlp::get_method(&sdk::fn::set_aspect_ratio,      "set_aspect"))
			return false;

		if (!il2cpp_hlp::get_method(&temp,                           "get_aspect"))
			return false;

		cfg::misc::aspect_ratio = reinterpret_cast<float(*)(void*, void*)>(temp)(sdk::main::main_cam->cam, nullptr);
		gui::box_scale = 0.954203f / cfg::misc::aspect_ratio;
		gui::health_scale = 1.10f / cfg::misc::aspect_ratio;

		// UnityEngine.Vector3

		if (!il2cpp_hlp::get_method(&sdk::fn::get_normalized_vec3, "get_normalized", -1, "UnityEngine", "Vector3"))
			return false;

		// UnityEngine.Object

		if (!il2cpp_hlp::get_method(&sdk::fn::get_object_name, "GetName", -1, "UnityEngine", "Object"))
			return false;

		//
		//// UnityEngine.PhysicsModule.dll
		//

		if (!il2cpp_hlp::set_method("UnityEngine.PhysicsModule.dll"))
			return false;

		// UnityEngine.Physics

		if (!il2cpp_hlp::get_method(&sdk::fn::physics_raycast, "Raycast", 6, "UnityEngine", "Physics"))
			return false;

		// UnityEngine.RaycastHit

		if (!il2cpp_hlp::get_method(&sdk::fn::get_raycast_rb, "get_rigidbody", -1, "UnityEngine", "RaycastHit"))
			return false;

		//
		//// icalls
		//

		if (!il2cpp_hlp::resolve_icall(&sdk::fn::set_behavior_enabled, "UnityEngine.Behaviour::set_enabled(System.Boolean)"))
			return false;
		
		return true;
	}
}