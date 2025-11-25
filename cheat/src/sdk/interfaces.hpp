#pragma once

#define METHOD const il2cpp::MethodInfo* method

namespace sdk
{
	struct GameManager_t;

	struct ClientStaticFields_t;

	struct PlayerMovement_t;

	struct camera_t;

	//
	//// TYPEDEFS
	//

	typedef il2cpp::UnityEngine_GameObject_o game_object_t;

	typedef il2cpp::UnityEngine_Collider_o collider_t;

	typedef il2cpp::Manager_PrefabManager_Fields prefab_mgr_t;

	typedef il2cpp::UnityEngine_Transform_o transform_t;

	typedef il2cpp::UnityEngine_RaycastHit_Fields raycast_hit_t;

	typedef int layer_mask_t;

	//
	//// DATA
	//

	namespace fn
	{
		inline void (*set_aspect_ratio)(il2cpp::UnityEngine_Camera_o* _this, float value, METHOD) = nullptr;

		inline void (*transform_get_forward)(glm::vec3* dst, sdk::transform_t* _this, METHOD) = nullptr;

		inline void (*transform_get_position)(const sdk::transform_t* _this, glm::vec3* dst, METHOD) = nullptr;

		inline void (*transform_set_position)(const sdk::transform_t* _this, const glm::vec3* src, METHOD) = nullptr;

		inline void (*transform_get_rotation)(const sdk::transform_t* _this, glm::vec4* dst, METHOD) = nullptr;

		inline void (*transform_set_rotation)(const sdk::transform_t* _this, const glm::vec4* src, METHOD) = nullptr;

		inline void (*transform_set_pos_and_rot)(const sdk::transform_t* _this, const glm::vec3* pos, const glm::vec4* rot, METHOD) = nullptr;

		inline void (*transform_look_at)(const sdk::transform_t* _this, const sdk::transform_t* target, METHOD) = nullptr;

		inline void (*get_normalized_vec3)(glm::vec3* dst, const glm::vec3* src, METHOD) = nullptr;

		inline void (*world_to_screen_point)(il2cpp::UnityEngine_Camera_o* _this, const glm::vec3* world_pos, int eye, glm::vec3* screen_pos) = nullptr; // eye must be mono (2)

		inline void (*set_behavior_enabled)(void* _this, bool value, METHOD) = nullptr;

		inline il2cpp::System_String_o* (*get_object_name)(void* _this, METHOD) = nullptr;

		inline bool(*physics_raycast)(glm::vec3* origin, glm::vec3* direction, sdk::raycast_hit_t* hit_info, float max_distance, sdk::layer_mask_t layer_mask, int trigger, METHOD) = nullptr;

		inline sdk::collider_t* (*raycast_get_collider)(sdk::raycast_hit_t* _this, METHOD) = nullptr;

		inline sdk::transform_t* (*raycast_get_transform)(sdk::raycast_hit_t* _this, METHOD) = nullptr;

		inline il2cpp::UnityEngine_Rigidbody_o* (*get_raycast_rb)(sdk::raycast_hit_t* hit_info, METHOD) = nullptr;

		inline void (*transform_rotate)(sdk::transform_t* _this, float x, float y, float z, METHOD) = nullptr;

		inline void (*toggle_noclip)(il2cpp::Movement_PlayerMovement_o* _this, METHOD) = nullptr;
	}

	namespace main
	{
		inline sdk::camera_t*             main_cam        = nullptr;

		inline sdk::GameManager_t*        game_mgr        = nullptr;

		inline sdk::ClientStaticFields_t* player_data     = nullptr;

		inline sdk::PlayerMovement_t*     player_movement = nullptr;

		inline sdk::prefab_mgr_t*         prefab_mgr      = nullptr;
	}

	namespace internal
	{
		inline HMODULE hGameAssembly = nullptr;

		inline std::mutex player_mutex;
	}

	//
	//// FORWARD DECLARATIONS
	//

	bool init();

	//
	//// TYPES
	//

	struct GameManager_t : il2cpp::Manager_GameManager_c
	{
		[[nodiscard]] bool is_in_game() const
		{
			return this->static_fields->on_setting_loaded != nullptr;
		}
	};

	struct ClientFields_t : il2cpp::Multiplayer_Entity_Client_ClientPlayer_Fields
	{
	private:

		enum outline_types_t : int
		{
			outline_none,
			outline_death,
			outline_invincible,
			outline_team,
			outline_infected,
			outline_has_banana
		};

	public:

		[[nodiscard]] bool is_teammate() const
		{
			return this->player_state->fields.outline_type == outline_team;
		}

		[[nodiscard]] bool is_invincible() const
		{
			return this->player_state->fields.outline_type == outline_invincible;
		}

		[[nodiscard]] bool is_bad_targ() const
		{
			return this->is_local || this->is_dead || !this->orientation || !this->head || !this->player_state;
		}

		[[nodiscard]] int max_health() const
		{
			return this->player_state->fields.max_health;
		}

		[[nodiscard]] int health() const
		{
			return this->player_state->fields.health;
		}

		[[nodiscard]] bool get_bone(uint32_t desired_bone, glm::vec3* pos_buffer, sdk::transform_t** transform_buffer) const
		{
			const auto bone_array = this->player_state->fields.bones->fields._items;

			for (int i = 0, sz = static_cast<int>(bone_array->max_length); i < sz; ++i)
			{
				const auto transform = bone_array->m_Items[i];
				const auto name = sdk::fn::get_object_name(transform, nullptr);

				const uint32_t bone_hash = hash_rt(reinterpret_cast<PCWSTR>(&name->fields._firstChar), name->fields._stringLength);

				if (bone_hash == desired_bone)
				{
					if (pos_buffer)
					{
						sdk::fn::transform_get_position(transform, pos_buffer, nullptr);
					}
					if (transform_buffer)
					{
						*transform_buffer = transform;
					}

					return true;
				}
			}

			return false;
		}

		[[nodiscard]] bool get_pos(glm::vec3* dst) const
		{
			const std::lock_guard guard(sdk::internal::player_mutex);

			if (!this->orientation)
			{
				return false;
			}

			sdk::fn::transform_get_position(this->orientation, dst, nullptr);
			return true;
		}
	};

	struct player_list_t
	{
		struct player_entry_t // System_Collections_Generic_Dictionary_Entry_TKey__TValue__Fields
		{
		private:

			char padding[0x10];

		public:

			il2cpp::Multiplayer_Entity_Client_ClientPlayer_o* obj;

			[[nodiscard]] sdk::ClientFields_t* get_client_fields() const
			{
				if (!this->obj) return nullptr;
				return reinterpret_cast<sdk::ClientFields_t*>(&this->obj->fields);
			}
		};

		player_entry_t* entries;
		int count;
	};

	struct ClientStaticFields_t : il2cpp::Multiplayer_Entity_Client_ClientPlayer_StaticFields
	{
		[[nodiscard]] sdk::ClientFields_t* local_player() const
		{
			return this->instance ? reinterpret_cast<sdk::ClientFields_t*>(&this->instance->fields) : nullptr;
		}

		bool get_player_list(sdk::player_list_t* dst) const
		{
			if (!this->list) return false;

			const std::lock_guard guard(sdk::internal::player_mutex);

			const int player_count = this->list->fields._count;
			if (player_count < 2) return false;

			const auto player_list = this->list->fields._entries;
			if (!player_list) return false;

			dst->count   = player_count;
			dst->entries = reinterpret_cast<sdk::player_list_t::player_entry_t*>(player_list->m_Items);

			return true;
		}
	};

	struct PlayerMovement_t : il2cpp::Movement_PlayerMovement_StaticFields
	{
		// Get local pitch/yaw
		void get_view_angles(angle_info_t* buffer) const
		{
			auto& f = this->Instance->fields;
			buffer->pitch = f.pitch;
			buffer->yaw   = f.yaw;
		}

		// Set local pitch/yaw
		void set_view_angles(const angle_info_t& angles)
		{
			auto& f = this->Instance->fields;
			f.pitch = angles.pitch;
			f.yaw   = angles.yaw;
		}
	};

	struct camera_t : il2cpp::PlayerCameraController_MoveCamera_Fields
	{
		// Get camera forward
		void get_forward(glm::vec3* dst) const
		{
			sdk::fn::transform_get_forward(dst, this->cam_transform, nullptr);
		}

		// Get camera world position
		void get_pos(glm::vec3* dst) const
		{
			sdk::fn::transform_get_position(this->cam_transform, dst, nullptr);
		}

		// Get camera rotation
		void get_rot(glm::vec4* dst) const
		{
			sdk::fn::transform_get_rotation(this->cam_transform, dst, nullptr);
		}

		// Set camera world position
		void set_pos(const glm::vec3* src)
		{
			sdk::fn::transform_set_position(this->cam_transform, src, nullptr);
		}

		// Set camera rotation
		void set_rot(const glm::vec4* src)
		{
			sdk::fn::transform_set_rotation(this->cam_transform, src, nullptr);
		}
	};
}